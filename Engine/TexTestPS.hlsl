// ShaderW0.ps
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
};

cbuffer TextConstantBuffer : register(b0)
{
    float2 cellIndex;
    float2 cellSize;
    float2 texResolution;
};

Texture2D testText : register(t0);
SamplerState testSampler : register(s0); 

float4 main(PS_INPUT input) : SV_Target
{  
    float4 textColor = testText.Sample(testSampler, input.UV);
    if (textColor.a < 0.1f)
        discard; 
    
    return textColor;
}