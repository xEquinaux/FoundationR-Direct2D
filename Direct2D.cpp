#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effecthelpers.h>
#include <d3d10.h>
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

DLL_EXPORT void Direct3D_Init()
{
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
    {
        return;
    }

    ID3D10Device* d3dDevice = nullptr;
    HRESULT result = D3D10CreateDevice(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, 0, D3D10_SDK_VERSION, &d3dDevice);
    if (FAILED(result))
    {
        return;
    }
    IDXGIDevice* dxgiDevice = nullptr;
    d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    
    ID3D10Texture2D* tex = GenerateTexture(d3dDevice);
    IDXGISurface* dxgiSurface = nullptr;
    HRESULT hr2 = tex->QueryInterface(IID_IDXGISurface, reinterpret_cast<void**>(&dxgiSurface));
    if (FAILED(hr2))
    {
        return;
    }

    D2D1_CREATION_PROPERTIES creationProperties = {};
    m_d2dContext = nullptr;
    D2D1CreateDeviceContext(dxgiSurface, creationProperties, &m_d2dContext);

    tex->Release();
    dxgiSurface->Release();
    dxgiDevice->Release();
    d3dDevice->Release();
    CoUninitialize();
}

ID3D10Texture2D* GenerateTexture(ID3D10Device* device)
{
    // Assume you have an existing ID3D11Device* device (created elsewhere)

    // Describe the texture properties
    D3D10_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = _WIDTH; // Width in pixels
    textureDesc.Height = _HEIGHT; // Height in pixels
    textureDesc.MipLevels = 1; // Number of mip levels (usually 1 for simplicity)
    textureDesc.ArraySize = 1; // Number of textures in the array
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format
    textureDesc.SampleDesc.Count = 1; // No multisampling
    textureDesc.Usage = D3D10_USAGE_DEFAULT; // Usage (other options available)
    textureDesc.BindFlags = D3D10_BIND_INDEX_BUFFER; // How the texture will be used
    textureDesc.CPUAccessFlags = 0; // No CPU access
    textureDesc.MiscFlags = 0; // No special flags

    // Create the texture
    ID3D10Texture2D* texture2D = nullptr;
    HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &texture2D);
    if (FAILED(hr)) {
        // Handle creation failure
        // ...
    }
    return texture2D;
}

// Initialize Direct2D
DLL_EXPORT void Direct2D_Init(HWND _hwnd, UINT width, UINT height)
{
    _WIDTH = width;
    _HEIGHT = height;
    pBackBuffer = CreateD2DBitmapFromARGBArray(nullptr, width, height);

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
    hWnd = _hwnd;//CreateWindow(L"WindowClass", L"Direct2D Example", WS_OVERLAPPEDWINDOW,
        //CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, nullptr, nullptr);

    // Create a render target
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
    pD2DFactory->CreateHwndRenderTarget(props, D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(width, height)), &pRenderTarget);
}

DLL_EXPORT void Direct3D_Begin()
{
    m_d2dContext->BeginDraw();
    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
}

DLL_EXPORT void Direct3D_Composite(BYTE* argbBytes, UINT x, UINT y, UINT width, UINT height)
{
    ID2D1Bitmap* pBitmap = CreateD2DBitmapFromARGBArray(argbBytes, width, height);
    if (pBitmap)
    {
        ID2D1Effect* compositeEffect = nullptr;
        
        m_d2dContext->CreateEffect(CLSID_D2D1Composite, &compositeEffect);
        compositeEffect->SetInput(0, pBackBuffer);
        compositeEffect->SetInput(1, pBitmap);
        
        compositeEffect->Release();
    }
}

DLL_EXPORT void Direct3D_Render()
{
    m_d2dContext->DrawBitmap(pBackBuffer);
}

DLL_EXPORT void Direct3D_End()
{
    m_d2dContext->EndDraw();
}

DLL_EXPORT void Direct2D_Begin()
{
    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
}

// Draw something
DLL_EXPORT void Direct2D_Composite(BYTE* argbBytes, UINT x, UINT y, UINT width, UINT height)
{
    // Assume you have an ARGB byte array named 'argbBytes'
    ID2D1Bitmap* pBitmap = CreateD2DBitmapFromARGBArray(argbBytes, width, height);
    if (pBitmap) 
    {
        pRenderTarget->DrawBitmap(pBitmap, D2D1::RectF(x, y, width, height));
    }
}

DLL_EXPORT void Direct2D_End()
{
    pRenderTarget->EndDraw();
}

DLL_EXPORT void Direct2D_Dispose()
{
    // Cleanup
    pRenderTarget->Release();
    pD2DFactory->Release();
    g_pWICFactory->Release();
    pBackBuffer->Release();
    DestroyWindow(hWnd);
}
