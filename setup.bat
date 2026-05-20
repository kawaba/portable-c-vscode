@echo off
chcp 65001 > nul

powershell.exe -ExecutionPolicy Bypass -Command "Unblock-File -Path '%~dp0setup.ps1'"
powershell.exe -ExecutionPolicy Bypass -File "%~dp0setup.ps1"

echo.
echo --- Script finished ---
pause
