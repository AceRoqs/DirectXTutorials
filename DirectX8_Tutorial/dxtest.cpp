#define WIN32_LEAN_AND_MEAN
#include <afxwin.h>
#include <windows.h>

// function prototypes for D3D
HRESULT InitDirect3D(HWND);
void ShutdownDirect3D();
HRESULT DrawScene();

// a simple WndProc that validates a window and kills it when the time comes
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch(iMsg)
    {
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

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while(1)
    {
        // clear out all the messages before we draw a new frame
        MSG msg;
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(WM_QUIT == msg.message)
            {
                ShutdownDirect3D();
                return msg.wParam;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // draw a frame and end if we error
        if(FAILED(DrawScene()))
        {
            DestroyWindow(hwnd);
        }
    }
} // WinMain
