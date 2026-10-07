@echo off
setlocal EnableDelayedExpansion

echo ==========================================
echo        WaveForge - Development Setup
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

echo Qt 6 MSVC 2022 was not found.
echo.
echo Please install Qt 6 MSVC 2022 64-bit.
echo.
goto :done

:qt_found

echo Qt 6 MSVC 2022 found.
echo.
echo Qt location:
echo !WAVELINE_QT_ROOT!
echo.

echo Saving Qt location for future CMake/Visual Studio sessions...

setx WAVELINE_QT_ROOT "!WAVELINE_QT_ROOT!" >nul

echo.
echo WAVELINE_QT_ROOT has been saved.
echo.
echo IMPORTANT:
echo If Visual Studio is currently open, close it completely
echo and reopen it so it can receive the new environment variable.
echo.

echo Setup check completed successfully.

:done
echo.
pause
endlocal