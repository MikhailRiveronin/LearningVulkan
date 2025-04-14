#include "Application.h"

#include "InputHandler.h"
#include "Logger.h"

#include <windows.h>
#include <windowsx.h>
#include <stdexcept>

LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void Application::run(SampleBase* sample)
{
    auto hInstance = GetModuleHandleA(nullptr);

    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = 0;
    wc.lpfnWndProc = windowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "WindowClass";
    wc.hIconSm = nullptr;

    if (!RegisterClassExA(&wc)) {
        throw std::runtime_error("");
    }

    ATOM result = RegisterClassExA(&wc);
    DWORD error = GetLastError();

    RECT rect = { 0, 0, (LONG)sample->width, (LONG)sample->height };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exStyle = WS_EX_APPWINDOW;

    BOOL result2 = AdjustWindowRect(&rect, style, FALSE);
    error = GetLastError();

    auto hWnd = CreateWindowExA(
        exStyle, "WindowClass", sample->name.c_str(), style,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hInstance, sample);

    if (!hWnd) {
        LOG_ERROR("Failed to create window");
        throw std::runtime_error("Failed to create window");
    }

    ShowWindow(hWnd, SW_SHOWNORMAL);
    sample->onInit(hInstance, hWnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    sample->onDestroy();
}

LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static SampleBase* sample;

    switch (message) {
    case WM_CREATE: {
        auto createStruct = (LPCREATESTRUCT)lParam;
        sample = (SampleBase*)createStruct->lpCreateParams;
        return 0;
    }

    case WM_PAINT:
        if (sample) {
            sample->onUpdate();
        }
        return 0;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        InputHandler::processKeystroke((u16)wParam, message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
        return 0;

    case WM_LBUTTONDOWN:
        if (wParam & MK_LBUTTON) {
            InputHandler::processLeftMouseButton(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return 0;

    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON) {
            InputHandler::processMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
    return 0;

    default:
    break;
    }

    return DefWindowProcA(hWnd, message, wParam, lParam);
}
