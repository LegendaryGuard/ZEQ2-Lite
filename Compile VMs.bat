@echo off
set oldSystemPath=%PATH%
set compilerPath=%~dp0Tools/MinGW-w64/bin
set PATH=%compilerPath%;%PATH%
%compilerPath%/make release COMPILE_PLATFORM=mingw32 BUILD_CLIENT=0 BUILD_SERVER=0 BUILD_GAME_SO=1 BUILD_GAME_QVM=1 install
pause
set PATH=%oldSystemPath%