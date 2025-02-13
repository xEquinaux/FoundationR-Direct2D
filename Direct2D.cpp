#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include "direct2D.h"
#include "pch.h"
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

#  define DLL_EXPORT extern "C" __declspec(dllexport)

ID2D1Bitmap* CreateD2DBitmapFromARGBArray(BYTE* argbBytes, UINT width, UINT height)
{
    // Create a WIC bitmap from the ARGB byte array
    IWICBitmap* pWICBitmap = nullptr;
    HRESULT hr = g_pWICFactory->CreateBitmapFromMemory(width, height,
        GUID_WICPixelFormat32bppPBGRA, width * 4, width * height * 4,
        argbBytes, &pWICBitmap);
    if (FAILED(hr))
    {
        // Handle error
        return nullptr;
    }

    // Create an ID2D1Bitmap from the WIC bitmap
    ID2D1Bitmap* pD2DBitmap = nullptr;
    HRESULT hr_ = pRenderTarget->CreateBitmapFromWicBitmap(pWICBitmap, &pD2DBitmap);
    if (FAILED(hr_))
    {
        // Handle error
        return nullptr;
    }

    // Release the WIC bitmap
    pWICBitmap->Release();

    return pD2DBitmap;
}

// Initialize Direct2D with CreateWindow
DLL_EXPORT void Direct2D_Init(UINT width, UINT height)
{
    _WIDTH = width;
    _HEIGHT = height; 
     
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

    // Create WIC factory
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory, reinterpret_cast<void**>(&g_pWICFactory));
    if (FAILED(hr))
    {
        // Handle error
        return;
    }

    // Create a window
    hWnd = CreateWindow(L"WindowClass", L"Direct2D Example", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, nullptr, nullptr);

    // Show the window
    ShowWindow(hWnd, 1);
    UpdateWindow(hWnd);
    
    // Create a render target
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
    pD2DFactory->CreateHwndRenderTarget(props, D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(width, height)), &pRenderTarget);
}

// Initialize Direct2D with external hWnd
DLL_EXPORT void Direct2D_InitEx(HWND hwnd, UINT width, UINT height)
{
    _WIDTH = width;
    _HEIGHT = height;
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

    // Create WIC factory
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory, reinterpret_cast<void**>(&g_pWICFactory));
    if (FAILED(hr))
    {
        // Handle error
        return;
    }

    // Assign global variable
    hWnd = hwnd;

    // Create a render target
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
    pD2DFactory->CreateHwndRenderTarget(props, D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(width, height)), &pRenderTarget);
}


DLL_EXPORT void Direct2D_Clear()
{
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
}

DLL_EXPORT void Direct2D_Begin()
{
    pRenderTarget->BeginDraw();
}

DLL_EXPORT void Direct2D_End()
{
    pRenderTarget->EndDraw();
}

// Draw something
DLL_EXPORT void Direct2D_Draw(BYTE* argbBytes, UINT x, UINT y, UINT width, UINT height)
{
    // Assume you have an ARGB byte array named 'argbBytes'
    ID2D1Bitmap* pBitmap = CreateD2DBitmapFromARGBArray(argbBytes, width, height);
    if (pBitmap)
    {
        if (x < 0 || y < 0 || x >= _WIDTH || y >= _HEIGHT)
        {
            return;
        }
        // Draw the bitmap using pRenderTarget
        pRenderTarget->DrawBitmap(pBitmap, D2D1::RectF(x, y, x + width, y + height));
        pBitmap->Release();
    }
}

// Draw something
DLL_EXPORT void Direct2D_Render(BYTE* argbBytes, UINT width, UINT height)
{
    // Assume you have an ARGB byte array named 'argbBytes'
    ID2D1Bitmap* pBitmap = CreateD2DBitmapFromARGBArray(argbBytes, width, height);
    if (pBitmap)
    {
        // Draw the bitmap using pRenderTarget
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        pRenderTarget->DrawBitmap(pBitmap, D2D1::RectF(0, 0, width, height));
        pRenderTarget->EndDraw();
        pBitmap->Release();
    }
}

DLL_EXPORT void Direct2D_Dispose()
{
    // Cleanup
    pRenderTarget->Release();
    pD2DFactory->Release();
    g_pWICFactory->Release();
    DestroyWindow(hWnd);
}

DLL_EXPORT LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SetPixel(hdc, 50, 50, RGB(255, 0, 0)); // Draw a red pixel at (50, 50)
        EndPaint(hwnd, &ps);
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

DLL_EXPORT int CreateWnd(HINSTANCE hInstance, HINSTANCE hPrevInstance,  int nShowCmd) {
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    //wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        nullptr,//CLASS_NAME,
        nullptr,//"Learn to Program Windows",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr) {
        return 0;
    }

    ShowWindow(hwnd, nShowCmd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}