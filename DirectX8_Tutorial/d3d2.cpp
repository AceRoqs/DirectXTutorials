#include <d3dx8.h>

#define HELPER_RELEASE(x) { if(x) { (x)->Release(); (x) = NULL; }}

IDirect3D8 * pID3D                   = NULL;
IDirect3DDevice8 * pID3DDevice       = NULL;
IDirect3DVertexBuffer8 * pStreamData = NULL;
IDirect3DIndexBuffer8 * pIndexBuffer = NULL;
IDirect3DTexture8 * pTexture         = NULL;

struct MYVERTEX
{
    FLOAT x, y, z; // The transformed position
    FLOAT rhw;     // 1.0 (reciprocal of homogeneous w)
    DWORD color;   // The vertex color
};

MYVERTEX vertices[] =
{
    {  120.0f, 120.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 127, 127, 127) },
    {  200.0f, 200.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 127, 127, 127) },
    {  30.0f,  200.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255) },
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
        if(FAILED(hr))
            break;

        // build a vertex buffer that matches our vertex format
        int num_elems = sizeof(vertices) / sizeof(vertices[0]);
        hr = pID3DDevice->CreateVertexBuffer(sizeof(MYVERTEX) *
                                             num_elems,
                                             D3DUSAGE_WRITEONLY,
                                             D3DFVF_XYZRHW | D3DFVF_DIFFUSE,
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
            v[ii].rhw   = vertices[ii].rhw;
            v[ii].color = vertices[ii].color;
        }

        hr = pStreamData->Unlock();
        if(FAILED(hr))
            break;

        hr = pID3DDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        if(FAILED(hr))
            break;

        hr = pID3DDevice->SetStreamSource(0, pStreamData, sizeof(MYVERTEX));
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

        // draw our triangle
        int num_elems = sizeof(vertices) / sizeof(vertices[0]);
        hr = pID3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST,
                                        0,
                                        num_elems / 3);
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
