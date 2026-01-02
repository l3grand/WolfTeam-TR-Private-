#include "Functions.h"

OverlayInfos OverlayInfo;
ProcessInfos ProcessInfo;

std::string __fastcall CFunctions::RandomString(int len) {
	srand(time(NULL));
	std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string newstr;
	int pos;
	while (newstr.size() != len) {
		pos = ((rand() % (str.size() - 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

int __fastcall CFunctions::ForeGroundFunctions(ImGuiIO& io)
{
    static RECT OldRect;
    HWND ForegroundWindow = GetForegroundWindow();
    if (ForegroundWindow == ProcessInfo.Hwnd) {
        HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
        SetWindowPos(OverlayInfo.Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    RECT TempRect;
    POINT TempPoint;
    ZeroMemory(&TempRect, sizeof(RECT));
    ZeroMemory(&TempPoint, sizeof(POINT));

    GetClientRect(ProcessInfo.Hwnd, &TempRect);
    ClientToScreen(ProcessInfo.Hwnd, &TempPoint);

    TempRect.left = TempPoint.x;
    TempRect.top = TempPoint.y;
    io.ImeWindowHandle = ProcessInfo.Hwnd;

    POINT TempPoint2;
    GetCursorPos(&TempPoint2);
    io.MousePos.x = TempPoint2.x - TempPoint.x;
    io.MousePos.y = TempPoint2.y - TempPoint.y;

    if (GetAsyncKeyState(0x1)) {
        io.MouseDown[0] = true;
        io.MouseClicked[0] = true;
        io.MouseClickedPos[0].x = io.MousePos.x;
        io.MouseClickedPos[0].x = io.MousePos.y;
    }
    else {
        io.MouseDown[0] = false;
    }

    if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
        OldRect = TempRect;
        ProcessInfo.WindowWidth = TempRect.right;
        ProcessInfo.WindowHeight = TempRect.bottom;
        SetWindowPos(OverlayInfo.Hwnd, (HWND)0, TempPoint.x, TempPoint.y, ProcessInfo.WindowWidth, ProcessInfo.WindowHeight, SWP_NOREDRAW);
    }

    return 0;
}


void __fastcall CFunctions::SetupWindow(LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)) {
    OverlayInfo.WindowClass = {
        sizeof(WNDCLASSEX), 0, WndProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverlayInfo.Name, LoadIcon(nullptr, IDI_APPLICATION)
    };

    RegisterClassEx(&OverlayInfo.WindowClass);
    if (ProcessInfo.Hwnd) {
        static RECT TempRect = { NULL };
        static POINT TempPoint;
        GetClientRect(ProcessInfo.Hwnd, &TempRect);
        ClientToScreen(ProcessInfo.Hwnd, &TempPoint);
        TempRect.left = TempPoint.x;
        TempRect.top = TempPoint.y;
        ProcessInfo.WindowWidth = TempRect.right;
        ProcessInfo.WindowHeight = TempRect.bottom;
    }

    OverlayInfo.Hwnd = CreateWindowEx(NULL, OverlayInfo.Name, OverlayInfo.Name, WS_POPUP | WS_VISIBLE, ProcessInfo.WindowLeft, ProcessInfo.WindowTop, ProcessInfo.WindowWidth, ProcessInfo.WindowHeight, NULL, NULL, 0, NULL);
    DwmExtendFrameIntoClientArea(OverlayInfo.Hwnd, &Interface.Margin);
    SetWindowLong(OverlayInfo.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
    ::ShowWindow(OverlayInfo.Hwnd, SW_SHOW);
    ::UpdateWindow(OverlayInfo.Hwnd);
}

int __fastcall CFunctions::OverlaySettings(int ProcID)
{
    bool WindowFocus = false;
    while (WindowFocus == false) {
        DWORD ForegroundWindowProcessID;
        GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
        if (ProcID == ForegroundWindowProcessID) {
            ProcessInfo.ID = GetCurrentProcessId();
            ProcessInfo.Handle = GetCurrentProcess();
            ProcessInfo.Hwnd = GetForegroundWindow();

            RECT TempRect;
            GetWindowRect(ProcessInfo.Hwnd, &TempRect);
            ProcessInfo.WindowWidth = TempRect.right - TempRect.left;
            ProcessInfo.WindowHeight = TempRect.bottom - TempRect.top;
            ProcessInfo.WindowLeft = TempRect.left;
            ProcessInfo.WindowRight = TempRect.right;
            ProcessInfo.WindowTop = TempRect.top;
            ProcessInfo.WindowBottom = TempRect.bottom;

            char TempTitle[MAX_PATH];
            GetWindowText(ProcessInfo.Hwnd, TempTitle, sizeof(TempTitle));
            ProcessInfo.Title = TempTitle;

            char TempClassName[MAX_PATH];
            GetClassName(ProcessInfo.Hwnd, TempClassName, sizeof(TempClassName));
            ProcessInfo.ClassName = TempClassName;

            char TempPath[MAX_PATH];
            GetModuleFileNameEx(ProcessInfo.Handle, NULL, TempPath, sizeof(TempPath));
            ProcessInfo.Path = TempPath;

            WindowFocus = true;
        }
    }
    OverlayInfo.Name = RandomString(32).c_str();
}

void __fastcall CFunctions::ImGuiDesign(ImGuiStyle* style)
{
    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
}