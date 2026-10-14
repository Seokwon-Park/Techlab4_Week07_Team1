#pragma pack_matrix(row_major)

#define LIGHT_TYPE_POINT 1   // C++ ELightType과 같아야 함
#define LIGHT_TYPE_SPOT  2
#define LIGHT_TYPE_RECT  3

#define MAX_LIGHTS_PER_CELL 32
#define CELL_STRIDE (MAX_LIGHTS_PER_CELL + 1)

// 조명 모델은 C++(FRenderResourceManager::LoadUberLit)이 매크로로 하나만 넘긴다.
// 아무 매크로 없이 컴파일될 때(기존 머티리얼이 쓰는 기본 키)만 Lambert를 켠다.
#if !defined(LIGHTING_MODEL_UNLIT) && !defined(LIGHTING_MODEL_GOURAUD) && \
    !defined(LIGHTING_MODEL_LAMBERT) && !defined(LIGHTING_MODEL_BLINN_PHONG) && \
    !defined(WORLD_NORMAL)
#define LIGHTING_MODEL_LAMBERT 1
#endif

// 아래 구조체와 cbuffer Lighting은 Renderer.h의 FLightingConstants와 바이트 단위로 같아야 한다.
struct FAmbientLightInfo
{
    float3 Color;
    float Intensity;
};

struct FDirectionalLightInfo
{
    float3 Direction; // 빛이 향하는 방향
    float Intensity;
    float3 Color;
    float Padding;
};

struct FPointLightInfo
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
    float FalloffExponent;
    float3 Padding;
};

struct FRectLightInfo
{
    float3 Position;   float Radius;
    float3 Color;      float Intensity;
    float3 Direction;  float HalfWidth;
    float3 Right;      float HalfHeight;
    float BarnDoorCos; float BarnDoorSin;
    float BarnDoorLength; float Padding;
};

struct FSpotLightInfo
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
    float3 Direction;
    float InnerConeCos;
    float FalloffExponent;
    float OuterConeCos;
    float2 Padding;
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



// Renderer.h의 FViewConstants와 바이트 단위로 같아야 한다. VS와 PS 둘 다 b0에 바인딩된다.
cbuffer Viewconstants : register(b0)
{
    matrix VP;
    float3 CameraWorldPos; // Specular의 V = normalize(CameraWorldPos - worldPos)
    float ViewPadding;
};

cbuffer MaterialParams : register(b1)
{
    float4 BaseColor;
    float2 UVOffset;
    float bOpaque;
    float Shininess; // Blinn-Phong 광택 지수 (Material.h FStaticMeshMaterialParams::Shininess)
};

cbuffer Worldconstants : register(b2)
{
    matrix World;
    matrix WorldInverseTranspose; // (World^-1)^T
};

// b3는 FireBall이 쓰는 동안 비워 둔다.

cbuffer Lighting : register(b4)
{
    FAmbientLightInfo Ambient;
    FDirectionalLightInfo Directional;
    uint NumLocalLights;
    uint3 LightingPadding;
};


cbuffer LightGrid : register(b5)
{
    uint NumTilesX;
    uint NumTilesY;
    uint NumSlices;
    uint LightDebugMode;
    float NearZ;
    float FarZ;
    float SliceScale;
    float SliceBias;
    uint TileSize;
    uint bOrthographic;
    uint2 LightGridPadding;
};

StructuredBuffer<FLightRenderParameters> LocalLights : register(t10);
StructuredBuffer<uint> TileLightIndices : register(t12);

Texture2D g_txColor : register(t0);
Texture2D NormalTexture : register(t1);
SamplerState g_Sample : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
#if LIGHTING_MODEL_GOURAUD
    float3 vertexLighting : COLOR1;
#endif
};

// Diffuse와 Specular를 담는 구조체
struct FLightContribution
{
    float3 Diffuse;
    float3 Specular;
};

// 원뿔 감쇠량
float SpotConeFactor(FLightRenderParameters S, float3 L)
{
    return smoothstep(S.OuterConeCos, S.InnerConeCos, dot(-L, S.Direction));
}

// L 벡터와 Attenuation을 구한다.
float LocalLightAttenuation(float3 LightPos, float Radius, float FalloffExp, float3 WorldPos, out float3 L)
{
    const float3 ToLight = LightPos - WorldPos; //픽셀의 월드 좌표에서 빛을 향하는 벡터를 구한다.
    const float Dist = length(ToLight); // 벡터 길이
    L = ToLight / max(Dist, 0.0001f); // normalize 벡터
    const float Atten = pow(saturate(1.0f - Dist / max(Radius, 0.001f)), max(FalloffExp, 0.001f)); // 거리와 radius에 비례한 강도 계산
    return Atten;
}



// Diffuse와 Specular를 구한다. Radiance = Color * Intensity * Falloff (* ConeFactor)
FLightContribution ShadeLight(float3 N, float3 L, float3 V, float3 Radiance, float InShininess)
{
    float3 NDotL = saturate(dot(normalize(N), normalize(L)));
    FLightContribution Contribution = (FLightContribution) 0;
    Contribution.Diffuse = NDotL;

#if LIGHTING_MODEL_BLINN_PHONG
    // Blinn Specular
    float3 H = normalize(L + V);
    if (dot(N, L) > 0)
    {
        Contribution.Specular = pow(saturate(dot(normalize(N), H)), max(InShininess, 1.f));
    }
#endif
    Contribution.Diffuse *= Radiance;
    Contribution.Specular *= Radiance;
    return Contribution;
}

// 표면에 들어오는 빛의 양만 돌려준다. 표면색(albedo)은 호출한 쪽에서 곱한다.
float3 CalcAmbient()
{
    return Ambient.Color * Ambient.Intensity;
}

// Directional Light의 영향을 구한다
FLightContribution CalcDirectional(float3 N, float3 V, float InShininess)
{
    float3 Radiance = Directional.Color * Directional.Intensity;
    return ShadeLight(N, -Directional.Direction, V, Radiance, InShininess);
}

// 해당 WorldPos에서의 각 PointLight의 영향(밝기)를 구한다
FLightContribution CalcPoint(FLightRenderParameters P, float3 N, float3 V, float3 WorldPos, float InShininess)
{
    float3 L;
    const float Atten = LocalLightAttenuation(P.Position, P.Radius, P.FalloffExponent, WorldPos, L);
    return ShadeLight(N, L, V, P.Color * P.Intensity * Atten, InShininess);
}

// Spot은 Point에서 ConeFactor만 곱하면 된다
FLightContribution CalcSpot(FLightRenderParameters S, float3 N, float3 V, float3 WorldPos, float InShininess)
{
    float3 L;
    float Attenuation = LocalLightAttenuation(S.Position, S.Radius, S.FalloffExponent, WorldPos, L);
    return ShadeLight(N, L, V, S.Color * S.Intensity * Attenuation * SpotConeFactor(S, L), InShininess);
}

FLightContribution CalcRect(FLightRenderParameters R, float3 N, float3 V, float3 WorldPos, float InShininess)
{
    const float3 D = WorldPos - R.Position;
    if (dot(D, R.Direction) <= 0.0f)                    // 면광원 뒤쪽
        return (FLightContribution) 0;

    const float3 Up = cross(R.Direction, R.Right);
    const float x = clamp(dot(D, R.Right), -R.HalfWidth, R.HalfWidth);
    const float y = clamp(dot(D, Up), -R.HalfHeight, R.HalfHeight);
    const float3 SamplePos = R.Position + R.Right * x + Up * y; // 사각형 위에서 가장 가까운 점

    float3 L;
    const float Atten = LocalLightAttenuation(SamplePos, R.Radius, R.FalloffExponent, WorldPos, L);
    return ShadeLight(N, L, V, R.Color * R.Intensity * Atten, InShininess);
}

FLightContribution CalcLocalLight(FLightRenderParameters Light, float3 N, float3 V, float3 WorldPos, float InShininess)
{
    FLightContribution Result;
    if (Light.Type == LIGHT_TYPE_SPOT)
        Result = CalcSpot(Light, N, V, WorldPos, InShininess);
    else if (Light.Type == LIGHT_TYPE_RECT)
        Result = CalcRect(Light, N, V, WorldPos, InShininess);
    else
        Result = CalcPoint(Light, N, V, WorldPos, InShininess);
    return Result;
}

uint GetSlice(float4 SVPosition)
{
    float Slice;
    if (bOrthographic)
    {
        // 직교: SV_Position.z(0~1)가 Near~Far에 선형으로 대응
        Slice = SVPosition.z * NumSlices;
    }
    else
    {
        // 원근: SV_Position.w = 클립 공간 w = View 깊이(X)
        Slice = log(SVPosition.w) * SliceScale + SliceBias;
    }
    return (uint) clamp(Slice, 0.0f, NumSlices - 1.0f);
}

uint GetTileBase(float4 SVPosition)
{
    const uint2 Tile = (uint2) SVPosition.xy / TileSize;
    const uint Slice = GetSlice(SVPosition);
    return ((Slice * NumTilesY + Tile.y) * NumTilesX + Tile.x) * CELL_STRIDE;
}

float3 HeatColor(uint Count)
{
    if (Count == 0)
        return float3(0, 0, 0);
    if (Count >= 32)
        return float3(1, 1, 1);

    float t = Count / 32.0f;
    return saturate(float3(t * 2 - 0.5, 1 - abs(t * 2 - 1), 1.5 - t * 2));
}

FLightContribution CalcRect(FRectLightInfo R, float3 N, float3 V, float3 WorldPos, float InShininess)
{
    const float3 Up = cross(R.Direction, R.Right);  // Z축
    const float3 D  = WorldPos - R.Position;

    // 1) 가장 가까운 점 찾기
    const float x = clamp(dot(D, R.Right), -R.HalfWidth,  R.HalfWidth);     // 사각형 가로 좌표
    const float y = clamp(dot(D, Up),      -R.HalfHeight, R.HalfHeight);    // 사각형 세로 좌표
    const float3 Closest = R.Position + R.Right * x + Up * y;

    // 2) 가장 가까운 점을 점광원처럼 취급
    float3 L;   // 표면 -> 빛
    const float Atten = LocalLightAttenuation(Closest, R.Radius, 1.0f, WorldPos, L);

    // 3) 면의 앞쪽만 비춘다 (뒤쪽이면 0)
    const float FrontFace = saturate(dot(-L, R.Direction));

    // 4) 문 안쪽(프러스텀)만 통과
    float Mask = 1.0f;

    // Door Length가 0이면 그냥 rect (전방향)
    if (R.BarnDoorLength > 0.0f)
    {
        const float3 Out = WorldPos - Closest;                 // 발광면에서 표면으로
        const float Dist = max(length(Out), 0.0001f);
        const float Fwd  = dot(Out, R.Direction);
        const float LatX = abs(dot(Out, R.Right));
        const float LatY = abs(dot(Out, Up));
        // 옆으로 벌어진 각도가 frustum 안인가?
        const float SlackX = (Fwd * R.BarnDoorSin - LatX * R.BarnDoorCos) / Dist;
        const float SlackY = (Fwd * R.BarnDoorSin - LatY * R.BarnDoorCos) / Dist;
        // Slack이 −0.05보다 작으면 0
        // x,y 모두 닿아야 빛이 닿으므로 곱으로 마스크 만들기
        Mask = smoothstep(-0.05f, 0.0f, SlackX) * smoothstep(-0.05f, 0.0f, SlackY);
    }

    const float3 Radiance = R.Color * R.Intensity * Atten * FrontFace * Mask;
    return ShadeLight(N, L, V, Radiance, InShininess);
}

// bTiled = true면 SVPosition이 속한 타일의 PointLight만, false면 전체 PointLight를 순회한다.
// VS(Gouraud)는 픽셀 위치가 없어 타일을 쓸 수 없으므로 false로 호출한다.
FLightContribution CalcLighting(float3 N, float3 V, float3 WorldPos, float InShininess, float4 SVPosition, bool Tiled)
{
    // Directional Light는 초기에 한 번만 계산
    FLightContribution Contribution = CalcDirectional(N, V, InShininess);
    
    uint LightCount = NumLocalLights;
    uint TileBase = 0;
    bool UseTileList = false;
    if (Tiled && NumTilesX > 0 && LightDebugMode != 2)
    {
        TileBase = GetTileBase(SVPosition);
        const uint Count = TileLightIndices[TileBase];
        if (Count < MAX_LIGHTS_PER_CELL)
        {
            LightCount = Count;
            UseTileList = true;
        }
    }

    for (uint Index = 0; Index < LightCount; ++Index)
    {
        const uint LightIndex = UseTileList ? TileLightIndices[TileBase + 1 + Index] : Index;
        const FLightContribution Temp = CalcLocalLight(LocalLights[LightIndex], N, V, WorldPos, InShininess);
        Contribution.Diffuse += Temp.Diffuse;
        Contribution.Specular += Temp.Specular;
    }

    return Contribution;
}

// mainVS
PS_INPUT mainVS(VS_INPUT Input)
{
    PS_INPUT Output;

    const float4 WorldPos = mul(float4(Input.position, 1.0f), World);
    Output.position = mul(WorldPos, VP);
    Output.worldPos = WorldPos.xyz;

    // 노멀은 역전치로, 탄젠트는 표면을 따라 놓인 방향이라 World로 변환한다.
    Output.normal = mul(float4(Input.normal, 0.0f), WorldInverseTranspose).xyz;
    Output.tangent = float4(mul(float4(Input.tangent.xyz, 0.0f), World).xyz, Input.tangent.w * (determinant((float3x3) World) < 0.0f ? -1.0f : 1.0f));

    Output.uv = Input.uv;
    Output.color = Input.color;
#if LIGHTING_MODEL_GOURAUD
    float3 V = normalize(CameraWorldPos - Output.worldPos);
    FLightContribution Contribution = CalcLighting(normalize(Output.normal), V, Output.worldPos, 1, Output.position, false);
    Output.vertexLighting = Contribution.Diffuse;
#endif
    return Output;
}

float4 mainPS(PS_INPUT Input) : SV_TARGET
{
    const float2 UV = Input.uv + UVOffset;
    const float4 Albedo = g_txColor.Sample(g_Sample, UV) * BaseColor;
#if !defined(EDITOR_ARROW_SHADER)
    if (LightDebugMode == 1 && NumTilesX > 0)
    {
        const uint Count = TileLightIndices[GetTileBase(Input.position)];
        return float4(lerp(Albedo.rgb * 0.2f, HeatColor(Count), 0.7f), 1.0f);
    }
    // 슬라이스 시각화: 이웃 슬라이스끼리 색이 크게 달라지도록 황금비로 색상환을 돈다.
    if (LightDebugMode == 3 && NumTilesX > 0)
    {
        const float Hue = frac(GetSlice(Input.position) * 0.618034f);
        const float3 SliceColor = saturate(abs(frac(Hue + float3(0.0f, 2.0f, 1.0f) / 3.0f) * 6.0f - 3.0f) - 1.0f);
        return float4(lerp(Albedo.rgb * 0.2f, SliceColor, 0.7f), 1.0f);
    }
#endif

    const float3 VertexNormal = { normalize(Input.normal) };
#if defined(LIGHTING_MODEL_LAMBERT) || defined(LIGHTING_MODEL_BLINN_PHONG) || defined(WORLD_NORMAL)
    const float3 T = { normalize(Input.tangent.xyz - VertexNormal * dot(VertexNormal, Input.tangent.xyz)) };
    const float3 B = { cross(VertexNormal, T) * (Input.tangent.w < 0.0f ? -1.0f : 1.0f) };
    const float3 TangentNormal = { NormalTexture.Sample(g_Sample, UV).xyz * 2.0f - 1.0f };
    const float3 N = { normalize(T * TangentNormal.x + B * -TangentNormal.y + VertexNormal * TangentNormal.z) };
#endif

    // Opaque는 알파를 1로 고정한다. 뷰포트 RT를 ImGui가 알파 블렌딩으로 그리므로 알파가 남으면 비쳐 보인다
    const float Alpha = { bOpaque > 0.5f ? 1.0f : Albedo.a };

#if LIGHTING_MODEL_UNLIT
    return float4(Albedo.rgb, Alpha);
#elif WORLD_NORMAL
    return float4(N * 0.5f + 0.5f, 1.0f);
#else
    float3 Lighting = { CalcAmbient() };
    float3 Specular = { 0.0f, 0.0f, 0.0f };

#if defined(LIGHTING_MODEL_LAMBERT) || defined(LIGHTING_MODEL_BLINN_PHONG)
    const float3 V = { normalize(CameraWorldPos - Input.worldPos) };
    const FLightContribution Contribution = { CalcLighting(N, V, Input.worldPos, Shininess, Input.position, true) };
    Lighting += Contribution.Diffuse;
    Specular = Contribution.Specular;
#elif LIGHTING_MODEL_GOURAUD
    Lighting += Input.vertexLighting;
#endif

    return float4(Albedo.rgb * Lighting + Specular, Alpha);
#endif
}
