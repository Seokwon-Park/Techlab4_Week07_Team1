#pragma pack_matrix(row_major)

#define TILE_SIZE 16
#define THREADS_PER_TILE (TILE_SIZE * TILE_SIZE)
#define MAX_LIGHTS_PER_TILE 255
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

struct FLocalLightInfo
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
    uint NumPointLights;
}

StructuredBuffer<FPointLightInfo> PointLights : register(t10);
RWStructuredBuffer<uint> TileLightIndices : register(u0);

groupshared uint sLightCount;
groupshared uint sLightIndices[MAX_LIGHTS_PER_TILE];

float4 NormalizePlane(float4 P)
{
    return P / length(P.xyz);
}

void BuildTileFrustum(uint2 TileID, out float4 Planes[6])
{
    // Projection 행렬의 열 성분을 index로 한번에 가져오기 위해서 
    float4x4 ProjTrans = transpose(Projection);
    
    float2 PixMin = TileID * TILE_SIZE;
    float2 PixMax = PixMin + TILE_SIZE;
    float L = 2.0f * PixMin.x / ViewportSize.x - 1.0f;
    float R = 2.0f * PixMax.x / ViewportSize.x - 1.0f;
    float T = 1.0f - 2.0f * PixMin.y / ViewportSize.y;
    float B = 1.0f - 2.0f * PixMax.y / ViewportSize.y;
    
    Planes[0] = NormalizePlane(ProjTrans[0] - L * ProjTrans[3]);
    Planes[1] = NormalizePlane(R * ProjTrans[3] - ProjTrans[0]);
    Planes[2] = NormalizePlane(ProjTrans[1] - B * ProjTrans[3]);
    Planes[3] = NormalizePlane(T * ProjTrans[3] - ProjTrans[1]);
    Planes[4] = NormalizePlane(ProjTrans[2]); // Near
    Planes[5] = NormalizePlane(ProjTrans[3] - ProjTrans[2]); // Far
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

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void mainCS(uint3 GroupID : SV_GroupID, uint GroupIndex : SV_GroupIndex)
{
    // 그룹 내에서 공유되는 공용 카운터는 한 스레드만 초기화
    // 스레드 간 순서가 보장되지 않지만 Barrier를 통해 0으로 초기화가 끝날때까지 대기한다.
    if (GroupIndex == 0)
        sLightCount = 0;
    GroupMemoryBarrierWithGroupSync();
    
    //Tile Frustum
    float4 Planes[6];
    BuildTileFrustum(GroupID.xy, Planes);
    
    //타일 내부의 256개 스레드는 라이트를 나눠서 동시에 검사한다.
    //스레드는 256개가 끝이지만 Light는 1024개까지 들어갈 수 있다.
    //따라서 0번 스레드가 0,256,512,768.. 이런식으로 각각 맡아서 검사한다.
    for (uint i = GroupIndex; i < NumPointLights; i += THREADS_PER_TILE)
    {
        FPointLightInfo Light = PointLights[i];
        float3 CenterVS = mul(float4(Light.Position, 1.0f), View).xyz;
        
        //i번째 라이트의 영향 범위(구)가 이 타일 프러스텀과 겹치는지 검사
        if (SphereInFrustum(CenterVS, Light.Radius, Planes))
        {
            uint Slot;
            InterlockedAdd(sLightCount, 1, Slot);
            //진행 순서 sLightCount의 값을 Slot에 저장
            //sLightCount에 value만큼 더한 값을 저장
            //원자적 연산
            if (Slot < MAX_LIGHTS_PER_TILE)
                sLightIndices[Slot] = i; // 칸에 라이트 번호를 적는다
        }
    }
    GroupMemoryBarrierWithGroupSync();
    
    // 타일(그룹)의 2D 좌표 -> 1D 타일 번호
    uint TileIndex = GroupID.y * NumTilesX + GroupID.x;
    // 전역 버퍼에서 이 타일 구역의 시작 칸. 타일 내 모든 스레드가 같은 값
    uint Base = TileIndex * TILE_STRIDE;
    // 타일 전체가 함께 센 개수. 상한을 넘었으면 잘라냄
    uint Count = min(sLightCount, MAX_LIGHTS_PER_TILE);

    // 구역 첫 칸 [Base]에 개수를 기록. 한 칸이라 한 스레드만 쓴다
    if (GroupIndex == 0)
        TileLightIndices[Base] = Count;

    // 라이트 번호 목록을 [Base+1]부터 복사. 0번 포함 모든 스레드가 j번째 항목을 나눠서 쓴다
    for (uint j = GroupIndex; j < Count; j += THREADS_PER_TILE)
        TileLightIndices[Base + 1 + j] = sLightIndices[j];
}