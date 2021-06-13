@echo off

SET CALLER_DIR=%CD%

SET VCPKG_PARENT_DIR=%perimeter_VCPKG_PARENT_DIR%
FOR %%i IN ("%~dp0") DO SET _PATH_VSD=%%~fi
SET VCPKG_SCRIPT_DIR=%_PATH_VSD%

FOR %%i IN ("%~dp0..") DO SET _PATH_P=%%~fi
SET PROJECT_PATH=%_PATH_P%

SET BUILD_CONF_TOKENS=VCPKG_REV
FOR %%A IN (%BUILD_CONF_TOKENS%) DO (
	FOR /F "tokens=2 delims==" %%B IN ('FINDSTR /b "%%A" %VCPKG_SCRIPT_DIR%\vcpkg_ver.conf') DO SET %%A=%%B
)

COPY %VCPKG_SCRIPT_DIR%\vcpkg.json %PROJECT_PATH%

CD %PROJECT_PATH%

IF NOT DEFINED VCPKG_PARENT_DIR (
	ECHO VCPKG_PARENT_DIR variable not set, check parent project dir
	FOR %%i IN ("%~dp2..") DO SET _PARENT_PR_PATH=%%~fi
	SET VCPKG_PARENT_DIR=%_PARENT_PR_PATH%
)

SET VCPKGPATH=%_PARENT_PR_PATH%\vcpkg

ECHO VCPKG directory: %VCPKGPATH%
ECHO VCPKG rev.: %VCPKG_REV%
ECHO.

CD %VCPKG_PARENT_DIR%

IF EXIST %VCPKGPATH%\NUL (
	ECHO Found VCPKG folder
) ELSE (
	ECHO VCPKG Not found, checkout it
	git clone --progress https://github.com/microsoft/vcpkg.git
)

CD %VCPKGPATH%

git diff --quiet %VCPKG_REV%
IF %ERRORLEVEL% NEQ 0 (
	git reset --hard
	git clean -d -fx -f
	git pull --progress -v --no-rebase "origin"
	git checkout %VCPKG_REV%
)

IF NOT EXIST vcpkg.exe (
	CALL bootstrap-vcpkg.bat -disableMetrics
)

CD %PROJECT_PATH%

RMDIR /S /Q build
MKDIR build
CD build

SET TOOLCHAIN=CMAKE_TOOLCHAIN_FILE=%VCPKGPATH%\scripts\buildsystems\vcpkg.cmake
SET VCPKGCUSTOMEDIR=CUSTOME_VCPKG_INSTALL_DIR=%VCPKG_PARENT_DIR%\vcpkg_installed

cmake -G "Visual Studio 16 2019" -A Win32 -D%VCPKGCUSTOMEDIR% -DUSE_VCPKG=ON -D%TOOLCHAIN% ..

CD %CALLER_DIR%

