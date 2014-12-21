// This is an example of an HLSL equivalent of the assembly shader demo.

//---(Vertex Shader)---

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Diffuse  : COLOR;
    float2 Texture  : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float4 Diffuse  : COLOR;
    float2 Texture  : TEXCOORD;
};

float4x4 mTransform : PROJECTION;

VS_OUTPUT main(const VS_INPUT input)
{
    VS_OUTPUT output;

    output.Position = mul(float4(input.Position, 1.0), mTransform);
    output.Diffuse  = input.Diffuse;
    output.Texture  = input.Texture;

    return output;
}

//---(Pixel Shader)---

struct PS_INPUT
{
    float3 Position : POSITION;
    float4 Diffuse  : COLOR;
    float2 Texture  : TEXCOORD;
};

float4 main(const PS_INPUT input)
{
    float4 output;

    output = input.Diffuse * input.Texture;

    return output;
}

