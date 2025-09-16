// ShaderW0.vs 
struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
     
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};
 

cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 MVP; // Model   (row-vector)
    float4 MeshColor;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 baseColor = input.Color;

    // Apply mesh color from constant buffer
    output.Color = baseColor * MeshColor;

    float4 wpos = float4(input.Position.xyz, 1.0f);

    // row: v' = v * MVP
    output.Position = mul(wpos, MVP);
    output.UV = input.UV;

    return output;
}