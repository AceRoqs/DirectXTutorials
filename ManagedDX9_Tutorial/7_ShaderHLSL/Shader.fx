//=========================================================================
// Shader.fx
// Coder: Thobias Jones
// Copyright (c) 2003 Thobias Jones. All rights reserved.
// Purpose: Simple HLSL vertex and pixel shaders
//=========================================================================
struct VS_INPUT
{
    float3 position : POSITION0;
    float4 color    : COLOR0;
    float2 texture0 : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position  : POSITION;
    float4 color     : COLOR0;
    float2 texture0  : TEXCOORD0;
};

// Vertex shader
PS_INPUT VertexShader(
    VS_INPUT vertex,
    uniform float4x4 matWorldViewProj)
{
    PS_INPUT vsOut;

    vsOut.position = mul(matWorldViewProj, float4(vertex.position, 1));
    vsOut.color    = vertex.color;
    vsOut.texture0 = vertex.texture0;

    return vsOut;
}

// Pixel shader
float4 PixelShader(PS_INPUT pixel, uniform sampler2D diffuse) : COLOR
{
    return tex2D(diffuse, pixel.texture0) * pixel.color;
}
