#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

ID2D1Factory* pD2DFactory = nullptr;
IWICImagingFactory* g_pWICFactory = nullptr;
ID2D1HwndRenderTarget* pRenderTarget = nullptr;
HWND hWnd = nullptr;