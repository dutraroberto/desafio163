@echo off
chcp 65001 > nul

echo Building Bitcoin Key Finder...
echo ===========================

if not exist "build" (
    mkdir build
)
cd build

cmake -G "Visual Studio 17 2022" -A x64 .. -DCMAKE_CXX_FLAGS="/O2 /openmp /arch:AVX2 /fp:fast /GL /Qpar"
if %ERRORLEVEL% neq 0 (
    echo.
    echo Build configuration failed!
    pause
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo.
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo The executable is located in: build\Release\Desafio163.exe
echo.
pause
