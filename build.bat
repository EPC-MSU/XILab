@if 'x%MERCURIAL%'=='x' set MERCURIAL=hg
set BASEDIR=%CD%
@cmd /C exit 0

set FIRSTPARAM="%1"
@if '%2' == 'add_service_build' set FIRSTPARAM="%2"
@if %FIRSTPARAM% == "cleandist" call :CLEAN && exit /B 0

:: skip checkout magic for pipeline builds
@if not "%BUILDOS%" == "" goto END_REFRESH

:: skip checkout magic for git repository builds
@if '%1' == 'git' goto END_REFRESH
@if '%2' == 'git' goto END_REFRESH

:: Prefer XIMC_REVISION
@if NOT 'x%XIMC_REVISION%' == 'x' (@set MERCURIAL_SUFFIX=-r %XIMC_REVISION%
) else if NOT 'x%MERCURIAL_REVISION%' == 'x' (@set MERCURIAL_SUFFIX=-r %MERCURIAL_REVISION%)
@echo Using checkout command: hg update -c %MERCURIAL_SUFFIX%
:: pull again because working copy often contain only one branch
%MERCURIAL% pull
@if not %errorlevel% == 0 goto FAIL
:: update -c fails on uncommited changes
:: XXX skipped it because xilab build is in the dirty directory
%MERCURIAL% update  %MERCURIAL_SUFFIX%
@if not %errorlevel% == 0 goto FAIL

@if 'x%XIMC_REVISION%' == 'x' goto END_REFRESH
@if 'x%MERCURIAL_REVISION%' == 'x' goto END_REFRESH
@echo Script could be refreshed script
@if '%1' == 'refresh' goto END_REFRESH
@echo Refresing script
call build.bat refresh
:: our job here is done, exiting
@goto :eof
:END_REFRESH

:: -------------------------------------
:: ---------- entry point --------------

:: Voodoo magic, do not touch.
for /f "eol=# delims== tokens=1,2" %%i in ( %~dp0\VERSIONS ) do (
    set %%i=%%j
)

set DISTDIR=dist_dir
set DEPDIR=C:\dependency_files
set QWTDIR=C:\Qwt\msvc2013\qwt-%QWT_VER%
set QTBASEDIR=C:\Qt\msvc2013
set QMAKESPEC=win32-msvc2013

echo "CERTNAME=%CERTNAME%"
echo "XIMC_RELEASE_TYPE=%XIMC_RELEASE_TYPE%"

call :LIB
@if not %errorlevel% == 0 goto FAIL
call :CLEAN
@if not %errorlevel% == 0 goto FAIL
call :APP
@if not %errorlevel% == 0 goto FAIL

@if "%NODE_NAME%" == "" goto SUCCESS

cd "%DISTDIR%"
7z a %BASEDIR%\xilab-win32.7z win32\
@if not %errorlevel% == 0 goto FAIL
7z a %BASEDIR%\xilab-win64.7z win64\
@if not %errorlevel% == 0 goto FAIL

:: it is an exit
:SUCCESS
@echo SUCCESS
@goto :eof

:FAIL
@echo FAIL
exit /B 1

:: ----------------------------
:: ---------- clean -----------
:CLEAN
::%MERCURIAL% purge -a
::@if not %errorlevel% == 0 goto FAIL
::@if exist %DISTDIR% rmdir /S /Q %DISTDIR%
::@if not %errorlevel% == 0 goto FAIL
goto :eof

:: ----------------------------
:: ---------- clean -----------
:LIB
7z x -y ximc-*.tar.gz
@if not %errorlevel% == 0 goto FAIL
7z x -y ximc-*.tar
@if not %errorlevel% == 0 goto FAIL
rmdir /S /Q xiresource
@if not %errorlevel% == 0 goto FAIL
git clone "https://gitlab.ximc.ru/ximc/xiresource"
@if not %errorlevel% == 0 goto FAIL

for %%G in (win32,win64) do (
powershell -Command "New-Item -ItemType File -Path ..\libximc-win\ximc\%%G\ximc.h -Force"
@if not %errorlevel% == 0 goto FAIL
powershell -Command "cp ximc*\ximc*\ximc.h ..\libximc-win\ximc\%%G\ximc.h"
@if not %errorlevel% == 0 goto FAIL
powershell -Command "cp ximc*\ximc*\%%G\libximc.lib ..\libximc-win\ximc\%%G\libximc.lib"
@if not %errorlevel% == 0 goto FAIL
powershell -Command "cp ximc*\ximc*\%%G\libximc.dll ..\libximc-win\ximc\%%G\libximc.dll"
@if not %errorlevel% == 0 goto FAIL
powershell -Command "cp ximc*\ximc*\%%G\xiwrapper.dll ..\libximc-win\ximc\%%G\xiwrapper.dll"
@if not %errorlevel% == 0 goto FAIL
powershell -Command "cp ximc*\ximc*\%%G\bindy.dll ..\libximc-win\ximc\%%G\bindy.dll"
@if not %errorlevel% == 0 goto FAIL
powershell -Command "cp ximc*\ximc*\%%G\bindy.lib ..\libximc-win\ximc\%%G\bindy.lib"
@if not %errorlevel% == 0 goto FAIL
)
goto :eof

:: ----------------------------------------
:: ---------- build entry point -----------
:APP
cd %BASEDIR%
@if not %errorlevel% == 0 goto FAIL
@if %FIRSTPARAM% == "add_service_build" call :XILAB win32 servicemode Win32
@if not %errorlevel% == 0 goto FAIL
call :XILAB win32 usermode Win32
@if not %errorlevel% == 0 goto FAIL
@if %FIRSTPARAM% == "add_service_build" call :XILAB win64 servicemode x64
@if not %errorlevel% == 0 goto FAIL
call :XILAB win64 usermode x64
@if not %errorlevel% == 0 goto FAIL
call :NSIS
@if not %errorlevel% == 0 goto FAIL
goto :eof


:: --------------------------------------
:: ---------- xilab wrapper -----------
:XILAB
@set DISTARCH=%DISTDIR%\%1
@set ARCH=%3
@echo Building xilab %2 for %ARCH%...
@set BINDIR="release - %2"
@set SOLCONF="release - %2|%3"

::%MERCURIAL% purge -a
::@if not %errorlevel% == 0 goto FAIL
@if not exist %DISTARCH% mkdir %DISTARCH%
@if not %errorlevel% == 0 goto FAIL

@if "%1" == "win32" set QTDIR=%QTBASEDIR%\%QT_VER%
@if not %errorlevel% == 0 goto FAIL
@if "%1" == "win32" call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
@if not %errorlevel% == 0 goto FAIL

@if "%1" == "win64" set QTDIR=%QTBASEDIR%\%QT_VER%_x64
@if not %errorlevel% == 0 goto FAIL
@if "%1" == "win64" call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64
@if not %errorlevel% == 0 goto FAIL

devenv /nologo /build %SOLCONF% XILab.sln 
@if not %errorlevel% == 0 goto FAIL
move /Y %ARCH%\%BINDIR%\"XILab "[%BINDIR%].exe a.exe
@if not %errorlevel% == 0 goto FAIL
:: Sign binaries only in release build and if CERTNAME is given
@if 'x%XIMC_RELEASE_TYPE%' == 'xRELEASE' if 'x%CERTNAME%' NEQ 'x' SignTool.exe sign /v /n %CERTNAME% a.exe

@if not %errorlevel% == 0 goto FAIL
move /Y a.exe %ARCH%\%BINDIR%\"XILab "[%BINDIR%].exe
@if not %errorlevel% == 0 goto FAIL
copy %ARCH%\%BINDIR%\"XILab ["%BINDIR%"].exe" %DISTARCH%
@if not %errorlevel% == 0 goto FAIL

copy %QTDIR%\bin\QtCore4.dll %DISTARCH%
@if not %errorlevel% == 0 goto FAIL
copy %QTDIR%\bin\QtGui4.dll %DISTARCH%
@if not %errorlevel% == 0 goto FAIL
copy %QTDIR%\bin\QtScript4.dll %DISTARCH%
@if not %errorlevel% == 0 goto FAIL
copy %QTDIR%\bin\QtSvg4.dll %DISTARCH%
@if not %errorlevel% == 0 goto FAIL
copy %QTDIR%\bin\QtNetwork4.dll %DISTARCH%
@if not %errorlevel% == 0 goto FAIL
copy %QTDIR%\bin\QtXml4.dll %DISTARCH%
@if not %errorlevel% == 0 goto FAIL

copy qt.conf %DISTARCH%\qt.conf

echo f | xcopy /Y /I %QTDIR%\plugins\imageformats\qgif4.dll %DISTARCH%\imageformats\qgif4.dll
@if not %errorlevel% == 0 goto FAIL
echo f | xcopy /Y /I %QTDIR%\plugins\imageformats\qsvg4.dll %DISTARCH%\imageformats\qsvg4.dll
@if not %errorlevel% == 0 goto FAIL
echo f | xcopy /Y /I %QTDIR%\plugins\iconengines\qsvgicon4.dll %DISTARCH%\iconengines\qsvgicon4.dll
@if not %errorlevel% == 0 goto FAIL
xcopy /Y /I driver %DISTARCH%\driver
@if not %errorlevel% == 0 goto FAIL
xcopy /Y /I /S xiresource\scripts %DISTARCH%\scripts
@if not %errorlevel% == 0 goto FAIL
xcopy /Y /I /S xiresource\profiles %DISTARCH%\profiles
@if not %errorlevel% == 0 goto FAIL
xcopy /Y /I /S xiresource\schemes %DISTARCH%\profiles
@if not %errorlevel% == 0 goto FAIL
xcopy /Y /I xilabdefault.cfg %DISTARCH%\
@if not %errorlevel% == 0 goto FAIL

:: Needed for the standalone version
powershell -Command "cp ximc*\ximc*\%1\keyfile.sqlite .\%DISTARCH%\default_keyfile.sqlite "
@if not %errorlevel% == 0 goto FAIL
:: Also copy keyfile to the base dir so that NSIS can find it
powershell -Command "cp .\%DISTARCH%\default_keyfile.sqlite .\ "
@if not %errorlevel% == 0 goto FAIL

xcopy /Y /I %DEPDIR%\%1\msvcp120.dll %DISTARCH%\
@if not %errorlevel% == 0 goto FAIL
xcopy /Y /I %DEPDIR%\%1\msvcr120.dll %DISTARCH%\
@if not %errorlevel% == 0 goto FAIL

powershell -Command "cp ximc*\ximc*\%1\libximc.dll .\%DISTARCH%\ "
@if not %errorlevel% == 0 goto FAIL
powershell -Command "cp ximc*\ximc*\%1\xiwrapper.dll .\%DISTARCH%\ "
@if not %errorlevel% == 0 goto FAIL
powershell -Command "cp ximc*\ximc*\%1\bindy.dll .\%DISTARCH%\ "
@if not %errorlevel% == 0 goto FAIL
@if "%1" == "win32" set QWTBINDIR=%QWTDIR%\lib32
@if "%1" == "win64" set QWTBINDIR=%QWTDIR%\lib64
copy %QWTBINDIR%\qwt.dll %DISTARCH%\qwt.dll
@if not %errorlevel% == 0 goto FAIL

@echo Building for %ARCH% completed
@goto :eof

:: --------------------------------------
:: ---------- NSIS wrapper --------------
:NSIS
makensis XILab-setup-UTF8.nsi
@if not %errorlevel% == 0 goto FAIL
ren XILab-*.exe xilab-*.exe
@if not %errorlevel% == 0 goto FAIL
:: Sign binaries only in release build
@if 'x%XIMC_RELEASE_TYPE%' == 'xRELEASE' SignTool.exe sign /v /n %CERTNAME% xilab-*.exe
@if not %errorlevel% == 0 goto FAIL
@goto :eof
