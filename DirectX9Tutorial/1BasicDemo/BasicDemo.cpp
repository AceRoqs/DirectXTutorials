#include <d3dx9.h>

#define ONCE ((void)0, false)

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

    } while(ONCE);

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

        // Put all drawing code here

        hr = pID3DDevice->EndScene();
        if(FAILED(hr))
        {
            break;
        }

        // flip back buffer to front
        hr = pID3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
    } while(ONCE);

    return hr;
}

