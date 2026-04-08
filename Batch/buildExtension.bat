@echo off
echo Baue VS Code Extension ...

cd /D C:\Uni\IKM\BALDUR\extensions\vscode
if errorlevel 1 (
    echo Fehler: extensions\vscode nicht gefunden.
    exit /b 1
)

make

echo Extension gebaut und gepackt

mkdir ..\..\__OUT\extension

REM Kopiere die gepackte *.vsix nach __OUT
copy "*.vsix" "..\..\__OUT\extension\" /Y

REM Dateinamen der *.vsix speicher
for %%f in ("*.vsix") do set VSIX_NAME=%%~nxf

del "*.vsix"

echo code --install-extension "%VSIX_NAME%" > "..\..\__OUT\extension\installExtension.bat"
echo code --uninstall-extension tr3dh.baldur > "..\..\__OUT\extension\uninstallExtension.bat"