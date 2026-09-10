@echo off
setlocal

set "PROJECT_DIR=%~dp0.."
set "BUILD_DIR=%PROJECT_DIR%\build"
set "SRC_DIR=%PROJECT_DIR%\Coding"

echo Creating build directories...
if not exist "%BUILD_DIR%\Debug" mkdir "%BUILD_DIR%\Debug"
if not exist "%BUILD_DIR%\Release" mkdir "%BUILD_DIR%\Release"

echo Building Debug configuration...
cd "%BUILD_DIR%\Debug"
cmake -DCMAKE_BUILD_TYPE=Debug "%SRC_DIR%"
cmake --build . --config Debug

echo Building Release configuration...
cd "%BUILD_DIR%\Release"
cmake -DCMAKE_BUILD_TYPE=Release "%SRC_DIR%"
cmake --build . --config Release

echo Build completed!
pause
