#include "D3DXSDK.h"

DirectX11Interface Interface;

bool CD3DXSDK::CreateDeviceD3D(HWND hWnd, int WindowWidth, int WindowHeight)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = WindowWidth;
    sd.BufferDesc.Height = WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &Interface.g_pSwapChain, &Interface.g_pd3dDevice, &featureLevel, &Interface.g_pd3dDeviceContext) != S_OK)
        return false;

    CD3DXSDK::CreateRenderTarget();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(Interface.g_pd3dDevice, Interface.g_pd3dDeviceContext);
    Interface.g_pd3dDevice->Release();
    return true;
}

void CD3DXSDK::CleanupDeviceD3D()
{
    CD3DXSDK::CleanupRenderTarget();
    if (Interface.g_pSwapChain) { Interface.g_pSwapChain->Release(); Interface.g_pSwapChain = NULL; }
    if (Interface.g_pd3dDeviceContext) { Interface.g_pd3dDeviceContext->Release(); Interface.g_pd3dDeviceContext = NULL; }
    if (Interface.g_pd3dDevice) { Interface.g_pd3dDevice->Release(); Interface.g_pd3dDevice = NULL; }
}

void CD3DXSDK::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    Interface.g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    Interface.g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &Interface.g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CD3DXSDK::CleanupRenderTarget()
{
    if (Interface.g_mainRenderTargetView) { Interface.g_mainRenderTargetView->Release(); Interface.g_mainRenderTargetView = NULL; }
}
