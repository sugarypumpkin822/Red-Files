#include "ui/file_dialog.hpp"

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

namespace ui
{
std::optional<std::filesystem::path> ShowSaveJpgDialog()
{
#ifdef _WIN32
    wchar_t fileName[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"JPEG Image (*.jpg)\0*.jpg;*.jpeg\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = L"jpg";

    if (!GetSaveFileNameW(&ofn))
        return std::nullopt;

    return std::filesystem::path(fileName);
#else
    return std::nullopt;
#endif
}
}
