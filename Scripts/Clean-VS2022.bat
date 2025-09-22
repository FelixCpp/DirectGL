@echo off
pushd %~dp0\..\

echo Cleaning up all *.sln files ...
for /R %%i in (*.sln) do (
	echo Cleaning %%i ...
	del /F /Q "%%i"
)

echo Cleaning up all *.vcxproj files ...
for /R %%i in (*.vcxproj) do (
	echo Cleaning %%i ...
	del /F /Q "%%i"
)

echo Cleaning up all *.vcxproj.filters files ...
for /R %%i in (*.vcxproj.filters) do (
	echo Cleaning %%i ...
	del /F /Q "%%i"
)

echo Cleaning up all *.user files ...
for /R %%i in (*.user) do (
	echo Cleaning %%i ...
	del /F /Q "%%i"
)

echo Done cleaning Visual Studio 2022 files.
popd
PAUSE>NUL