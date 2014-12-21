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
    { 120.0f, 120.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 127, 127, 127) },
    { 200.0f, 200.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 127, 127, 127) },
    { 30.0f,  200.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255) },
};

static const WORD indices[] = { 0, 1, 2 };

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

        // Setup our vertex buffer
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
        hr = pID3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
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

