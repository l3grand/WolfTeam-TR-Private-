#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER

#include "Include.h"

class CFunctions
{
public:
	std::string __fastcall RandomString(int len);
	int __fastcall ForeGroundFunctions(ImGuiIO& io);
	void __fastcall SetupWindow(LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam));
	int __fastcall OverlaySettings(int ProcID);
	void __fastcall ImGuiDesign(ImGuiStyle* style);
};


#endif