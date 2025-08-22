@echo off
setlocal EnableExtensions
pushd "%~dp0"

set "LIST=to_delete.txt"
if not exist "%LIST%" (
  echo [ERROR] %LIST% not found in "%CD%".
  echo Run build_to_delete.py first.
  goto :eof
)

for /f "usebackq delims=" %%F in ("%LIST%") do (
  if exist "%%~F" (
    del /f /q "%%~F" >nul 2>&1
    echo [DELETED] %%~F
  ) else (
    echo [MISSING] %%~F
  )
)

echo Done.
popd
endlocal
