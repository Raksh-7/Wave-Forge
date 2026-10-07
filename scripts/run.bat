@echo off
setlocal

echo ==========================================
echo        WaveForge - Run
echo ==========================================
echo.

cd /d "%~dp0.."

if not exist "build\Debug\Waveline.exe" (
    echo ERROR: Waveline.exe was not found.
    echo.
    echo Please run build.bat first.
    echo.
    pause
    exit /b 1
)

echo Starting Waveline...
echo.

start "" "build\Debug\Waveline.exe"

endlocal