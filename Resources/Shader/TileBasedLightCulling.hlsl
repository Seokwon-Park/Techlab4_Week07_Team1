#pragma pack_matrix(row_major)

#define LIGHT_TYPE_POINT 1   
#define LIGHT_TYPE_SPOT  2
#define LIGHT_TYPE_RECT  3

#define TILE_SIZE 64
#define THREADS_PER_CELL 64
#define MAX_LIGHTS_PER_TILE 32
#define TILE_STRIDE (MAX_LIGHTS_PER_TILE + 1)

struct FPointLightInfo
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
    float FalloffExponent;
    float3 Padding;
};

struct FLightRenderParameters
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
    float3 Direction;
    float InnerConeCos;
    float3 Right;
    float FalloffExponent;
    float OuterConeCos;
    float HalfWidth;
    float HalfHeight;
    uint Type;
};

cbuffer FLightCullingConstants : register(b0)
{
    float4x4 View;
    float4x4 Projection;
    float2 ViewportSize;
    uint NumTilesX;
    uint NumTilesY;
    uint NumLocalLights;
    uint NumSlices;
    uint TileSize;
    uint bOrthographic;
    float NearZ;
    float FarZ;
    float2 Padding;
};

StructuredBuffer<FLightRenderParameters> LocalLights : register(t10);
RWStructuredBuffer<uint> TileLightIndices : register(u0);

groupshared uint sLightCount;
groupshared uint sLightIndices[MAX_LIGHTS_PER_TILE];

float SliceBoundary(uint Slice)
{
    const float t = (float) Slice / NumSlices;
    return bOrthographic ? lerp(NearZ, FarZ, t) : NearZ * pow(FarZ / NearZ, t);
}

void GetCullSphere(FLightRenderParameters L, out float3 Center, out float Radius)
{
    Center = L.Position;
    Radius = L.Radius;

    if (L.Type == LIGHT_TYPE_RECT)
    {
        // 사각형 위 가장 가까운 점부터 Radius만큼 닿으므로 대각선 반만큼 늘린다
        Radius += length(float2(L.HalfWidth, L.HalfHeight));
    }
    else if (L.Type == LIGHT_TYPE_SPOT)
    {
        const float CosA = L.OuterConeCos;
        const float SinA = sqrt(saturate(1.0f - CosA * CosA));

        if (CosA < 0.70710678f)          // 바깥 각도 > 45도: 넓은 원뿔
        {
            Center = L.Position + L.Direction * (L.Radius * CosA);
            Radius = L.Radius * SinA;
        }
        else // 바깥 각도 <= 45도: 좁은 원뿔
        {
            Center = L.Position + L.Direction * (L.Radius / (2.0f * CosA));
            Radius = L.Radius / (2.0f * CosA);
        }
    }
}

float4 NormalizePlane(float4 P)
{
    return P / length(P.xyz);
}

void BuildTileFrustum(uint3 GroupID, out float4 Planes[6])
{
    // Projection 행렬의 열 성분을 index로 한번에 가져오기 위해서 
    float4x4 ProjTrans = transpose(Projection);
    
    float2 PixMin = GroupID.xy * TileSize;
    float2 PixMax = PixMin + TileSize;
    float L = 2.0f * PixMin.x / ViewportSize.x - 1.0f;
    float R = 2.0f * PixMax.x / ViewportSize.x - 1.0f;
    float T = 1.0f - 2.0f * PixMin.y / ViewportSize.y;
    float B = 1.0f - 2.0f * PixMax.y / ViewportSize.y;
    
    Planes[0] = NormalizePlane(ProjTrans[0] - L * ProjTrans[3]);
    Planes[1] = NormalizePlane(R * ProjTrans[3] - ProjTrans[0]);
    Planes[2] = NormalizePlane(ProjTrans[1] - B * ProjTrans[3]);
    Planes[3] = NormalizePlane(T * ProjTrans[3] - ProjTrans[1]);
    Planes[4] = float4(1, 0, 0, -SliceBoundary(GroupID.z));
    Planes[5] = float4(-1, 0, 0, SliceBoundary(GroupID.z + 1));
}

bool SphereInFrustum(float3 CenterVS, float Radius, float4 Planes[6])
{
    [unroll]
    for (int i = 0; i < 6; ++i)
    {
        if (dot(Planes[i].xyz, CenterVS) + Planes[i].w < -Radius)
            return false;
    }
    return true;
}

void BuildCellAABB(uint3 GroupID, out float3 Min, out float3 Max)
{
    float2 PixMin = GroupID.xy * TileSize;
    float2 PixMax = PixMin + TileSize;
    float L = 2.0f * PixMin.x / ViewportSize.x - 1.0f;
    float R = 2.0f * PixMax.x / ViewportSize.x - 1.0f;
    float T = 1.0f - 2.0f * PixMin.y / ViewportSize.y;
    float B = 1.0f - 2.0f * PixMax.y / ViewportSize.y;
    // L R T B는 BuildTileFrustum과 같은 식
    float Near = SliceBoundary(GroupID.z);
    float Far = SliceBoundary(GroupID.z + 1);
    
    const float Depths[2] = { Near, Far };
    const float Us[2] = { L, R };
    const float Vs[2] = { T, B };

    Min = 1e30f;
    Max = -1e30f;
    if (bOrthographic)
    {
    // clip.x = Y * XScale + XOffset  →  Y = (u - XOffset) / XScale
        const float YMin = (L - Projection[3][0]) / Projection[1][0];
        const float YMax = (R - Projection[3][0]) / Projection[1][0];
        const float ZMin = (B - Projection[3][1]) / Projection[2][1];
        const float ZMax = (T - Projection[3][1]) / Projection[2][1];

        Min = float3(Near, YMin, ZMin);
        Max = float3(Far, YMax, ZMax);
    }
    else
    {
        [unroll]
        for (uint di = 0; di < 2; ++di)
            [unroll]
            for (uint ui = 0; ui < 2; ++ui)
                [unroll]
                for (uint vi = 0; vi < 2; ++vi)
                {
                    const float d = Depths[di];
                    const float3 P = float3(d, Us[ui] * d / Projection[1][0], Vs[vi] * d / Projection[2][1]);
                    Min = min(Min, P);
                    Max = max(Max, P);
                }
    }
    

}

bool SphereIntersectsAABB(float3 C, float R, float3 Min, float3 Max)
{
    float3 D = max(0, max(Min - C, C - Max));
    return dot(D, D) <= R * R;
}

// Bart Wronski - Cull that Cone
bool SphereOutsideCone(float3 ConePos, float3 ConeDir, float Range,
                       float CosA, float SinA, float3 S, float SR)
{
    const float3 V = S - ConePos;
    const float VLen = dot(V, ConeDir);
    const float Perp = sqrt(max(dot(V, V) - VLen * VLen, 0.0f));
    const float Dist = CosA * Perp - SinA * VLen;

    const bool bOutsideSide = Dist > SR;
    const bool bBeyondRange = VLen > Range + SR;
    const bool bBehindApex = VLen < -SR;
    return bOutsideSide || bBeyondRange || bBehindApex;
}

[numthreads(THREADS_PER_CELL, 1, 1)]
void mainCS(uint3 GroupID : SV_GroupID, uint GroupIndex : SV_GroupIndex)
{
    // 그룹 내에서 공유되는 공용 카운터는 한 스레드만 초기화
    // 스레드 간 순서가 보장되지 않지만 Barrier를 통해 0으로 초기화가 끝날때까지 대기한다.
    if (GroupIndex == 0)
        sLightCount = 0;
    GroupMemoryBarrierWithGroupSync();
    
    //Tile Frustum
    float4 Planes[6];
    BuildTileFrustum(GroupID, Planes);
    
    float3 CellMin, CellMax;
    BuildCellAABB(GroupID, CellMin, CellMax);
    
    const float3 CellCenter = (CellMin + CellMax) * 0.5f;
    const float CellRadius = length(CellMax - CellMin) * 0.5f;
    
    //타일 내부의 256개 스레드는 라이트를 나눠서 동시에 검사한다.
    //스레드는 256개가 끝이지만 Light는 1024개까지 들어갈 수 있다.
    //따라서 0번 스레드가 0,256,512,768.. 이런식으로 각각 맡아서 검사한다.
    for (uint i = GroupIndex; i < NumLocalLights; i += THREADS_PER_CELL)
    {
        const FLightRenderParameters L = LocalLights[i];
        
        float3 Center;
        float CullRadius;
        GetCullSphere(L, Center, CullRadius);
        
        float3 CenterVS = mul(float4(Center, 1.0f), View).xyz;
        
        //i번째 라이트의 영향 범위(구)가 이 타일 프러스텀과 겹치는지 검사
        if (!SphereInFrustum(CenterVS, CullRadius, Planes) || !SphereIntersectsAABB(CenterVS, CullRadius, CellMin, CellMax))
            continue;
        
        if (L.Type == LIGHT_TYPE_SPOT)
        {
            const float3 ConePos = mul(float4(L.Position, 1.0f), View).xyz;
            const float3 ConeDir = normalize(mul(float4(L.Direction, 0.0f), View).xyz);
            const float SinA = sqrt(saturate(1.0f - L.OuterConeCos * L.OuterConeCos));
            if (SphereOutsideCone(ConePos, ConeDir, L.Radius, L.OuterConeCos, SinA, CellCenter, CellRadius))
                continue;
        }
        
        uint Slot;
        InterlockedAdd(sLightCount, 1, Slot);
            //진행 순서 sLightCount의 값을 Slot에 저장
            //sLightCount에 value만큼 더한 값을 저장
            //원자적 연산
        if (Slot < MAX_LIGHTS_PER_TILE)
            sLightIndices[Slot] = i; // 칸에 라이트 번호를 적는다
    }
    GroupMemoryBarrierWithGroupSync();
    
    // 타일(그룹)의 2D 좌표 -> 1D 타일 번호
    uint CellIndex = (GroupID.z * NumTilesY + GroupID.y) * NumTilesX + GroupID.x;
    // 전역 버퍼에서 이 타일 구역의 시작 칸. 타일 내 모든 스레드가 같은 값
    uint Base = CellIndex * TILE_STRIDE;
    // 타일 전체가 함께 센 개수. 상한을 넘었으면 잘라냄
    uint Count = min(sLightCount, MAX_LIGHTS_PER_TILE);

    // 구역 첫 칸 [Base]에 개수를 기록. 한 칸이라 한 스레드만 쓴다
    if (GroupIndex == 0)
        TileLightIndices[Base] = Count;

    // 라이트 번호 목록을 [Base+1]부터 복사. 0번 포함 모든 스레드가 j번째 항목을 나눠서 쓴다
    for (uint j = GroupIndex; j < Count; j += THREADS_PER_CELL)
        TileLightIndices[Base + 1 + j] = sLightIndices[j];
}