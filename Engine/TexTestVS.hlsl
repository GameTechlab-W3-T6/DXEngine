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
    float IsSelected;
    float padding[3];
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    //float4 baseColor = input.Color;
    //
    //if (0.5f < IsSelected)
    //{
    //    baseColor.rgb = baseColor.rgb + 0.25f;
    //}
    //
    //output.Color = baseColor * MeshColor;

    float4 wpos = float4(input.Position.xyz, 1.0f);

    // row: v' = v * MVP
    output.Position = mul(wpos, MVP);
    output.UV = input.UV;

    return output;
}