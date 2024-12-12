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

echo Criando diretório build...
if not exist build mkdir build
cd build

echo Configurando CMake...
cmake .. -A x64

if errorlevel 1 (
    echo Error: Falha ao executar o CMake!
    echo Certifique-se de que o CMake está instalado e no PATH do sistema
    pause
    exit /b 1
)

echo Compilando projeto...
cmake --build . --config Release

if errorlevel 1 (
    echo Erro na compilação!
    pause
    exit /b 1
)

echo.
echo Compilação concluída com sucesso!
echo O executável está em: build\Release\Desafio163.exe
pause
