@echo off
REM --------------------------------------------------
REM Windows Build Script for Ashwathama Chess Engine
REM --------------------------------------------------

echo Building Ashwathama Chess Engine...

REM Compile with g++ (C++17, optimizations enabled)
g++ -std=c++17 -O2 -o engine.exe src/main.cpp -Isrc

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful! Binary: engine.exe
    echo.
    echo To run in UCI mode: .\engine.exe --uci
    echo To run normally:    .\engine.exe
) else (
    echo.
    echo Build failed! Check errors above.
    exit /b 1
)
