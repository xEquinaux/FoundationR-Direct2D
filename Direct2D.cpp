#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effecthelpers.h>
#include <d3d10.h>
#include <wincodec.h>
#include "direct2D.h"
#include "pch.h"
#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d10")
#pragma comment(lib, "dxguid")
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

ID2D1Bitmap* ConvertImageToBitmap(ID2D1Image* pImg, D2D1_SIZE_U size)
{
    ID2D1Image* oldTarget = NULL;
    ID2D1Bitmap1* targetBitmap = NULL;

    //Create a Bitmap with "D2D1_BITMAP_OPTIONS_TARGET"
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );
    m_d2dContext->CreateBitmap(size, 0, 0, bitmapProperties, &targetBitmap);

    //Save current Target, replace by ID2D1Bitmap
    m_d2dContext->GetTarget(&oldTarget);
    m_d2dContext->SetTarget(targetBitmap);
    //Draw Image on Target (if currently not drawing also call Begin/EndDraw)
    m_d2dContext->DrawImage(pImg);

    //Set previous Target
    m_d2dContext->SetTarget(oldTarget);

    oldTarget->Release();

    return targetBitmap;
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

DLL_EXPORT void Direct3D_Init(BYTE* argbBytes, UINT width, UINT height)
{
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
    {
        return;
    }

    HRESULT result = D3D10CreateDevice(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, 0, D3D10_SDK_VERSION, &d3dDevice);
    if (FAILED(result))
    {
        return;
    }
    d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    
    ID3D10Texture2D* tex = GenerateTexture(d3dDevice);
    HRESULT hr2 = tex->QueryInterface(IID_IDXGISurface, reinterpret_cast<void**>(&dxgiSurface));
    if (FAILED(hr2))
    {
        return;
    }

    D2D1_CREATION_PROPERTIES creationProperties = {};
    D2D1CreateDeviceContext(dxgiSurface, creationProperties, &m_d2dContext);
    pBackBuffer = CreateD2DBitmapFromARGBArray(argbBytes, width, height);
}

// Initialize Direct2D
DLL_EXPORT void Direct2D_Init(HWND _hwnd, UINT width, UINT height)
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
    hWnd = _hwnd;//CreateWindow(L"WindowClass", L"Direct2D Example", WS_OVERLAPPEDWINDOW,
        //CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, nullptr, nullptr);
    
        // Create a render target
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
    pD2DFactory->CreateHwndRenderTarget(props, D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(width, height)), &pRenderTarget);
    pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Create bitmap render target
    //pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF(_WIDTH, _HEIGHT), &bmpRT);
}

DLL_EXPORT void Direct3D_Draw(BYTE* argbBytes, UINT x, UINT y, UINT width, UINT height)
{
    ID2D1Bitmap* pBitmap = CreateD2DBitmapFromARGBArray(argbBytes, width, height);
    if (pBitmap)
    {
        ID2D1BitmapRenderTarget* bmpRT = nullptr;
        ID2D1Effect* compositeEffect = nullptr;

        HRESULT hr = m_d2dContext->CreateEffect(CLSID_D2D1Composite, &compositeEffect);
        if (SUCCEEDED(hr))
        {
            
            // Set inputs in the correct order
            compositeEffect->SetInput(0, pBitmap);
            compositeEffect->SetInput(1, pBackBuffer);

            // Get the output
            ID2D1Image* pNewBackBuffer = nullptr;
            compositeEffect->GetOutput(&pNewBackBuffer);

            // Release the old back buffer and update it
            if (pBackBuffer)
                pBackBuffer->Release();
            pBackBuffer = pNewBackBuffer;
        }
        else
        {
            // Handle error (e.g., log or display a message)
            // Don't forget to release pBitmap here as well
            pBitmap->Release();
        }
    }
}

DLL_EXPORT void Direct3D_Render()
{
    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
    pRenderTarget->DrawBitmap(ConvertImageToBitmap(pBackBuffer, D2D1::SizeU(_WIDTH, _HEIGHT)));
    pRenderTarget->EndDraw();
}

DLL_EXPORT void Direct2D_Begin(UINT viewX, UINT viewY)
{
    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
    D2D1_RECT_F clipRect = D2D1::RectF(0, 0, _WIDTH, _HEIGHT);
    pRenderTarget->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_ALIASED);
    D2D1_MATRIX_3X2_F transformMatrix = D2D1::Matrix3x2F::Translation(viewX, viewY);
    pRenderTarget->SetTransform(transformMatrix);
}

DLL_EXPORT void Direct2D_Draw(BYTE* argbBytes, UINT x, UINT y, UINT width, UINT height)
{
    ID2D1Bitmap* bmp = CreateD2DBitmapFromARGBArray(argbBytes, width, height);
    if (bmp)
    {
        pRenderTarget->DrawBitmap(bmp, D2D1::RectF(x, y, width, height), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
    bmp->Release();
}

DLL_EXPORT void Direct2D_End()
{
    pRenderTarget->PopAxisAlignedClip();
    pRenderTarget->EndDraw();
}

DLL_EXPORT void Dispose()
{
    // Cleanup
    bmpRT->Release();
    d3dDevice->Release();
    dxgiDevice->Release();
    tex->Release();
    dxgiSurface->Release();
    m_d2dContext->Release();
    pRenderTarget->Release();
    pD2DFactory->Release();
    g_pWICFactory->Release();
    pBackBuffer->Release();
    CoUninitialize();
    DestroyWindow(hWnd);
}
