@echo off
pushd %~dp0\..\

echo Cleaning up all *.slnx files ...
for /R %%i in (*.slnx) do (
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

if exist build (
	echo Deleting the build directory
	rmdir /S /Q build
)

echo Done cleaning Visual Studio 2026 files.
popd
PAUSE>NUL