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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning(disable:4127)  // Conditional expression is constant.

// This is a framework for the DirectX demos.  Demo modules must implement
// the following functions:
HRESULT InitDirect3D(_In_ HWND);
void ShutdownDirect3D();
HRESULT DrawScene();

// a simple WndProc that validates a window and kills it when the time comes.
LRESULT CALLBACK WndProc(_In_ HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch(iMsg)
    {
        case WM_ERASEBKGND:
        {
            return 0;
        }

        case WM_DESTROY:
        {
            ::PostQuitMessage(0);
            return 0;
        }
    }

    return ::DefWindowProc(hwnd, iMsg, wParam, lParam);
}

// The main purpose of this function is to build a window and call the hook functions.
int WINAPI WinMain(_In_ HINSTANCE hInstance, HINSTANCE, LPSTR, int iCmdShow)
{
    const TCHAR szClassName[] = TEXT("DirectX 9 Tutorial");

    WNDCLASSEX wndclass =
    {
        sizeof(wndclass),                       // cbSize
        CS_HREDRAW | CS_VREDRAW,                // style
        WndProc,                                // lpfnWndProc
        0,                                      // cbClsExtra
        0,                                      // cbWndExtra
        hInstance,                              // hInstance
        ::LoadIcon(nullptr, IDI_APPLICATION),   // hIcon
        ::LoadCursor(nullptr, IDC_ARROW),       // hCursor
        nullptr,                                // hBackground
        nullptr,                                // lpszMenuName
        szClassName,                            // lpszClassName
        ::LoadIcon(nullptr, IDI_APPLICATION)    // hIconSm
    };

    RegisterClassEx(&wndclass);

    HWND hwnd = ::CreateWindow(szClassName,             // lpClassName
                               szClassName,             // lpWindowName
                               WS_OVERLAPPEDWINDOW,     // dwStyle
                               CW_USEDEFAULT,           // x
                               CW_USEDEFAULT,           // y
                               CW_USEDEFAULT,           // nWidth
                               CW_USEDEFAULT,           // nHeight
                               nullptr,                 // hwndParent
                               nullptr,                 // hMenu
                               hInstance,               // hInstance
                               nullptr);                // lpParam

    // In a real app, you might want to give an error message here.
    if(FAILED(InitDirect3D(hwnd)))
    {
        return 0;
    }

    ::ShowWindow(hwnd, iCmdShow);
    ::UpdateWindow(hwnd);

    while(true)
    {
        // Clear out all the messages before we draw a new frame.
        MSG msg;
        while(::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if(WM_QUIT == msg.message)
            {
                ShutdownDirect3D();
                return static_cast<int>(msg.wParam);
            }

            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }

        // draw a frame and end if we error
        if(FAILED(DrawScene()))
        {
            ::DestroyWindow(hwnd);
        }
    }
}

