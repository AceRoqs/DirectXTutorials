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

#include <d3dx9.h>

#pragma warning(disable:4127)  // Conditional expression is constant.

template<typename Ty>
void HELPER_RELEASE(_In_opt_ Ty ** pointer)
{
    if(nullptr != *pointer)
    {
        (*pointer)->Release();
        *pointer = nullptr;
    }
}

// In general, these will not be global, and should be wrapped in a smart pointer.
static IDirect3D9 * pID3D                   = nullptr;
static IDirect3DDevice9 * pID3DDevice       = nullptr;
static IDirect3DVertexBuffer9 * pStreamData = nullptr;
static IDirect3DIndexBuffer9 * pIndexBuffer = nullptr;
static IDirect3DTexture9 * pTexture         = nullptr;

struct MYVERTEX
{
    FLOAT x, y, z; // The transformed position
    FLOAT rhw;     // 1.0 (reciprocal of homogeneous w)
    DWORD color;   // The vertex color
};

static const MYVERTEX vertices[] =
{
    {  120.0f, 120.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 127, 127, 127) },
    {  200.0f, 200.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 127, 127, 127) },
    {  30.0f,  200.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255) },
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
        present.SwapEffect       = D3DSWAPEFFECT_COPY;
        present.Windowed         = TRUE;
        present.BackBufferFormat = d3ddm.Format;

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

        // build a vertex buffer that matches our vertex format
        unsigned int num_elems = ARRAYSIZE(vertices);
        hr = pID3DDevice->CreateVertexBuffer(sizeof(MYVERTEX) * num_elems,
                                             D3DUSAGE_WRITEONLY,
                                             D3DFVF_XYZRHW | D3DFVF_DIFFUSE,
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
        for(unsigned int ii = 0; ii < num_elems; ++ii)
        {
            pVertex[ii] = vertices[ii];
        }

        hr = pStreamData->Unlock();
        if(FAILED(hr))
        {
            break;
        }

        hr = pID3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        if(FAILED(hr))
        {
            break;
        }

        hr = pID3DDevice->SetStreamSource(0, pStreamData, 0, sizeof(MYVERTEX));
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

HRESULT DrawScene()
{
    HRESULT hr;
    do
    {
        // clear back buffer
        hr = pID3DDevice->Clear(0,
                                nullptr,
                                D3DCLEAR_TARGET,
                                D3DCOLOR_RGBA(0, 0, 127, 0),
                                0,
                                0);
        if(FAILED(hr))
        {
            break;
        }

        // start drawing
        hr = pID3DDevice->BeginScene();
        if(FAILED(hr))
        {
            break;
        }

        // draw our triangle
        int num_elems = ARRAYSIZE(vertices);
        hr = pID3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST,
                                        0,
                                        num_elems / 3);
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

