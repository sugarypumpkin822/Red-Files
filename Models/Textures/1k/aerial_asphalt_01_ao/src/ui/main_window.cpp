#include "ui/main_window.hpp"
#include "ui/file_dialog.hpp"
#include "core/rebuild.hpp"

#include <string>

namespace
{
constexpr int IDC_PICK = 1001;
constexpr int IDC_REBUILD = 1002;
}

namespace ui
{
int MainWindow::Run(HINSTANCE instance, int showCmd)
{
    const wchar_t* className = L"TextureRebuildWindow";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = &MainWindow::WndProc;
    wc.hInstance = instance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    m_hwnd = CreateWindowExW(
        0, className, L"Texture JPG Rebuild",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 520, 180,
        nullptr, nullptr, instance, this);

    if (!m_hwnd)
        return 1;

    ShowWindow(m_hwnd, showCmd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* self = nullptr;
    if (msg == WM_NCCREATE)
    {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = reinterpret_cast<MainWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->m_hwnd = hwnd;
    }
    else
    {
        self = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (self)
        return self->HandleMessage(hwnd, msg, wParam, lParam);

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        CreateWindowW(L"BUTTON", L"Pick output...", WS_VISIBLE | WS_CHILD, 16, 16, 140, 28, hwnd, (HMENU)IDC_PICK, nullptr, nullptr);
        CreateWindowW(L"BUTTON", L"Rebuild JPG", WS_VISIBLE | WS_CHILD, 170, 16, 140, 28, hwnd, (HMENU)IDC_REBUILD, nullptr, nullptr);

        m_status = CreateWindowW(L"STATIC", L"No output selected.", WS_VISIBLE | WS_CHILD, 16, 60, 470, 60, hwnd, nullptr, nullptr, nullptr);
        return 0;
    }
    case WM_COMMAND:
    {
        const int id = LOWORD(wParam);
        if (id == IDC_PICK)
            OnPickOutput();
        else if (id == IDC_REBUILD)
            OnRebuild();
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

void MainWindow::OnPickOutput()
{
    m_outputPath = ShowSaveJpgDialog();
    if (m_outputPath)
    {
        std::wstring s = L"Output: " + m_outputPath->wstring();
        UpdateStatus(s.c_str());
    }
}

void MainWindow::OnRebuild()
{
    if (!m_outputPath)
    {
        MessageBoxW(m_hwnd, L"Pick an output path first.", L"Texture Rebuild", MB_OK | MB_ICONWARNING);
        return;
    }

    const bool ok = core::RebuildToFile(*m_outputPath);
    if (ok)
        MessageBoxW(m_hwnd, L"JPG rebuilt successfully.", L"Texture Rebuild", MB_OK | MB_ICONINFORMATION);
    else
        MessageBoxW(m_hwnd, L"Failed to rebuild JPG.", L"Texture Rebuild", MB_OK | MB_ICONERROR);
}

void MainWindow::UpdateStatus(const wchar_t* text)
{
    if (m_status)
        SetWindowTextW(m_status, text);
}
}
