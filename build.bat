@echo off
echo Procurando Visual Studio...

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo Error: Visual Studio não encontrado!
    echo Por favor, instale o Visual Studio 2019 ou 2022 com suporte a C++
    pause
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set VS_PATH=%%i
)

if not exist "%VS_PATH%\Common7\Tools\VsDevCmd.bat" (
    echo Error: Visual Studio Command Prompt não encontrado!
    pause
    exit /b 1
)

echo Configurando ambiente do Visual Studio...
call "%VS_PATH%\Common7\Tools\VsDevCmd.bat"

REM Procurar o CMake do Visual Studio
set "CMAKE_PATH=%VS_PATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

if not exist "%CMAKE_PATH%" (
    echo Error: CMake não encontrado no Visual Studio!
    echo Por favor, certifique-se de que o componente "CMake Tools para Windows" está instalado
    pause
    exit /b 1
)

echo Criando diretório build...
if not exist build mkdir build
cd build

echo Configurando CMake...
"%CMAKE_PATH%" .. -A x64

echo Compilando projeto...
"%CMAKE_PATH%" --build . --config Release

if errorlevel 1 (
    echo Erro na compilação!
    pause
    exit /b 1
)

echo.
echo Compilação concluída com sucesso!
echo O executável está em: build\Release\Desafio163.exe
pause
