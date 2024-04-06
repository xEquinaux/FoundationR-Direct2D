#include <d3d11.h>
#include <d3dcompiler.h>
#include "directCompute.h"

// Initialize Direct3D and the compute shader
void Initialize()
{
    // Create device and context
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &device, nullptr, &context);

    // Load and compile the compute shader
    ID3DBlob* csBlob;
    D3DCompileFromFile(L"ComputeShader.hlsl", nullptr, nullptr, "CSMain", "cs_5_0", 0, 0, &csBlob, nullptr);

    // Create the compute shader
    device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &computeShader);
}

// Run the compute shader
void RunComputeShader()
{
    // Set the compute shader
    context->CSSetShader(computeShader, nullptr, 0);

    // Dispatch the compute shader
    context->Dispatch(1, 1, 1);
}

//int main()
//{
//    Initialize();
//    RunComputeShader();
//    return 0;
//}
