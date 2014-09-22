/*
Copyright (C) 2003-2014 by Toby Jones.

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

#include <d3dx9.h>

#pragma warning(disable:4127)  // Conditional expression is constant.

template<typename Ty>
void HELPER_RELEASE(_In_ Ty ** pointer)
{
    if(nullptr != *pointer)
    {
        (*pointer)->Release();
        *pointer = nullptr;
    }
}

const PCTSTR FILENAME = TEXT("photo.png");

// In general, these will not be global, and should be wrapped in a smart pointer.
static IDirect3D9 * pID3D                   = nullptr;
static IDirect3DDevice9 * pID3DDevice       = nullptr;
static IDirect3DVertexBuffer9 * pStreamData = nullptr;
static IDirect3DIndexBuffer9 * pIndexBuffer = nullptr;
static IDirect3DTexture9 * pTexture         = nullptr;

struct MYVERTEX
{
    FLOAT x, y, z; // The transformed position
    DWORD color;   // The vertex color
    FLOAT tu, tv;  // Texture coordinates
};

static const MYVERTEX vertices[] =
{
    // front face
    {  0.5f, -0.5f, -0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 1.0, 1.0 }, // 0
    { -0.5f, -0.5f, -0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 0.0, 1.0 }, // 1
    {  0.5f,  0.5f, -0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0, 0.0 }, // 2
    { -0.5f,  0.5f, -0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0, 0.0 }, // 3

    // top face
    {  0.5f,  0.5f, -0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0, 1.0 }, // 4
    { -0.5f,  0.5f, -0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0, 1.0 }, // 5
    {  0.5f,  0.5f,  0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0, 0.0 }, // 6
    { -0.5f,  0.5f,  0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0, 0.0 }, // 7

    // back face
    { -0.5f, -0.5f,  0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 1.0, 1.0 }, // 8
    {  0.5f, -0.5f,  0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 0.0, 1.0 }, // 9
    {  0.5f,  0.5f,  0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0, 0.0 }, // 10
    { -0.5f,  0.5f,  0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0, 0.0 }, // 11

    // left face
    {  0.5f, -0.5f,  0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 1.0, 1.0 }, // 12
    {  0.5f,  0.5f, -0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0, 0.0 }, // 13
    {  0.5f,  0.5f,  0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0, 0.0 }, // 14
    {  0.5f, -0.5f, -0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 0.0, 1.0 }, // 15

    // bottom face
    {  0.5f, -0.5f,  0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 1.0, 1.0 }, // 16
    { -0.5f, -0.5f, -0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 0.0, 0.0 }, // 17
    {  0.5f, -0.5f, -0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 1.0, 0.0 }, // 18
    { -0.5f, -0.5f,  0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 0.0, 1.0 }, // 19

    // right face
    { -0.5f,  0.5f,  0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0, 0.0 }, // 20
    { -0.5f,  0.5f, -0.5f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0, 0.0 }, // 21
    { -0.5f, -0.5f,  0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 0.0, 1.0 }, // 22
    { -0.5f, -0.5f, -0.5f, D3DCOLOR_ARGB(255, 127, 127, 127), 1.0, 1.0 }, // 23
};

static const WORD indices[] = {  0,  1,  2,  3,  2,  1,
                                 4,  5,  6,  5,  7,  6,
                                 8,  9, 10,  8, 10, 11,
                                12, 13, 14, 12, 15, 13,
                                16, 17, 18, 16, 19, 17,
                                20, 21, 22, 21, 23, 22
                              };

HRESULT InitDirect3D(_In_ HWND hwnd)
{
    pID3D = ::Direct3DCreate9(D3D_SDK_VERSION);

    HRESULT hr;
    do
    {
        // we need the display mode so we can get
        // the properties of our back buffer
        D3DDISPLAYMODE d3ddm;
        hr = pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
        if(FAILED(hr))
        {
            break;
        }

        D3DPRESENT_PARAMETERS present;
        ::ZeroMemory(&present, sizeof(present));
        present.SwapEffect             = D3DSWAPEFFECT_COPY;
        present.Windowed               = TRUE;
        present.BackBufferFormat       = d3ddm.Format;
        present.EnableAutoDepthStencil = TRUE;
        present.AutoDepthStencilFormat = D3DFMT_D16;

        hr = pID3D->CreateDevice(D3DADAPTER_DEFAULT,
                                 D3DDEVTYPE_HAL,
                                 hwnd,
                                 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                 &present,
                                 &pID3DDevice);

        if(FAILED(hr))
        {
            break;
        }

        // we do our own coloring, so disable lighting
        hr = pID3DDevice->SetRenderState(D3DRS_LIGHTING,
                                         FALSE);
        if(FAILED(hr))
        {
            break;
        }

        // enable the z-buffer
        hr = pID3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
        if(FAILED(hr))
        {
            break;
        }

        // Setup our vertex buffer
        unsigned int num_elems = ARRAYSIZE(vertices);
        hr = pID3DDevice->CreateVertexBuffer(sizeof(MYVERTEX) * num_elems,
                                             D3DUSAGE_WRITEONLY,
                                             D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                             D3DPOOL_DEFAULT,
                                             &pStreamData,
                                             nullptr);
        if(FAILED(hr))
        {
            break;
        }

        MYVERTEX *pVertex;
        hr = pStreamData->Lock(0, 0, reinterpret_cast<void**>(&pVertex), 0);
        if(FAILED(hr))
        {
            break;
        }

        // copy our vertices
        unsigned int ii;
        for(ii = 0; ii < num_elems; ++ii)
        {
            pVertex[ii] = vertices[ii];
        }

        hr = pStreamData->Unlock();
        if(FAILED(hr))
        {
            break;
        }

        // Set a common vertex shader
        hr = pID3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        if(FAILED(hr))
        {
            break;
        }

        hr = pID3DDevice->SetStreamSource(0, pStreamData, 0, sizeof(MYVERTEX));
        if(FAILED(hr))
        {
            break;
        }

        // Setup our index buffer
        num_elems = ARRAYSIZE(indices);
        hr = pID3DDevice->CreateIndexBuffer(sizeof(WORD) * num_elems, // in bytes, not indices
                                            D3DUSAGE_WRITEONLY,
                                            D3DFMT_INDEX16,
                                            D3DPOOL_DEFAULT,
                                            &pIndexBuffer,
                                            nullptr);
        WORD *pIndex;
        hr = pIndexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pIndex), 0);
        if(FAILED(hr))
        {
            break;
        }
        for(ii = 0; ii < num_elems; ++ii)
        {
            pIndex[ii] = indices[ii];
        }

        hr = pIndexBuffer->Unlock();
        if(FAILED(hr))
        {
            break;
        }

        hr = pID3DDevice->SetIndices(pIndexBuffer);
        if(FAILED(hr))
        {
            break;
        }

        hr = D3DXCreateTextureFromFile(pID3DDevice, FILENAME, &pTexture);
        if(FAILED(hr))
        {
            break;
        }

        hr = pID3DDevice->SetTexture(0, pTexture);
    } while(false);

    return hr;
}

void ShutdownDirect3D()
{
    HELPER_RELEASE(&pTexture);
    HELPER_RELEASE(&pIndexBuffer);
    HELPER_RELEASE(&pStreamData);
    HELPER_RELEASE(&pID3DDevice);
    HELPER_RELEASE(&pID3D);
}

void BuildMatrices()
{
    D3DXMATRIX matrix;
    D3DXMatrixRotationY(&matrix, timeGetTime() / 1000.0f);
    pID3DDevice->SetTransform(D3DTS_WORLD, &matrix);

    D3DXVECTOR3 camerapos(0.0f, 3.0f, -5.0f);
    D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    D3DXMatrixLookAtLH(&matrix, &camerapos, &lookat, &up);
    pID3DDevice->SetTransform(D3DTS_VIEW, &matrix);

    // Set our frustum to 45 degrees
    D3DXMatrixPerspectiveFovLH(&matrix, D3DX_PI / 4, 4.0f / 3.0f, 1.0f, 100.0f);
    pID3DDevice->SetTransform(D3DTS_PROJECTION, &matrix);
}

HRESULT DrawScene()
{
    HRESULT hr;
    do
    {
        // clear back buffer
        hr = pID3DDevice->Clear(0,
                                nullptr,
                                D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                                D3DCOLOR_RGBA(0, 0, 127, 0),
                                1.0,
                                0);
        if(FAILED(hr))
        {
            break;
        }

        BuildMatrices();

        // start drawing
        hr = pID3DDevice->BeginScene();
        if(FAILED(hr))
        {
            break;
        }

        // draw our triangle
        hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                               0,
                                               0,
                                               ARRAYSIZE(vertices),
                                               0,
                                               ARRAYSIZE(indices) / 3);
        if(FAILED(hr))
        {
            break;
        }

        hr = pID3DDevice->EndScene();
        if(FAILED(hr))
        {
            break;
        }

        // flip back buffer to front
        hr = pID3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
    } while(false);

    return hr;
}

