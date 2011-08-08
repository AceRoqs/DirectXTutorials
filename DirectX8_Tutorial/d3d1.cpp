#include <d3dx8.h>

#define HELPER_RELEASE(x) { if(x) { (x)->Release(); (x) = NULL; }}

IDirect3D8 * pID3D                   = NULL;
IDirect3DDevice8 * pID3DDevice       = NULL;
IDirect3DVertexBuffer8 * pStreamData = NULL;
IDirect3DIndexBuffer8 * pIndexBuffer = NULL;
IDirect3DTexture8 * pTexture         = NULL;

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
            break;

        // we do our own coloring, so disable lighting
        hr = pID3DDevice->SetRenderState(D3DRS_LIGHTING,
                                         FALSE);

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

HRESULT DrawScene()
{
    HRESULT hr;
    do
    {
        // clear back buffer
        hr = pID3DDevice->Clear(0,
                                NULL,
                                D3DCLEAR_TARGET,
                                D3DCOLOR_RGBA(0,63,0,0),
                                0,
                                0);
        if(FAILED(hr))
            break;

        // start drawing
        hr = pID3DDevice->BeginScene();
        if(FAILED(hr))
            break;

        // Put all drawing code here

        hr = pID3DDevice->EndScene();
        if(FAILED(hr))
            break;

        // flip back buffer to front
        hr = pID3DDevice->Present(NULL, NULL, NULL, NULL);
    } while(0);

    return hr;
}
