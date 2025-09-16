// Gizmo Vertex Shader
struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 MVP;
    float4 MeshColor;
    bool IsSelected;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 baseColor = input.Color;

    // Apply selection highlighting
    if (IsSelected)
    {
        baseColor.rgb = baseColor.rgb + 0.5f;
    }

    // Apply mesh color from constant buffer
    output.Color = baseColor * MeshColor;

    float4 wpos = float4(input.Position.xyz, 1.0f);
    output.Position = mul(wpos, MVP);

    return output;
}