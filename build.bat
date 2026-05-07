
@REM this script no longer works, but keeping it incase I need it
@REM and for the work count
@REM cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH=C:\raylib
@REM cmake --build build

@echo off
setlocal

@REM Word Count
find /c /v "" *.c *.h


@REM BUILD
REM set RL=C:\raylib

REM if exist build rmdir /s /q build
REM cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH=%RL% -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE="/MT"
@REM cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH=%RL% -DCMAKE_BUILD_TYPE=Debug
REM cmake --build build

@REM echo.
@REM echo Run:
@REM build\create.exe

@REM copy /Y build\*.exe .


