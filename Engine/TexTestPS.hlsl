// ShaderW0.ps
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

Texture2D testText : register(t0);
SamplerState testSampler : register(s0);

cbuffer TextConstantBuffer : register(b0)
{
    float2 cellSize;
    float2 texReolution;  
    float2 cellIndex; 
    float2 padding;
};

float4 main(PS_INPUT input) : SV_Target
{ 
    return float4(1, 1, 1, 1);
    float2 cellScale = cellSize / texReolution;
    
    float2 cellOffset = cellIndex * cellSize;
    float2 uv = input.UV * cellScale + cellOffset;
    
  
    return testText.Sample(testSampler, uv);
    //return input.Color;
}