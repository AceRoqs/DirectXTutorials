/*
Copyright (C) 2000 by Toby Jones.

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

#include <d3dx8.h>

#define HELPER_RELEASE(x) { if(x) { (x)->Release(); (x) = NULL; }}
#define FILENAME "dx5_logo.bmp"

IDirect3D8 * pID3D                   = NULL;
IDirect3DDevice8 * pID3DDevice       = NULL;
IDirect3DVertexBuffer8 * pStreamData = NULL;
IDirect3DIndexBuffer8 * pIndexBuffer = NULL;
IDirect3DTexture8 * pTexture         = NULL;

struct MYVERTEX
{
    FLOAT x, y, z; // The transformed position
    DWORD color;   // The vertex color
    FLOAT tu, tv;  // Texture coordinates
};

MYVERTEX vertices[] =
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

WORD indices[] = {  0,  1,  2,  3,  2,  1,
                    4,  5,  6,  5,  7,  6,
                    8,  9, 10,  8, 10, 11,
                   12, 13, 14, 12, 15, 13,
                   16, 17, 18, 16, 19, 17,
                   20, 21, 22, 21, 23, 22
                 };

HRESULT InitDirect3D(HWND hwnd)
{
    pID3D = Direct3DCreate8(D3D_SDK_VERSION);

    HRESULT hr;
    do
    {
        // we need the display mode so we can get
        // the properties of our back buffer
        D3DDISPLAYMODE d3ddm;
        hr = pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
        if(FAILED(hr))
            break;

        D3DPRESENT_PARAMETERS present;
        ZeroMemory(&present, sizeof(present));
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
            break;

        // we do our own coloring, so disable lighting
        hr = pID3DDevice->SetRenderState(D3DRS_LIGHTING,
                                         FALSE);
        if(FAILED(hr))
            break;

        // enable the z-buffer
        hr = pID3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
        if(FAILED(hr))
            break;

        // Setup our vertex buffer
        int num_elems = sizeof(vertices) / sizeof(vertices[0]);
        hr = pID3DDevice->CreateVertexBuffer(sizeof(MYVERTEX) *
                                             num_elems,
                                             D3DUSAGE_WRITEONLY,
                                             D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                             D3DPOOL_DEFAULT,
                                             &pStreamData);
        if(FAILED(hr))
            break;

        MYVERTEX *v;
        hr = pStreamData->Lock(0, 0, (BYTE**)&v, 0);
        if(FAILED(hr))
            break;

        // copy our vertices
        for(int ii = 0; ii < num_elems; ii++)
        {
            v[ii].x     = vertices[ii].x;
            v[ii].y     = vertices[ii].y;
            v[ii].z     = vertices[ii].z;
            v[ii].color = vertices[ii].color;
            v[ii].tu    = vertices[ii].tu;
            v[ii].tv    = vertices[ii].tv;
        }

        hr = pStreamData->Unlock();
        if(FAILED(hr))
            break;

        // Set a common vertex shader
        hr = pID3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        if(FAILED(hr))
            break;

        hr = pID3DDevice->SetStreamSource(0, pStreamData, sizeof(MYVERTEX));
        if(FAILED(hr))
            break;

        // Setup our index buffer
        num_elems = sizeof(indices) / sizeof(indices[0]);
        hr = pID3DDevice->CreateIndexBuffer(sizeof(WORD) * num_elems, // in bytes, not indices
                                            D3DUSAGE_WRITEONLY,
                                            D3DFMT_INDEX16,
                                            D3DPOOL_DEFAULT,
                                            &pIndexBuffer);
        WORD *pIndex;
        hr = pIndexBuffer->Lock(0, 0, (BYTE **)&pIndex, 0);
        if(FAILED(hr))
            break;
        for(ii = 0; ii < num_elems; ii++)
        {
            pIndex[ii] = indices[ii];
        }

        hr = pIndexBuffer->Unlock();
        if(FAILED(hr))
            break;

        hr = pID3DDevice->SetIndices(pIndexBuffer, 0);
        if(FAILED(hr))
            break;

        hr = D3DXCreateTextureFromFile(pID3DDevice, FILENAME, &pTexture);
        if(FAILED(hr))
            break;

        hr = pID3DDevice->SetTexture(0, pTexture);
    } while(0);

    return hr;
}

void ShutdownDirect3D()
{
    HELPER_RELEASE(pTexture);
    HELPER_RELEASE(pIndexBuffer);
    HELPER_RELEASE(pStreamData);
    HELPER_RELEASE(pID3DDevice);
    HELPER_RELEASE(pID3D);
}

void BuildMatrices()
{
    D3DXMATRIX matrix;
    D3DXMatrixRotationY(&matrix, timeGetTime() / 1000.0f);
    pID3DDevice->SetTransform(D3DTS_WORLD, &matrix);

    D3DXMatrixLookAtLH(&matrix, &D3DXVECTOR3(0.0f, 3.0f, -5.0f), // camera position
                                &D3DXVECTOR3(0.0f, 0.0f, 0.0f),  // point to look at
                                &D3DXVECTOR3(0.0f, 1.0f, 0.0f)); // up vector
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
                                NULL,
                                D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                                D3DCOLOR_RGBA(0,63,0,0),
                                1.0,
                                0);
        if(FAILED(hr))
            break;

        BuildMatrices();

        // start drawing
        hr = pID3DDevice->BeginScene();
        if(FAILED(hr))
            break;

        // draw our triangle
        hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                               0,
                                               sizeof(indices) / sizeof(indices[0]),
                                               0,
                                               sizeof(indices) / sizeof(indices[0]) / 3);
        if(FAILED(hr))
            break;

        hr = pID3DDevice->EndScene();
        if(FAILED(hr))
            break;

        // flip back buffer to front
        hr = pID3DDevice->Present(NULL, NULL, NULL, NULL);
    } while(0);

    return hr;
}

