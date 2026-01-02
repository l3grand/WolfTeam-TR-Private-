#pragma once
#include <iostream>

#include <Windows.h>
#include <direct.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>

#include <shellapi.h>
#include <d3d11.h>
#include <dwmapi.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")

typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;
#include "D3DXSDK.h"
#include "Memory.h"
#include "Logger.hpp"
#include "Functions.h"


using namespace std;