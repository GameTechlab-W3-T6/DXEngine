// ShaderW0.vs 
struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
     
    float4 M0 : INST_M0;
    float4 M1 : INST_M1;
    float4 M2 : INST_M2;
    
    float2 UVOffset : INST_UV_OFFSET;
    float2 UVScale : INST_UV_SCALE;
    
    float4 Color2 : INST_COLOR;
}; 
//struct VS_INST
//{
//    float4 M0 : INST_M0;
//    float4 M1 : INST_M1;
//    float4 M2 : INST_M2;
    
//    float2 UVOffset : INST_UV_OFFSET;
//    float2 UVScale : INST_UV_SCALE;
    
//    float4 Color : INST_COLOR;
//};

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
    
    float4x4 M = float4x4(
        float4(input.M0.xyz, 0),
        float4(input.M1.xyz, 0),
        float4(input.M2.xyz, 0),
        float4(input.M0.w, input.M1.w, input.M2.w, 1)
    );

    float4 wpos = float4(input.Position.xyz, 1.0f);

    // row: v' = v * MVP
    
    output.Position = mul(mul(wpos, M), MVP);
    output.UV = input.UVOffset + input.UV * input.UVScale;
    output.Color = input.Color2;

    return output;
}