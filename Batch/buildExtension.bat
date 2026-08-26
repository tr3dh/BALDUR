@echo off
echo Baue VS Code Extension ...

cd /D extensions\vscode
if errorlevel 1 (
    echo Fehler: extensions\vscode nicht gefunden.
    exit /b 1
)

make

echo Extension gebaut und gepackt

mkdir ..\..\__OUT\VSCodeExtension

REM Kopiere die gepackte *.vsix nach __OUT
copy "*.vsix" "..\..\__OUT\VSCodeExtension\" /Y

REM Dateinamen der *.vsix speicher
for %%f in ("*.vsix") do set VSIX_NAME=%%~nxf

del "*.vsix"

echo code --install-extension "%VSIX_NAME%" > "..\..\__OUT\VSCodeExtension\installExtension.bat"
echo code --uninstall-extension tr3dh.baldur > "..\..\__OUT\VSCodeExtension\uninstallExtension.bat"

powershell -NoProfile -Command "Compress-Archive -Path '..\..\__OUT\VSCodeExtension\*' -DestinationPath '..\..\__OUT\VSCodeExtension.zip' -Force"