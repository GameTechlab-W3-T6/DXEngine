// ShaderW0.ps
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 Color : COLOR;
};

Texture2D testText : register(t0);
SamplerState testSampler : register(s0);


float4 main(PS_INPUT input) : SV_Target
{
    return testText.Sample(testSampler, 0); 
    //return input.Color;
}