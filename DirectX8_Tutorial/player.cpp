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

#define WIN32_LEAN_AND_MEAN
#include <afxwin.h>
#include <windows.h>
#include <d3dx8.h>
#include <streams.h>

#define WM_GRAPHEVENT   WM_USER
#define HELPER_RELEASE(x) { if(x) { (x)->Release(); (x) = NULL; }}
#define FILENAME L"highway.avi"

typedef struct tagVIDEOPLAYER
{
    HWND hwndParent;
    IGraphBuilder  * pGraphBuilder;
    IMediaControl  * pMediaControl;
    IMediaEventEx  * pMediaEvent;
    IMediaPosition * pMediaPosition;
    IVideoWindow   * pVideoWindow;
} VIDEOPLAYER;

typedef VIDEOPLAYER * PVIDEOPLAYER;

VIDEOPLAYER VideoPlayer;
IDirect3D8       * pID3D        = NULL;
IDirect3DDevice8 * pID3DDevice  = NULL;

// function prototypes for D3D
HRESULT InitDirect3D(HWND);
void    ShutdownDirect3D();
HRESULT DrawScene();
HRESULT CreateVideoPlayer(HWND hwnd, PVIDEOPLAYER pVideoPlayer);
void    DestroyVideoPlayer(PVIDEOPLAYER pVideoPlayer);
void    PlayAVI(PVIDEOPLAYER pVideoPlayer, LPCWSTR szFilename);
LRESULT OnGraphEvent(PVIDEOPLAYER pVideoPlayer);

// a simple WndProc that validates a window and kills it when the time comes
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch(iMsg)
    {
        case WM_GRAPHEVENT:
            return OnGraphEvent(&VideoPlayer);

        case WM_ERASEBKGND:
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
} // WndProc

// the main purpose of this function is to build a window and call DirectX
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int iCmdShow)
{
    const TCHAR szClassName[] = _T("DirectX 8 Tutorial");

    // build us a window
    WNDCLASSEX wndclass =
    {
        sizeof(wndclass),                   // cbSize
        CS_HREDRAW | CS_VREDRAW,            // style
        WndProc,                            // lpfnWndProc
        0,                                  // cbClsExtra
        0,                                  // cbWndExtra
        hInstance,                          // hInstance
        LoadIcon(NULL, IDI_APPLICATION),    // hIcon
        LoadCursor(NULL, IDC_ARROW),        // hCursor
        NULL,                               // hBackground
        NULL,                               // lpszMenuName
        szClassName,                        // lpszClassName
        LoadIcon(NULL, IDI_APPLICATION)     // hIconSm
    };

    RegisterClassEx(&wndclass);

    HWND hwnd = CreateWindow(szClassName,           // lpClassName
                             szClassName,           // lpWindowName
                             WS_OVERLAPPEDWINDOW,   // dwStyle
                             CW_USEDEFAULT,         // x
                             CW_USEDEFAULT,         // y
                             CW_USEDEFAULT,         // nWidth
                             CW_USEDEFAULT,         // nHeight
                             NULL,                  // hwndParent
                             NULL,                  // hMenu
                             hInstance,             // hInstance
                             NULL);                 // lpParam

    // in a real app, you might want to give an error message here
    if(FAILED(InitDirect3D(hwnd)))
        return 0;

    if(FAILED(CreateVideoPlayer(hwnd, &VideoPlayer)))
        return 0;

    PlayAVI(&VideoPlayer, FILENAME);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while(1)
    {
        // clear out all the messages before we draw a new frame
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(WM_QUIT == msg.message)
            {
                DestroyVideoPlayer(&VideoPlayer);
                ShutdownDirect3D();

                return msg.wParam;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
} // WinMain

HRESULT CreateVideoPlayer(
    HWND hwnd,
    PVIDEOPLAYER pVideoPlayer)
{
    ZeroMemory(pVideoPlayer, sizeof(VIDEOPLAYER));
    pVideoPlayer->hwndParent = hwnd;

    CoInitialize(NULL);
    HRESULT hr;

    do
    {
        hr = CoCreateInstance(CLSID_FilterGraph,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IGraphBuilder,
                              (void**)&pVideoPlayer->pGraphBuilder);

        if(FAILED(hr))
            break;

        // Create our needed DirectShow interfaces. They are uninteresting.
        hr = pVideoPlayer->pGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&pVideoPlayer->pMediaControl);
        if(FAILED(hr))
            break;

        hr = pVideoPlayer->pGraphBuilder->QueryInterface(IID_IMediaEvent, (void**)&pVideoPlayer->pMediaEvent);
        if(FAILED(hr))
            break;

        hr = pVideoPlayer->pGraphBuilder->QueryInterface(IID_IMediaPosition, (void**)&pVideoPlayer->pMediaPosition);
        if(FAILED(hr))
            break;

        // Turn on notifications
        hr = pVideoPlayer->pMediaEvent->SetNotifyWindow((OAHWND)hwnd, WM_GRAPHEVENT, 0);
        if(FAILED(hr))
            break;

        hr = pVideoPlayer->pMediaEvent->SetNotifyFlags(0);
    } while(0);

    return hr;
} // CreateVideoPlayer

void DestroyVideoPlayer(
    PVIDEOPLAYER pVideoPlayer)
{
    HELPER_RELEASE(pVideoPlayer->pMediaPosition);
    HELPER_RELEASE(pVideoPlayer->pMediaEvent);
    HELPER_RELEASE(pVideoPlayer->pMediaControl);
    HELPER_RELEASE(pVideoPlayer->pGraphBuilder);
}

// Render a video file full screen
void PlayAVI(
    PVIDEOPLAYER pVideoPlayer,
    LPCWSTR szFilename)
{
    pVideoPlayer->pGraphBuilder->RenderFile(szFilename, NULL);

    pVideoPlayer->pGraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&pVideoPlayer->pVideoWindow);
    pVideoPlayer->pMediaControl->Run();
    if(NULL != pVideoPlayer->pVideoWindow)
    {
        pVideoPlayer->pVideoWindow->put_FullScreenMode(OATRUE);
    }
}

// handle WM_GRAPHEVENT:
LRESULT OnGraphEvent(
    PVIDEOPLAYER pVideoPlayer)
{
    long EventCode, Param1, Param2;
    while(pVideoPlayer->pMediaEvent->GetEvent(&EventCode, &Param1, &Param2, 0) != E_ABORT)
    {
        switch(EventCode)
        {
            // Gets here when media is completely done playing
            case EC_COMPLETE:
            {
                // Stop video playing and end program
                pVideoPlayer->pMediaControl->Stop();
                pVideoPlayer->pVideoWindow->put_FullScreenMode(OAFALSE);
                HRESULT hr = pVideoPlayer->pVideoWindow->put_Visible(OAFALSE);
                HELPER_RELEASE(pVideoPlayer->pVideoWindow);
                DestroyWindow(pVideoPlayer->hwndParent);
                break;
            }

            // react to specific events here

            default:
                break;
        }
        pVideoPlayer->pMediaEvent->FreeEventParams(EventCode, Param1, Param2);
    }

    return 0;
}

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
        present.SwapEffect              = D3DSWAPEFFECT_COPY;
        present.Windowed                = TRUE;
        present.BackBufferFormat        = d3ddm.Format;
        present.EnableAutoDepthStencil  = TRUE;
        present.AutoDepthStencilFormat  = D3DFMT_D16;

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

        // clear back buffer
        hr = pID3DDevice->Clear(0,
                                NULL,
                                D3DCLEAR_TARGET,
                                D3DCOLOR_RGBA(0,63,0,0),
                                0,
                                0);
    } while(0);

    return hr;
} // InitDirect3D

void ShutdownDirect3D()
{
    HELPER_RELEASE(pID3DDevice);
    HELPER_RELEASE(pID3D);
}

