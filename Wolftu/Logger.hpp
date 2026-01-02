#pragma once

struct GameInfos {
	LPCSTR GameName = "explorer.exe";
};

struct ProcessInfos {
	DWORD ID;
	HANDLE Handle;
	HWND Hwnd;
	WNDPROC WndProc;
	int WindowWidth;
	int WindowHeight;
	int WindowLeft;
	int WindowRight;
	int WindowTop;
	int WindowBottom;
	LPCSTR Title;
	LPCSTR ClassName;
	LPCSTR Path;
};

struct OverlayInfos {
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
};

struct DirectX11Interface {
	ID3D11Device* g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
};

extern DirectX11Interface Interface;

extern ProcessInfos ProcessInfo;
extern GameInfos GameInfo;
extern OverlayInfos OverlayInfo;

