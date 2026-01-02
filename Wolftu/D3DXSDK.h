#ifndef D3DXSDK_HEADER
#define D3DXSDK_HEADER

#include "Include.h"

class CD3DXSDK
{
public:
	bool CreateDeviceD3D(HWND hWnd, int WindowWidth, int WindowHeight);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
};


#endif