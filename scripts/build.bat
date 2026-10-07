@echo off
setlocal EnableDelayedExpansion

echo ==========================================
echo        WaveForge - Build
echo ==========================================
echo.

set "WAVELINE_QT_ROOT="

echo Searching for Qt 6 MSVC 2022...
echo.

for %%D in (C D E F) do (
    for %%R in (
        "%%D:\Qt"
        "%%D:\Softwares\Qt"
    ) do (
        if exist "%%~R" (
            for /d %%V in ("%%~R\6.*") do (
                if exist "%%V\msvc2022_64\bin\qmake.exe" (
                    set "WAVELINE_QT_ROOT=%%V\msvc2022_64"
                    goto :qt_found
                )
            )
        )
    )
)

echo ERROR: Qt 6 MSVC 2022 was not found.
echo.
echo Please run setup.bat first.
echo.
pause
exit /b 1

:qt_found

echo Qt found:
echo !WAVELINE_QT_ROOT!
echo.

cd /d "%~dp0.."

if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

echo.
echo Configuring CMake...
echo.

cmake .. -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH="!WAVELINE_QT_ROOT!"

if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo Building WaveForge...
echo.

cmake --build . --config Debug

if errorlevel 1 (
    echo.
    echo ERROR: Build failed.
    pause
    exit /b 1
)

echo.
echo Deploying Qt runtime...
echo.

"!WAVELINE_QT_ROOT!\bin\windeployqt.exe" --debug --compiler-runtime "Debug\Waveline.exe"

if errorlevel 1 (
    echo.
    echo ERROR: Qt deployment failed.
    pause
    exit /b 1
)

echo.
echo ==========================================
echo       BUILD COMPLETED SUCCESSFULLY
echo ==========================================
echo.
echo Application:
echo %cd%\Debug\Waveline.exe
echo.

pause
endlocal