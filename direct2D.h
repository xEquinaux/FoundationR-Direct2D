#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

ID2D1Factory* pD2DFactory = nullptr;
IWICImagingFactory* g_pWICFactory = nullptr;
ID2D1HwndRenderTarget* pRenderTarget = nullptr;
ID2D1Bitmap* pBackBuffer = nullptr;
ID2D1DeviceContext* m_d2dContext = nullptr;
HWND hWnd = nullptr;
UINT _WIDTH = 0;
UINT _HEIGHT = 0;
