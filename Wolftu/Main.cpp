#define WIN32_LEAN_AND_MEAN
#define FMT_HEADER_ONLY


#include "Include.h"
#include "Driver.hpp"
#include <mutex>
#include "Macro.hpp"
//#include "xorstr.hpp"
#include <fstream>
#include "Kernel_Exe.h"
#include "Kernel_Sys.h"
#include <iostream>
#include <string>

CFunctions* PFunctions;
CD3DXSDK* PD3DXSDK;

bool ShowMenu = true;
float overlayConsts[4];

std::string Status_NameEsp = "0";
std::string Status_NoRecoil = "0";
std::string Status_InstantRespawn = "0";
std::string Status_GodMode = "0";
std::string Status_TeamKill = "0";
std::string Status_AlwaysHS = "0";
std::string Status_Kill_OneHit = "0";
std::string Status_AntiStun_Freeze = "0";
std::string Status_NoFallDmg = "0";
std::string Status_NoReload = "0";
std::string Status_RapidFire = "0";

bool NameEsp = false;
bool NoRecoil = false;
bool InstantRespawn = false;
bool GodMode = false;
bool TeamKill = false;
bool AlwaysHS = false;
bool Kill_OneHit = false;
bool AntiStun_Freeze = false;
bool NoFallDmg = false;
bool NoReload = false;
bool RapidFire = false;

bool Bypass = false;

bool ModeEnable = false;
int ModeType = 0;

uint32_t WolfteamBase;
uint32_t cShellBase;
uint32_t ObjectBase;
uint32_t OleBase;
uint32_t MsvcrBase;


void TextCentered(std::string text) {
	auto windowWidth = ImGui::GetWindowSize().x;
	auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void __fastcall Render()
{
	setlocale(LC_ALL, st("Turkish"));

	::ImGui_ImplDX11_NewFrame();
	::ImGui_ImplWin32_NewFrame();

	::ImGui::NewFrame();

	::ImGui::GetIO().IniFilename = nullptr;

	ImVec2 window_pos, window_pos_pivot;
	window_pos.y = 25.0f;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

	ImGui::SetNextWindowBgAlpha(0.35f);


	ImGui::End();

	long style2 = ::GetWindowLong(OverlayInfo.Hwnd, GWL_EXSTYLE);

	if (ShowMenu) {
		if (ImGui::Begin(st("Discord"), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text(st("CH34TGL0BAL"));
		}


		style2 &= ~WS_EX_LAYERED;
		::SetWindowLong(OverlayInfo.Hwnd, GWL_EXSTYLE, style2);
		::SetForegroundWindow(OverlayInfo.Hwnd);

		ImGui::Begin(st("##3Discord"), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove);
		{
			ImGui::NewLine();
			TextCentered(st("CH34TGL0BAL - Free Wolftu"));
			ImGui::NewLine();
			ImGui::NewLine();

			ImGui::Checkbox(st("Name ESP"), &NameEsp); ImGui::SameLine(); HelpMarker(st("Karsi takimdaki dusmanlarin yerini gosterir."));
			ImGui::Checkbox(st("No Reload"), &NoReload);             ImGui::SameLine(); HelpMarker(st("Mermi doldururken animasyonu siler ve aninda mermi doldurur."));
			ImGui::Checkbox(st("No Recoil"), &NoRecoil);  ImGui::SameLine(); HelpMarker(st("Silahin geri tepmesini siler ve asla tepmez."));
			ImGui::Checkbox(st("Instant Respawn"), &InstantRespawn);  ImGui::SameLine(); HelpMarker(st("Hemen olur olmez aninda canlanirsiniz."));
			ImGui::Checkbox(st("GodMode H4ck"), &GodMode);  ImGui::SameLine(); HelpMarker(st("Olumsuz olmanizi saglar ve sizi kimse olduremez."));
			ImGui::Checkbox(st("Team Kill H4ck"), &TeamKill);  ImGui::SameLine(); HelpMarker(st("Olumsuz olmanizi saglar ve sizi kimse olduremez."));
			ImGui::Checkbox(st("Always HS"), &AlwaysHS);  ImGui::SameLine(); HelpMarker(st("Attiginiz her mermi ayaga bile siksaniz kafaya gidecektir."));
			ImGui::Checkbox(st("Kill One Hit (Acarsan Name ESP calismaz)"), &Kill_OneHit); ImGui::SameLine(); HelpMarker(st("Tek vurusta dusmani oldurmenizi sagðlar."));
			ImGui::Checkbox(st("Anti Stun UnFreeze"), &AntiStun_Freeze);  ImGui::SameLine(); HelpMarker(st("Asla sersemlik islemez veya sizi donduramazlar."));
			ImGui::Checkbox(st("No Fall Damage"), &NoFallDmg); ImGui::SameLine(); HelpMarker(st("Yuksekten dusunce caniniz eksilmez yada olmezsiniz"));
			ImGui::Checkbox(st("Rapid Fire"), &RapidFire); ImGui::SameLine(); HelpMarker(st("Saniyede 20 mermi atmanizi saglar cok hizli ates edersiniz."));

			ImGui::NewLine();
		}
		ImGui::End();
	}
	else if (!ShowMenu)
	{
		style2 |= WS_EX_LAYERED;
		::SetWindowLong(OverlayInfo.Hwnd, GWL_EXSTYLE, style2);
	}

	::ImGui::EndFrame();

	::ImGui::Render();
}

void __fastcall MainLoop()
{
	if (!PD3DXSDK->CreateDeviceD3D(OverlayInfo.Hwnd, ProcessInfo.WindowWidth, ProcessInfo.WindowHeight))
	{
		PD3DXSDK->CleanupDeviceD3D();
	}

	::ShowWindow(OverlayInfo.Hwnd, SW_SHOW);
	::UpdateWindow(OverlayInfo.Hwnd);

	::ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.Fonts->AddFontFromFileTTF(st("C:\\Windows\\Fonts\\Ruda-Bold.ttf"), 12);
	io.Fonts->AddFontFromFileTTF(st("C:\\Windows\\Fonts\\Ruda-Bold.ttf"), 10);
	io.Fonts->AddFontFromFileTTF(st("C:\\Windows\\Fonts\\Ruda-Bold.ttf"), 14);
	io.Fonts->AddFontFromFileTTF(st("C:\\Windows\\Fonts\\Ruda-Bold.ttf"), 18);

	ImGui::StyleColorsLight();

	bool done = false;
	while (!done)
	{
		if (GetAsyncKeyState(VK_END) & 1) exit(0);
		if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;

		while (::PeekMessage(&Interface.Message, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&Interface.Message);
			::DispatchMessage(&Interface.Message);
			if (Interface.Message.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		PFunctions->ForeGroundFunctions(io);

		Render();


		Interface.g_pd3dDeviceContext->ClearRenderTargetView(Interface.g_mainRenderTargetView, overlayConsts);
		Interface.g_pd3dDeviceContext->OMSetRenderTargets(1, &Interface.g_mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		Interface.g_pSwapChain->Present(1, 0);
	}

	::ImGui_ImplDX11_Shutdown();
	::ImGui_ImplWin32_Shutdown();
	::ImGui::DestroyContext();


	if (Interface.g_pd3dDeviceContext != NULL) {
		Interface.g_pd3dDeviceContext->Release();
	}

	PD3DXSDK->CleanupDeviceD3D();
	::DestroyWindow(OverlayInfo.Hwnd);
	::UnregisterClass(OverlayInfo.WindowClass.lpszClassName, OverlayInfo.WindowClass.hInstance);

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __fastcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (Interface.g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX11_InvalidateDeviceObjects();
			PD3DXSDK->CleanupRenderTarget();
			Interface.g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			PD3DXSDK->CreateRenderTarget();
			ImGui_ImplDX11_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


auto __fastcall Object(HMODULE hModule) -> void
{
	while (true)
	{
		ObjectBase = memory::getModuleAddress(2);
		Sleep(500);
	}
}

auto __fastcall Visuals(HMODULE hModule) -> void
{
	while (true)
	{
		if (ObjectBase != NULL)
		{
			//######
			Sleep(3000);
		}

		// ELSE SONRASI ÇALIÞMAYANLARDIR

		if (NameEsp)
		{
			memory::write<uint32_t>(cShellBase + 0x3767C5, 3465227124);
			if (Status_NameEsp == "1") {
				Status_NameEsp = "0";
			}
		}
		else
		{
			if (Status_NameEsp == "0") {
				Status_NameEsp = "1";
				//memory::write<uint32_t>(cShellBase + 0x3767C5, 3465227125);
			}
		}
		//##
		if (InstantRespawn)
		{
			memory::write<uint32_t>(cShellBase + 0x779D5, 1574503796);
			if (Status_InstantRespawn == "1") {
				Status_InstantRespawn = "0";
			}
		}
		else
		{
			if (Status_InstantRespawn == "0") {
				Status_InstantRespawn = "1";
				//memory::write<uint32_t>(cShellBase + 0x779D5, 1574503797);
			}
		}
		//##
		if (GodMode)
		{
			memory::write<uint32_t>(ObjectBase + 0x2EBE4, 3641198040);
			if (Status_GodMode == "1") {
				Status_GodMode = "0";
			}
		}
		else
		{
			if (Status_GodMode == "0") {
				Status_GodMode = "1";
				//memory::write<uint32_t>(ObjectBase + 0x2EBE4, 3641198041);
			}
		}
		//##
		if (AlwaysHS)
		{
			memory::write<uint32_t>(ObjectBase + 0x83DF5, 1566441904);
			if (Status_AlwaysHS == "1") {
				Status_AlwaysHS = "0";
			}
		}
		else
		{
			if (Status_AlwaysHS == "0") {
				Status_AlwaysHS = "1";
				//memory::write<uint32_t>(ObjectBase + 0x83DF5, 1566490674);
			}
		}
		//##
		if (NoReload)
		{
			memory::write<uint32_t>(cShellBase + 0x1392EE, 2257258100);
			if (Status_NoReload == "1") {
				Status_NoReload = "0";
			}
		}
		else
		{
			if (Status_NoReload == "0") {
				Status_NoReload = "1";
				//memory::write<uint32_t>(cShellBase + 0x1392EE, 2257258101);
			}
		}
		//##
		if (RapidFire)
		{
			memory::write<uint32_t>(cShellBase + 0x13C968, 2654543220);
			if (Status_RapidFire == "1") {
				Status_RapidFire = "0";
			}
		}
		else
		{
			if (Status_RapidFire == "0") {
				Status_RapidFire = "1";
				//memory::write<uint32_t>(cShellBase + 0x13C968, 1960467318);
			}
		}
		//##
		if (NoFallDmg)
		{
			memory::write<uint32_t>(cShellBase + 0x118943, 2654543220);
			if (Status_NoFallDmg == "1") {
				Status_NoFallDmg = "0";
			}
		}
		else
		{
			if (Status_NoFallDmg == "0") {
				Status_NoFallDmg = "1";
				//memory::write<uint32_t>(cShellBase + 0x118943, 2654543221);
			}
		}

		//#####################################//
		//ÝPTAL EDÝLENLER
		if (AntiStun_Freeze)
		{
			//memory::write<uint32_t>(cShellBase + 0xCA28E, 1574503796);
		}
		else
		{
			//memory::write<uint32_t>(cShellBase + 0xCA28E, 1574503797);
		}
		if (TeamKill)
		{
			//memory::write<uint32_t>(ObjectBase + 0x1B8815, 4286937971);
		}
		else
		{
			//memory::write<uint32_t>(ObjectBase + 0x1B8815, 4286937972);
		}
		if (NoRecoil)
		{
			//memory::write<uint32_t>(cShellBase + 0x1221EC8, 0);
			//memory::write<uint32_t>(ObjectBase + 0x13C815, 1301090420);
		}
		else
		{
			//memory::write<uint32_t>(cShellBase + 0x1221EC8, 1078530011);
			//memory::write<uint32_t>(ObjectBase + 0x13C815, 1301090421);
		}
		if (Kill_OneHit)
		{
			//memory::write<uint32_t>(ObjectBase + 0x172615, 5);
		}
		else
		{
			//memory::write<uint32_t>(ObjectBase + 0x172615, 1967244534);
		}
	}
}


auto __fastcall CheckingWolfteam(HMODULE hModule) -> void
{
	while (true)
	{
		if (memory::getPid(st("Wolfteam.bin")) == NULL)
		{
			exit(0);
		}
	}
}

auto __fastcall Setup() -> bool
{
	if (!OpenMutexA(SYNCHRONIZE, false, (st("uyg263mgba0moadnvc9h")))) {
	return false;
	}

	const char* discordProcessName = "Discord.exe";
	const char* wolfeamProcessName = "Wolfteam.bin";
	const char* vgkProcessName = "vgtray.exe";
	
	HANDLE discordSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 discordProcessEntry;
	discordProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	bool discordRunning = false;
	if (Process32First(discordSnapshot, &discordProcessEntry)) {
		do {
			if (_stricmp(discordProcessEntry.szExeFile, discordProcessName) == 0) {
				discordRunning = true;
				break;
			}
		} while (Process32Next(discordSnapshot, &discordProcessEntry));
	}
	CloseHandle(discordSnapshot);
	if (!discordRunning) {
		MessageBox(NULL, "Discord.exe is not found. Please open Discord..", "Error", MB_OK);
		return false;
	}
	
	const HANDLE wolfeamSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 wolfeamProcessEntry;
	wolfeamProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	bool wolfeamRunning = false;
	if (Process32First(wolfeamSnapshot, &wolfeamProcessEntry)) {
		do {
			if (_stricmp(wolfeamProcessEntry.szExeFile, wolfeamProcessName) == 0) {
				wolfeamRunning = true;
				break;
			}
		} while (Process32Next(wolfeamSnapshot, &wolfeamProcessEntry));
	}
	CloseHandle(wolfeamSnapshot);
	if (!wolfeamRunning) {
		MessageBox(NULL, "Wolfeam.bin is not found. Please open Wolfeam.", "Error", MB_OK);
		return false;
	}
	
	const HANDLE vgkSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 vgkProcessEntry;
	vgkProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	bool vgkRunning = false;
	if (Process32First(vgkSnapshot, &vgkProcessEntry)) {
		do {
			if (_stricmp(vgkProcessEntry.szExeFile, vgkProcessName) == 0) {
				vgkRunning = true;
				break;
			}
		} while (Process32Next(vgkSnapshot, &vgkProcessEntry));
	}
	CloseHandle(vgkSnapshot);
	if (vgkRunning) {
		MessageBox(NULL, "Vanguard is detected! Please uninstall and remove VanGuard! Restart computer and try again.", "Error", MB_OK);
		const char* command1 = "sc stop vgc";
		int result1 = system(command1);
		return false;
	}

	const std::size_t byte_size = sizeof(kernel_data);
	const char* tempFolder = std::getenv("TEMP");
	if (!tempFolder) {
		std::cerr << "Temp folder not found." << std::endl;
		return false;
	}
	std::string filePath = std::string(tempFolder) + "\\WinCRT.exe";
	std::ofstream outFile(filePath, std::ios::binary);
	if (outFile.is_open()) {
		outFile.write(reinterpret_cast<const char*>(kernel_data), byte_size);
		outFile.close();
	}
	else {
		std::cerr << "File is not write. Please close AntiVirus or Windows Defender" << std::endl;
		return false;
	}
	const std::size_t byte_size2 = sizeof(kernel_data2);
	const char* tempFolder2 = std::getenv("TEMP");
	if (!tempFolder2) {
		std::cerr << "Temp folder not found." << std::endl;
		return false;
	}
	std::string filePath2 = std::string(tempFolder2) + "\\WinCRT.sys";
	std::ofstream outFile2(filePath2, std::ios::binary);
	if (outFile2.is_open()) {
		outFile2.write(reinterpret_cast<const char*>(kernel_data2), byte_size2);
		outFile2.close();
	}
	else {
		std::cerr << "File is not write. Please close AntiVirus or Windows Defender" << std::endl;
		return false;
	}

	const char* command2 = """%temp%\\WinCRT.exe"" ""%temp%\\WinCRT.sys""";
	int result2 = system(command2);

	const char* command3 = "start https://href.li/?https://discord.gg/HkHBvgbsfw";
	int result3 = system(command3);

	if (!memory::checkingDriver()) 
	{
		MessageBox(NULL, "Driver Connection Error! Please try restart computer.", "Error", MB_OK);
		return false;
	}

	const char* command5 = "cls";
	int result5 = system(command5);

	std::cout << st("[CG-LOG] HCode: ") << (int*)memory::DriverHandle << std::endl;

	Sleep(1000);

	std::cout << st("[CG-LOG] Waiting for Wolfteam...") << std::endl;

	DWORD pId;

	while (true)
	{
		pId = memory::getPid(st("Wolfteam.bin"));

		if (pId != NULL)
		{
			break;
		}
	}

	Sleep(1000);

	if (!memory::initialize(pId))
	{
		return false;
	}

	std::cout << st("[CG-LOG] Wolfteam Found.") << std::endl;

	Sleep(1000);

	std::cout << st("[CG-LOG] Loading CH34T settings...") << std::endl;


	Sleep(1500);

	WolfteamBase = memory::getModuleAddress(0);

	while (true)
	{
		cShellBase = memory::getModuleAddress(1);
		if (cShellBase != NULL)
		{
			Sleep(600);
			break;
		}
	}

	WolfteamBase = memory::getModuleAddress(0);
	cShellBase = memory::getModuleAddress(1);
	MsvcrBase = memory::getModuleAddress(4);

	return true;
}

auto main() -> int
{
	if (Setup())
	{
		HANDLE hdl = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Visuals), nullptr, NULL, nullptr);
		CloseHandle(hdl);

		HANDLE hdl2 = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Object), nullptr, NULL, nullptr);
		CloseHandle(hdl2);

		HANDLE hdl3 = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(CheckingWolfteam), nullptr, NULL, nullptr);
		CloseHandle(hdl3);


		::ShowWindow(::GetConsoleWindow(), SW_HIDE);

		PFunctions->OverlaySettings(memory::getPid(st("discord.exe")));
		PFunctions->SetupWindow(WndProc);

		MainLoop();
	}

	exit(0);

	return 0;
}