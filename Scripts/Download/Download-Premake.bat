@ECHO OFF
SETLOCAL enabledelayedexpansion

REM ====================================================
REM  Download & Setup Premake5
REM ====================================================

ECHO.
ECHO ==============================
ECHO   Premake5 Download Script
ECHO ==============================
ECHO.

REM Zielordner
set "TARGET_DIR=premake5"
set "PREMAKE_REPOSITORY=https://github.com/premake/premake-core.git"

REM Check if there's Git installed
WHERE git >nul 2>&1
IF errorlevel 1 (
    echo [FEHLER] Couldn't find Git. Please install Git and ensure it's in your PATH.
    pause
    exit /b 1
)

REM ===========================================================================
REM Clone the Premake5 repository

IF NOT EXIST "%TARGET_DIR%" (
    REM Repository needs to be cloned
    ECHO Cloning Premake5 repository using %PREMAKE_REPOSITORY%
    git clone --depth=1 %PREMAKE_REPOSITORY% "%TARGET_DIR%"

    if errorlevel 1 (
        ECHO [FEHLER] Konnte Premake5 nicht klonen.
        PAUSE
        EXIT /B 1
    )
) ELSE (
    REM Repository does already exist
    ECHO Premake5 repository already exists at %TARGET_DIR%.
    ECHO Do you want to re-clone it? This will overwrite any local changes.
    CHOICE /M "Re-clone Premake5 repository?"
    IF errorlevel 2 (
        ECHO Skipping clone.
    ) ELSE (
        ECHO Deleting existing repository...
        RMDIR /S /Q "%TARGET_DIR%"
        ECHO Cloning Premake5 repository using %PREMAKE_REPOSITORY%
        git clone --depth=1 %PREMAKE_REPOSITORY% "%TARGET_DIR%"

        if errorlevel 1 (
            ECHO [ERROR] Failed to clone Premake5 repository.
            PAUSE
            EXIT /B 1
        )
    )
)

REM ===========================================================================

REM Make a copy of Bootstrap_Fix.bat inside the TARGET_DIR folder
COPY /Y "Bootstrap_Fix.bat" "%TARGET_DIR%\Bootstrap_Customized.bat"
IF errorlevel 1 (
    ECHO [ERROR] Failed to copy Bootstrap_Fix.bat to %TARGET_DIR%\Bootstrap_Customized.bat
    PAUSE
    EXIT /B 1
)

REM Make a copy of Bootstrap_Fix.mak inside the TARGET_DIR folder
COPY /Y "Bootstrap_Fix.mak" "%TARGET_DIR%\Bootstrap_Customized.mak"
IF errorlevel 1 (
    ECHO [ERROR] Failed to copy Bootstrap_Fix.mak to %TARGET_DIR%\Bootstrap_Customized.mak
    PAUSE
    EXIT /B 1
)

(CD "%TARGET_DIR%" && CALL Bootstrap_Customized.bat vs2026 && CD ..)
IF errorlevel 1 (
    ECHO [ERROR] Failed to build Premake5
    PAUSE
    EXIT /B 1
)

REM ===========================================================================
REM Copy the executable to a known location

SET "BUILT_FILE=%TARGET_DIR%\build\bootstrap\premake_bootstrap.exe"
SET "DESTINATION_FILE=..\Utilities\Premake\bin\premake5.exe"

IF NOT EXIST %BUILT_FILE% (
    ECHO [WARNING] There's no file at %BUILT_FILE%. Please build Premake5 manually.
    PAUSE
    exit /B 1
)

ECHO [INFO] Copying built Premake5 to a known location...
COPY /Y %BUILT_FILE% %DESTINATION_FILE%

IF errorlevel 1 (
    ECHO [ERROR] Failed to copy Premake5 executable from %BUILT_FILE% to %DESTINATION_FILE%.
    PAUSE
    EXIT /B 1
)

REM ===========================================================================

ECHO Premake5 has been successfully downloaded and built.
PAUSE
ENDLOCAL