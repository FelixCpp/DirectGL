@echo off
pushd %~dp0\..\
Utilities\Premake\bin\premake5.exe vs2022 --file=Build.lua --os=windows
popd
PAUSE>NUL