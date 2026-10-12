#pragma pack_matrix(row_major)

// 조명 모델은 C++(FRenderResourceManager::LoadUberLit)이 매크로로 하나만 넘긴다.
// 아무 매크로 없이 컴파일될 때(기존 머티리얼이 쓰는 기본 키)만 Lambert를 켠다.
#if !defined(LIGHTING_MODEL_UNLIT) && !defined(LIGHTING_MODEL_GOURAUD) && \
    !defined(LIGHTING_MODEL_LAMBERT) && !defined(LIGHTING_MODEL_PHONG) && \
    !defined(WORLD_NORMAL)
#define LIGHTING_MODEL_LAMBERT 1
#endif

// C++ MaxPointLights / MaxSpotLights와 같아야 한다.
#define NUM_POINT_LIGHT 4
#define NUM_SPOT_LIGHT 4

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

struct FSpotLightInfo
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
    float3 Direction;
    float InnerConeCos;
    float FalloffExponent;
    float2 Padding;
    float OuterConeCos;
};

struct FRectLightInfo
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
    float3 Direction;
    float InnerConeCos;
    float FalloffExponent;
    float2 Padding;
    float OuterConeCos;
};


cbuffer Viewconstants : register(b0)
{
    matrix VP;
};

cbuffer MaterialParams : register(b1)
{
    float4 BaseColor;
    float2 UVOffset;
    float bOpaque;
    float MaterialPadding;
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
    FPointLightInfo PointLights[NUM_POINT_LIGHT];
    FSpotLightInfo SpotLights[NUM_SPOT_LIGHT];
    uint NumPointLights;
    uint NumSpotLights;
    uint2 LightingPadding;
};

Texture2D g_txColor : register(t0);
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

float LocalLightFactor(float3 LightPos, float Radius, float FalloffExp, float3 N, float3 WorldPos, out float3 L)
{
    const float3 ToLight = LightPos - WorldPos; //픽셀의 월드 좌표에서 빛을 향하는 벡터를 구한다.
    const float Dist = length(ToLight); // 벡터 길이
    L = ToLight / max(Dist, 0.0001f); // normalize 벡터
    const float Atten = pow(saturate(1.0f - Dist / max(Radius, 0.001f)), max(FalloffExp, 0.001f)); // 거리와 radius에 비례한 강도 계산
    return saturate(dot(N, L)) * Atten; // ndotl에 강도 적용
}

// 표면에 들어오는 빛의 양만 돌려준다. 표면색(albedo)은 호출한 쪽에서 곱한다.
float3 CalcAmbient()
{
    return Ambient.Color * Ambient.Intensity;
}

float3 CalcDirectional(float3 N)
{
    return Directional.Color * Directional.Intensity * saturate(dot(N, -Directional.Direction));
}

float3 CalcPoint(FPointLightInfo P, float3 N, float3 WorldPos)
{
    float3 L;
    return P.Color * P.Intensity * LocalLightFactor(P.Position, P.Radius, P.FalloffExponent, N, WorldPos, L);

}

float3 CalcSpot(FSpotLightInfo S, float3 N, float3 WorldPos)
{
    float3 L;
    const float Base = LocalLightFactor(S.Position, S.Radius, S.FalloffExponent, N, WorldPos, L);
    const float Cone = smoothstep(S.OuterConeCos, S.InnerConeCos, dot(-L, S.Direction));
    return S.Color * S.Intensity * Base * Cone;
}

float3 CalcLighting(float3 N, float3 WorldPos)
{
    float3 Result = 0.f;
    Result += CalcDirectional(N);
    for (uint i = 0; i < NumPointLights; ++i)
    {
        Result += CalcPoint(PointLights[i], N, WorldPos);
    }
    
    for (uint s = 0; s < NumSpotLights; ++s)
    {
        Result += CalcSpot(SpotLights[s], N, WorldPos);
    }
    return Result;
}

//float3 CalcRect(FRectLightInfo R, float3 N, float3 WorldPos)
//{
//    const float3 Up = cross(R.Direction, R.Right);

//    // 1~2. 평면에 투영 후 직사각형 범위로 clamp
//    const float3 D = WorldPos - R.Position;
//    const float x = clamp(dot(D, R.Right), -R.HalfWidth, R.HalfWidth);
//    const float y = clamp(dot(D, Up), -R.HalfHeight, R.HalfHeight);
//    const float3 Closest = R.Position + R.Right * x + Up * y;

//    // 3. 가장 가까운 점을 Point처럼 계산 (FalloffExponent는 1로 고정)
//    float3 L;
//    const float Base = LocalLightFactor(Closest, R.Radius, 1.0f, N, WorldPos, L);

//    // 4. 면의 앞쪽만 비춤
//    const float FrontFace = saturate(dot(-L, R.Direction));
//    return R.Color * R.Intensity * Base * FrontFace;
//}

PS_INPUT mainVS(VS_INPUT Input)
{
    PS_INPUT Output;

    const float4 WorldPos = mul(float4(Input.position, 1.0f), World);
    Output.position = mul(WorldPos, VP);
    Output.worldPos = WorldPos.xyz;

    // 노멀은 역전치로, 탄젠트는 표면을 따라 놓인 방향이라 World로 변환한다.
    Output.normal = mul(float4(Input.normal, 0.0f), WorldInverseTranspose).xyz;
    Output.tangent = float4(mul(float4(Input.tangent.xyz, 0.0f), World).xyz, Input.tangent.w);

    Output.uv = Input.uv;
    Output.color = Input.color;
#if LIGHTING_MODEL_GOURAUD
    Output.vertexLighting = CalcLighting(normalize(Output.normal), Output.worldPos);
#endif
    return Output;
}

float4 mainPS(PS_INPUT Input) : SV_TARGET
{
    const float4 Albedo = g_txColor.Sample(g_Sample, Input.uv + UVOffset) * BaseColor;
    const float3 N = normalize(Input.normal);

    // Opaque는 알파를 1로 고정한다. 뷰포트 RT를 ImGui가 알파 블렌딩으로 그리므로 알파가 남으면 비쳐 보인다
    const float Alpha = bOpaque > 0.5f ? 1.0f : Albedo.a;

#if LIGHTING_MODEL_UNLIT
    // 조명 없이 표면색 그대로
    return float4(Albedo.rgb, Alpha);
    
#else
    float3 Lighting = CalcAmbient();
    
#if LIGHTING_MODEL_LAMBERT
    Lighting += CalcLighting(N, Input.worldPos);
    
#elif LIGHTING_MODEL_GOURAUD
    Lighting += Input.vertexLighting;
    
#endif

    return float4(Albedo.rgb * Lighting, Alpha);
#endif
}
