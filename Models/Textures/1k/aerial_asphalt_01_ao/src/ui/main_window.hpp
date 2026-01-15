#pragma once

#include <filesystem>
#include <optional>

#include <windows.h>

namespace ui
{
class MainWindow
{
public:
    int Run(HINSTANCE instance, int showCmd);

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void OnPickOutput();
    void OnRebuild();
    void UpdateStatus(const wchar_t* text);

    HWND m_hwnd = nullptr;
    HWND m_status = nullptr;
    std::optional<std::filesystem::path> m_outputPath;
};
}
