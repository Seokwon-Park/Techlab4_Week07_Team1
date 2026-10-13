Texture2D InputTexture : register(t0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float3 ToneMapACES(float3 X)
{
    float A = 2.51f;
    float B = 0.03f;
    float C = 2.43f;
    float D = 0.59f;
    float E = 0.14f;
    return saturate((X * (A * X + B)) / (X * (C * X + D) + E));
}

PSInput mainVS(uint VertexID : SV_VertexID)
{
    PSInput Output;
    float2 UV = float2((VertexID << 1) & 2, VertexID & 2); // (0,0) (2,0) (0,2)
    Output.Position = float4(UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    Output.UV = UV;
    return Output;
}

float4 mainPS(PSInput input) : SV_Target
{
    float4 Loaded = InputTexture.Load(int3(input.Position.xy, 0));
    float4 Color = max(Loaded, 0);
    const float Exposure = 0.6f;
    return float4(ToneMapACES(Color.rgb * Exposure), 1);
}