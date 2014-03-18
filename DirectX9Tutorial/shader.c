/*
Copyright (C) 2003-2011 by Toby Jones.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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

