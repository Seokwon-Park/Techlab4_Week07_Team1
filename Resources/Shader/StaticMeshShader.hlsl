#pragma pack_matrix(row_major)
cbuffer Viewconstants : register(b0)
{
    matrix VP;
};

cbuffer Worldconstants : register(b2)
{
    matrix World;
};

cbuffer MaterialParams : register(b1)
{
    float4 BaseColor;
    float2 UVOffset;
    float bOpaque;
    float Shininess;
};

struct LightRenderData
{
    float4 PositionRadius;
    float4 ColorIntensity;
    float4 DirectionType;
    float4 Params;
    float4 Extent;
};

cbuffer LightConstants : register(b3)
{
    LightRenderData Lights[16];
    uint LightCount;
    float3 LightPadding;
};

cbuffer ViewParams : register(b4)
{
    float3 CameraPos;
    float  ViewPadding;
};

// LightModel
#define GOURAUD 0       // per-vertex lambert
#define LAMBERT 1       // per-pixel lambert
#define BLINNPHONG 2    // per-pixel blinnphong

// LightType
#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2
#define RECT 3
#define FIREBALL 4
#define Sky 5

#ifndef LIGHTING_MODEL
#define LIGHTING_MODEL LAMBERT      // Default
#endif

// 빛이 오는 방향과 감쇠를 구하는 함수
float3 EvaluateLight(LightRenderData light, float3 N, float3 worldPos)
{
    if (type == SKY) return color * intensity;

    int type = (int)(light.DirectionType.w + 0.5f);
    float3 dir = light.DirectionType.xyz;           // 빛의 방향
    float3 lightPos = light.PositionRadius.xyz;     // 빛의 위치
    float radius = max(light.PositionRadius.w, 0.001f);
    float3 color = light.ColorIntensity.rgb;
    float intensity = light.ColorIntensity.w;
    float3 L; // 표면에서 본 빛 쪽 방향
    float3 V; // 표면에서 본 카메라쪽 방향
    float3 H; // Normalize(L+V)
    float atten = 1.0f;

    if (type == DIRECTIONAL)
    {
        L = -dir; // 감쇠 없음
    }
    else
    {
        float3 samplePos = lightPos;

        if (type == RECT)
        {
            float3 right = light.Extent.xyz;
            float3 up = cross(dir, right);
            float3 d = worldPos - lightPos;

            // 물체가 면광원 뒤에 있음.
            if (dot(d, dir) <= 0.0f) return float3(0, 0, 0);
            
            float x = clamp(dot(d, right), -light.Extent.w, light.Extent.w);   // 반폭
            float y = clamp(dot(d, up), -light.Params.y, light.Params.y);   // 반높이
            samplePos = lightPos + right * x + up * y;
        }

        float3 toLight = samplePos - worldPos;
        float dist = length(toLight);
        L = toLight / max(dist, 0.0001f);

        // 마지막 Light에 곱해질 예정 (반경을 기준으로 멀리 있을수록 0)
        atten = pow(saturate(1.0f - dist / radius), max(light.Params.x, 0.001f));   // 거리 감쇠

        if (type == SPOT)
        {
            // 표면이 빛의 중심에서 얼마나 벗어나 있는지 (0도면 1)
            float cosAngle = dot(-L, dir);

            // y는 Inner, z는 Outer. Inner 안 = 1, Inner ~ Outer 사이 = 0~1, Outer 밖 = 0
            float cone = saturate((cosAngle - light.Params.z) / max(light.Params.y - light.Params.z, 0.001f));
            atten *= pow(cone, max(light.Params.w, 0.001f));
        }
    }

    float NL = saturate(dot(N, L));
    
    // Diffuse
    float3 result = color * NL;
    
    #if LIGHTING_MODEL == BLINNPHONG
        
        V = normalize(CameraPos - worldPos); 
        H = normalize(L + V);
        float specular = (NL > 0.0f)? pow(saturate(dot(N, H)), Shininess) : 0.0f;
        
        // specular
        result += color * specular;
    #endif

    return result * atten * intensity;
}

struct VS_INPUT
{
    float3 p : POSITION; // Input position from vertex buffer
    float3 n : NORMAL;
    float4 c : COLOR; // Input color from vertex buffer
    float2 t : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 vertexLight : TEXCOORD2;
};

Texture2D g_txColor : register(t0);
SamplerState g_Sample : register(s0);

// 전역광
// static const float3 AmbientColor = float3(0.5f, 0.5f, 0.5f);

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    output.worldPos = mul(float4(input.p, 1.0f), World).xyz;
    output.position = mul(mul(float4(input.p, 1.0f), World), VP);
    
    // 월드 공간으로 보낸다. 비균등 스케일이면 역전치가 필요함.
    output.normal = mul(float4(input.n, 0.0f), World).xyz;
    output.color = input.c;
    output.uv = input.t;
    output.vertexLight = float3(0, 0, 0);

    #if LIGHTING_MODEL == GOURAUD
        float3 N = normalize(output.normal);
        for (uint i = 0; i < LightCount; ++i)
            output.vertexLight += EvaluateLight(Lights[i], N, output.worldPos);
    #endif

    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Mesh 표면색
    float4 texColor = g_txColor.Sample(g_Sample, input.uv + UVOffset);
    // 빛을 받기 전 표면이 원래 가진 색
    float4 albedo = texColor * BaseColor;

    float3 N = normalize(input.normal);

    float3 totalLight = float3(0, 0, 0);

    // Gouraud Shading 인 경우 VS에서 계산한 Light를 사용
    #if LIGHTING_MODEL == GOURAUD
        totalLight = input.vertexLight;
    
    // Blinn Phong Shading인 경우 계산
    #else
    for (uint i = 0; i < LightCount; ++i)
    {
            totalLight += EvaluateLight(Lights[i], N, input.worldPos);
    }
    #endif

    // Opaque 알파 1로 고정
    float alpha = bOpaque > 0.5f ? 1.0f : albedo.a;

    float3 finalRGB = albedo.rgb * totalLight;
    return float4(finalRGB, alpha);
}