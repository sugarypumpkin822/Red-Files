@echo off
echo Building Red Files Font Compiler (C++)

REM Check for GCC compiler (MinGW/MSYS2)
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: GCC C++ compiler not found
    echo Please install MinGW-w64 or MSYS2
    echo Download from: https://www.mingw-w64.org/downloads/
    echo.
    echo Alternative: Use the Python build script:
    echo   python build_cpp.py
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build
if not exist "build\bin" mkdir build\bin

echo Found GCC compiler
g++ --version
echo.

echo Compiling font_compiler.cpp...

REM Compile with GCC
g++ -std=c++17 -O2 -DNDEBUG -Wall ^
   -I"include" ^
   -I"integration" ^
   -o "build\bin\font_compiler.exe" ^
   tools\font_compiler_main.cpp ^
   -lkernel32 -luser32 -lgdi32 -lopengl32

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Compilation failed
    echo Check that all required files are present
    pause
    exit /b 1
)

echo.
echo [OK] C++ font compiler built successfully!
echo Location: build\bin\font_compiler.exe
echo.
echo You can now use: python compile.py --input font.json --output font.ttf
echo The script will automatically detect and use the C++ compiler
echo.
pause
