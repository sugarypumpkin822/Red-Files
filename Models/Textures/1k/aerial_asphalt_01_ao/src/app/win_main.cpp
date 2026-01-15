#include <windows.h>

#include "ui/main_window.hpp"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCmd)
{
    ui::MainWindow app;
    return app.Run(instance, showCmd);
}
