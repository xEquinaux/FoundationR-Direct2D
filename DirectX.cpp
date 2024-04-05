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

#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Graphics.DirectX.h>

#  define DLL_EXPORT extern "C" __declspec(dllexport)

using namespace winrt;
using namespace Windows::UI::Composition;
using namespace Windows::Graphics::DirectX;

void Main()
{
    // Create CompositionGraphicsDevice (assumes existing DirectX device)
    CompositionGraphicsDevice graphicsDevice = CompositionGraphicsDevice::CreateForCurrentThread();

    // Create a surface with an initial size (e.g., 800x600)
    CompositionDrawingSurface surface = graphicsDevice.CreateDrawingSurface(
        Size{ 800, 600 }, DirectXPixelFormat::B8G8R8A8UIntNormalized, DirectXAlphaMode::Premultiplied);

    // Load pixels into the surface
    auto d2dContext = surface.BeginDraw();
    // Render or upload pixels into d2dContext (ID2D1DeviceContext)
    // Call EndDraw when done

    // Use the surface in your visual tree (e.g., via CompositionSurfaceBrush)
}