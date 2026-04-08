@echo off
echo Baue VS Code Extension ...

cd /D C:\Uni\IKM\BALDUR\extensions\vscode
if errorlevel 1 (
    echo Fehler: extensions\vscode nicht gefunden.
    exit /b 1
)

make

echo Extension gebaut und gepackt

REM Kopiere die gepackte *.vsix nach __OUT
copy "*.vsix" "..\..\__OUT\" /Y
del "*.vsix"