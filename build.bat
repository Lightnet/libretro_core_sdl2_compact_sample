@echo off
setlocal

:: Set project paths
set PROJECT_DIR=%CD%
set BUILD_DIR=%PROJECT_DIR%\build
set RETROARCH_DIR=D:\dev\RetroArch-Win64
set CORE_NAME=my_libretro_core.dll

:: Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

:: Change to build directory
cd "%BUILD_DIR%"

:: Run CMake to generate VS2022 solution for Debug
echo Configuring CMake for Debug...
@REM cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Debug ..

:: Check if CMake configuration succeeded
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b %ERRORLEVEL%
)

:: Build the project in Debug mode
echo Building Debug configuration...
cmake --build . --config Debug

:: Check if build succeeded
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

:: Copy the DLL and font to RetroArch cores directory
echo Copying %CORE_NAME% and fonts to RetroArch...
copy "%BUILD_DIR%\Debug\%CORE_NAME%" "%RETROARCH_DIR%\cores\"
if %ERRORLEVEL% neq 0 (
    echo Failed to copy DLL to RetroArch!
    exit /b %ERRORLEVEL%
)

:: Copy fonts directory
if not exist "%RETROARCH_DIR%\cores\fonts" mkdir "%RETROARCH_DIR%\cores\fonts"
copy "%PROJECT_DIR%\fonts\arial.ttf" "%RETROARCH_DIR%\cores\fonts\"
if %ERRORLEVEL% neq 0 (
    echo Failed to copy font to RetroArch!
    exit /b %ERRORLEVEL%
)

echo Build and setup completed successfully!
echo To debug, open %BUILD_DIR%\my_libretro_core.sln in VS2022 and set up debugging as described.

endlocal