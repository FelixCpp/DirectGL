@echo off
pushd %~dp0\..\
Premake\bin\premake5.exe vs2022 --file=Build.lua --os=windows
popd
PAUSE>NUL