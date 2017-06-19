@echo off

cd %~dp0
call :create_links Debug x64
call :create_links Release x64
call :create_links UnitTest x64
call :create_links Debug Win32
call :create_links Release Win32
call :create_links UnitTest Win32
pause
exit

:create_links
if not exist "%2\%1" (
mkdir "%2\%1"
echo + %2\%1
)
if not %1 == UnitTest (
mklink /d "%2\%1\data" "..\..\TextVenturer\data"
)
::mklink "%2\%1\glew32.dll" "..\..\glew\dlls\%2\glew32.dll"
goto :eof