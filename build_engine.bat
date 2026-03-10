@echo off
setlocal enabledelayedexpansion

echo [Nets Build] Checking for C++ compilers...

:: Try MinGW (g++)
where g++ >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [Nets Build] Found g++. Compiling nets_engine.exe...
    g++ -std=c++17 -O3 nets_engine.cpp -o nets_engine.exe
    if %ERRORLEVEL% equ 0 (
        echo [Nets Build] Successfully built nets_engine.exe with g++.
        goto :success
    )
)

:: Try MSVC (cl)
where cl >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [Nets Build] Found MSVC (cl). Compiling nets_engine.exe...
    cl /EHsc /O2 /std:c++17 nets_engine.cpp /Fe:nets_engine.exe
    if %ERRORLEVEL% equ 0 (
        echo [Nets Build] Successfully built nets_engine.exe with MSVC.
        :: Clean up MSVC artifacts
        del nets_engine.obj
        goto :success
    )
)

echo [Nets Build] ERROR: No compatible C++ compiler found (g++ or cl).
echo Please install MinGW-w64 (via MSYS2) or Visual Studio Build Tools.
pause
exit /b 1

:success
echo [Nets Build] Build complete! You can now run the Java game.
pause
exit /b 0
