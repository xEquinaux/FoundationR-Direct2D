#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

ID2D1Factory* pD2DFactory = nullptr;
IWICImagingFactory* g_pWICFactory = nullptr;
ID2D1HwndRenderTarget* pRenderTarget = nullptr;
ID2D1Image* pBackBuffer = nullptr;
ID2D1DeviceContext* m_d2dContext = nullptr;
HWND hWnd = nullptr;
ID3D10Device* d3dDevice = nullptr;
IDXGIDevice* dxgiDevice = nullptr;
ID3D10Texture2D* tex = nullptr;
IDXGISurface* dxgiSurface = nullptr;
ID2D1BitmapRenderTarget* bmpRT = NULL;
UINT _WIDTH = 0;
UINT _HEIGHT = 0;
