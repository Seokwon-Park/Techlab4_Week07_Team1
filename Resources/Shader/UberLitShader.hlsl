#pragma pack_matrix(row_major)

#ifndef EnableAmbientLight
#define EnableAmbientLight 1
#endif

#ifndef EnableDirectionalLight
#define EnableDirectionalLight 1
#endif

#ifndef EnablePointLight
#define EnablePointLight 1
#endif

#ifndef EnableSpotLight
#define EnableSpotLight 1
#endif

#ifndef EnableRectLight
#define EnableRectLight 1
#endif

struct FLightRenderParameters {
    float4 LightColor;
    float3 WorldPosition;
    float InvRadius;

    float3 WorldDirection;
    float FalloffExponent;

    float SourceRadius;
    float SoftSourceRadius;
    float SourceLength;
    float DiffuseScale;

    float2 SpotAngles;
    float SpecularScale;
    float Padding;
};

cbuffer ViewConstants : register(b0) {
    matrix VP;
};

cbuffer MaterialParams : register(b1) {
    float4 BaseColor;
    float2 UVOffset;
    float Opaque;
    float MaterialPadding;
};

cbuffer WorldConstants : register(b2) {
    matrix World;
};

cbuffer LightConstants : register(b3) {
    uint AmbientLightCount;
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
    uint RectLightCount;
    uint3 LightPadding;
};

cbuffer PhongConstants : register(b4) {
    float3 CameraWorldPosition;
    float Shininess;
    float3 SpecularColor;
    float PhongPadding;
};

struct FVertexInput {
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

struct FPixelInput {
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 WorldPosition : TEXCOORD1;
};

Texture2D ColorTexture : register(t0);
StructuredBuffer<FLightRenderParameters> Lights : register(t1);
SamplerState ColorSampler : register(s0);

float3 NormalizeDirection(float3 Direction) {
    return Direction * rsqrt(max(dot(Direction, Direction), 1.0e-8f));
}

float3 TransformWorldNormal(float3 Normal) {
    const float3 Row0 = { World[0].xyz };
    const float3 Row1 = { World[1].xyz };
    const float3 Row2 = { World[2].xyz };
    const float3 Cofactor0 = { cross(Row1, Row2) };
    const float3 Cofactor1 = { cross(Row2, Row0) };
    const float3 Cofactor2 = { cross(Row0, Row1) };
    const float Determinant = { dot(Row0, Cofactor0) };
    const float Orientation = { Determinant < 0.0f ? -1.0f : 1.0f };
    return NormalizeDirection((Normal.x * Cofactor0 + Normal.y * Cofactor1 + Normal.z * Cofactor2) * Orientation);
}

#if EnablePointLight || EnableSpotLight || EnableRectLight
float ComputeRangeAttenuation(FLightRenderParameters Light, float Distance) {
    if (Light.InvRadius <= 0.0f) {
        return 0.0f;
    }

    const float RangeFade = { saturate(1.0f - Distance * Light.InvRadius) };
    return pow(RangeFade, max(Light.FalloffExponent, 1.0f));
}
#endif

#if EnableRectLight
float3 ComputeRectPosition(FLightRenderParameters Light, float3 WorldPosition, float3 Forward) {
    const float3 AxisZ = { 0.0f, 0.0f, 1.0f };
    const float3 AxisY = { 0.0f, 1.0f, 0.0f };
    const float3 ReferenceUp = { abs(Forward.z) < 0.999f ? AxisZ : AxisY };
    const float3 Right = { NormalizeDirection(cross(ReferenceUp, Forward)) };
    const float3 Up = { cross(Forward, Right) };
    const float3 Offset = { WorldPosition - Light.WorldPosition };
    const float HalfWidth = { max(Light.SourceRadius, 0.0f) };
    const float HalfHeight = { max(Light.SourceLength, 0.0f) };
    const float Horizontal = { clamp(dot(Offset, Right), -HalfWidth, HalfWidth) };
    const float Vertical = { clamp(dot(Offset, Up), -HalfHeight, HalfHeight) };
    return Light.WorldPosition + Right * Horizontal + Up * Vertical;
}
#endif

#if EnableDirectionalLight || EnablePointLight || EnableSpotLight || EnableRectLight
float3 EvaluatePhong(FLightRenderParameters Light, float3 Albedo, float3 Normal, float3 ViewDirection, float3 LightDirection, float Attenuation) {
    const float NDotL = { saturate(dot(Normal, LightDirection)) };
    if (NDotL <= 0.0f || Attenuation <= 0.0f) {
        return 0.0f;
    }

    const float3 ReflectionDirection = { reflect(-LightDirection, Normal) };
    const float SpecularFactor = { pow(saturate(dot(ReflectionDirection, ViewDirection)), max(Shininess, 1.0f)) };
    const float3 Diffuse = { Albedo * NDotL * max(Light.DiffuseScale, 0.0f) };
    const float3 Specular = { max(SpecularColor, 0.0f) * SpecularFactor * max(Light.SpecularScale, 0.0f) };
    return max(Light.LightColor.rgb, 0.0f) * (Diffuse + Specular) * Attenuation;
}
#endif

#if EnableAmbientLight
float3 EvaluateAmbientLight(FLightRenderParameters Light, float3 Albedo) {
    return Albedo * max(Light.LightColor.rgb, 0.0f) * max(Light.DiffuseScale, 0.0f);
}
#endif

#if EnableDirectionalLight
float3 EvaluateDirectionalLight(FLightRenderParameters Light, float3 Albedo, float3 Normal, float3 ViewDirection) {
    const float3 Forward = { NormalizeDirection(Light.WorldDirection) };
    return EvaluatePhong(Light, Albedo, Normal, ViewDirection, -Forward, 1.0f);
}
#endif

#if EnablePointLight
float3 EvaluatePointLight(FLightRenderParameters Light, float3 Albedo, float3 WorldPosition, float3 Normal, float3 ViewDirection) {
    const float3 ToLight = { Light.WorldPosition - WorldPosition };
    const float Distance = { length(ToLight) };
    const float3 LightDirection = { NormalizeDirection(ToLight) };
    const float Attenuation = { ComputeRangeAttenuation(Light, Distance) };
    return EvaluatePhong(Light, Albedo, Normal, ViewDirection, LightDirection, Attenuation);
}
#endif

#if EnableSpotLight
float3 EvaluateSpotLight(FLightRenderParameters Light, float3 Albedo, float3 WorldPosition, float3 Normal, float3 ViewDirection) {
    const float3 Forward = { NormalizeDirection(Light.WorldDirection) };
    const float3 ToLight = { Light.WorldPosition - WorldPosition };
    const float Distance = { length(ToLight) };
    const float3 LightDirection = { NormalizeDirection(ToLight) };
    const float ConeCosine = { dot(-LightDirection, Forward) };
    const float ConeFade = { saturate((ConeCosine - Light.SpotAngles.x) * max(Light.SpotAngles.y, 0.0f)) };
    const float Attenuation = { ComputeRangeAttenuation(Light, Distance) * ConeFade * ConeFade };
    return EvaluatePhong(Light, Albedo, Normal, ViewDirection, LightDirection, Attenuation);
}
#endif

#if EnableRectLight
float3 EvaluateRectLight(FLightRenderParameters Light, float3 Albedo, float3 WorldPosition, float3 Normal, float3 ViewDirection) {
    const float3 Forward = { NormalizeDirection(Light.WorldDirection) };
    const float3 Position = { ComputeRectPosition(Light, WorldPosition, Forward) };
    const float3 ToLight = { Position - WorldPosition };
    const float Distance = { length(ToLight) };
    const float3 LightDirection = { NormalizeDirection(ToLight) };
    const float Attenuation = { ComputeRangeAttenuation(Light, Distance) * saturate(dot(Forward, -LightDirection)) };
    return EvaluatePhong(Light, Albedo, Normal, ViewDirection, LightDirection, Attenuation);
}
#endif

FPixelInput mainVS(FVertexInput Input) {
    const float4 LocalPosition = { Input.Position, 1.0f };
    const float4 WorldPosition = { mul(LocalPosition, World) };
    FPixelInput Output = { mul(WorldPosition, VP), TransformWorldNormal(Input.Normal), Input.Color, Input.UV, WorldPosition.xyz };
    return Output;
}

float4 mainPS(FPixelInput Input) : SV_TARGET {
    const float4 Albedo = { ColorTexture.Sample(ColorSampler, Input.UV + UVOffset) * BaseColor * Input.Color };
#if EnableDirectionalLight || EnablePointLight || EnableSpotLight || EnableRectLight
    const float3 Normal = { NormalizeDirection(Input.Normal) };
    const float3 ViewDirection = { NormalizeDirection(CameraWorldPosition - Input.WorldPosition) };
#endif
    uint BufferLightCount = { 0u };
    uint LightStride = { 0u };
    Lights.GetDimensions(BufferLightCount, LightStride);

    const uint AmbientEnd = { min(AmbientLightCount, BufferLightCount) };
    const uint DirectionalEnd = { AmbientEnd + min(DirectionalLightCount, BufferLightCount - AmbientEnd) };
    const uint PointEnd = { DirectionalEnd + min(PointLightCount, BufferLightCount - DirectionalEnd) };
    const uint SpotEnd = { PointEnd + min(SpotLightCount, BufferLightCount - PointEnd) };
    const uint RectEnd = { SpotEnd + min(RectLightCount, BufferLightCount - SpotEnd) };
    float3 LitColor = { 0.0f, 0.0f, 0.0f };

#if EnableAmbientLight
    for (uint AmbientIndex = { 0u }; AmbientIndex < AmbientEnd; ++AmbientIndex) {
        LitColor += EvaluateAmbientLight(Lights[AmbientIndex], Albedo.rgb);
    }
#endif

#if EnableDirectionalLight
    for (uint DirectionalIndex = { AmbientEnd }; DirectionalIndex < DirectionalEnd; ++DirectionalIndex) {
        LitColor += EvaluateDirectionalLight(Lights[DirectionalIndex], Albedo.rgb, Normal, ViewDirection);
    }
#endif

#if EnablePointLight
    for (uint PointIndex = { DirectionalEnd }; PointIndex < PointEnd; ++PointIndex) {
        LitColor += EvaluatePointLight(Lights[PointIndex], Albedo.rgb, Input.WorldPosition, Normal, ViewDirection);
    }
#endif

#if EnableSpotLight
    for (uint SpotIndex = { PointEnd }; SpotIndex < SpotEnd; ++SpotIndex) {
        LitColor += EvaluateSpotLight(Lights[SpotIndex], Albedo.rgb, Input.WorldPosition, Normal, ViewDirection);
    }
#endif

#if EnableRectLight
    for (uint RectIndex = { SpotEnd }; RectIndex < RectEnd; ++RectIndex) {
        LitColor += EvaluateRectLight(Lights[RectIndex], Albedo.rgb, Input.WorldPosition, Normal, ViewDirection);
    }
#endif

    const float Alpha = { Opaque > 0.5f ? 1.0f : Albedo.a };
    const float4 Result = { LitColor, Alpha };
    return Result;
}
