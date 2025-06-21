@echo off

FOR %%i IN ("%~dp0") DO SET VCPKG_SCRIPT_DIR=%%~fi

REM ---------------------------------------------------------------------------------------
REM If build.bat is run not from MSVC command prompt then check system and try to locate it.

IF "%VSCMD_ARG_TGT_ARCH%" == "" (
	SET MSVC_ARCH=%1

	FOR %%A IN ("x86" "x64") DO (
		IF "%MSVC_ARCH%" == %%A (
			SET ARCH_PARAM=1
		)
	)

	IF NOT DEFINED ARCH_PARAM (
		SET MSVC_ARCH=x86
	)

	IF "%MSVC_ARCH%" == "x64" (
		SET MSVC_CMD_ARCH=x86_amd64
	)

	FOR /f "usebackq delims=" %%i IN (`%VCPKG_SCRIPT_DIR%\vswhere.exe -version "[16.0,17.0)" -latest -property installationPath`) DO (
		IF EXIST "%%i\Common7\Tools\vsdevcmd.bat" (
			CALL "%%i\Common7\Tools\vsdevcmd.bat" %MSVC_CMD_ARCH%
		)
	)
	
	IF "%VSCMD_ARG_TGT_ARCH%" NEQ "%MSVC_ARCH%" (
		ECHO Wrong vs cmd prompt arch. Expected: %MSVC_ARCH%, current: "%VSCMD_ARG_TGT_ARCH%"
		EXIT /B
	)
) ELSE (
	SET MSVC_ARCH=%VSCMD_ARG_TGT_ARCH%
)
REM ---------------------------------------------------------------------------------------

SET CALLER_DIR=%CD%

SET VCPKG_PARENT_DIR=%PERIMETER_VCPKG_DIR%
FOR %%i IN ("%~dp0..") DO SET PROJECT_PATH=%%~fi

SET BUILD_CONF_TOKENS=VCPKG_REV
FOR %%A IN (%BUILD_CONF_TOKENS%) DO (
	FOR /F "tokens=2 delims==" %%B IN ('FINDSTR /b "%%A" %VCPKG_SCRIPT_DIR%\vcpkg_ver.conf') DO SET %%A=%%B
)

COPY %VCPKG_SCRIPT_DIR%\vcpkg.json %PROJECT_PATH%
CD %PROJECT_PATH%

IF NOT DEFINED VCPKG_PARENT_DIR (
	ECHO VCPKG_PARENT_DIR variable not set, check parent project dir
	FOR %%I IN (..) DO SET VCPKG_PARENT_DIR=%%~fI
)

SET VCPKGPATH=%VCPKG_PARENT_DIR%\vcpkg

ECHO VCPKG directory: %VCPKGPATH%
ECHO VCPKG rev.: %VCPKG_REV%
ECHO.

CD %VCPKG_PARENT_DIR%

IF EXIST %VCPKGPATH%\NUL (
	ECHO Found VCPKG folder
) ELSE (
	ECHO VCPKG Not found, checkout it
	git clone --progress https://github.com/microsoft/vcpkg.git %VCPKGPATH%
)

CD %VCPKGPATH%

git diff --quiet %VCPKG_REV%
IF %ERRORLEVEL% NEQ 0 (
	git reset --hard
	git clean -d -fx -f
	git pull --progress -v --no-rebase "origin"
	git checkout %VCPKG_REV% --quiet
)

IF NOT EXIST vcpkg.exe (
	CALL %VCPKGPATH%\bootstrap-vcpkg.bat -disableMetrics
)

CD %PROJECT_PATH%

RMDIR /S /Q build
MKDIR build
CD build

SET CMAKE_GENERATOR="Visual Studio 17 2022"
SET TOOLCHAIN=CMAKE_TOOLCHAIN_FILE=%VCPKGPATH%\scripts\buildsystems\vcpkg.cmake
SET VCPKGCUSTOMEDIR=_VCPKG_INSTALLED_DIR=%VCPKG_PARENT_DIR%\vcpkg_installed
IF "%MSVC_ARCH%" == "x86" (
	SET CMAKE_ARCH=Win32
	SET VCPKGTRIPLET=VCPKG_TARGET_TRIPLET="x86-windows"
) ELSE (
	SET CMAKE_ARCH=x64
	SET VCPKGTRIPLET=VCPKG_TARGET_TRIPLET="x64-windows"
)

cmake -G %CMAKE_GENERATOR% -A %CMAKE_ARCH% -D%VCPKGCUSTOMEDIR% -D%VCPKGTRIPLET% -DOPTION_DISABLE_STACKTRACE=ON -D%TOOLCHAIN% ..

CD %CALLER_DIR%
