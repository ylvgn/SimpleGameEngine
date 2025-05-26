@REM #change current directory to this file
%~d0
cd %~dp0

@REM dl: https://gnuwin32.sourceforge.net/downlinks/make.php
@REM default save to "C:\Program Files (x86)\GnuWin32\"
@REM GnuWin32 Official: https://gnuwin32.sourceforge.net/packages/make.htm
@REM set gmake_exe="C:\Program Files (x86)\GnuWin32\bin\make.exe"
@set gmake_exe=..\..\..\..\src\tools\gmake_win32\make.exe

%gmake_exe% -j %*

@if %ERRORLEVEL% NEQ 0 (
	@pause
)

@REM sge_editor_app will call this .bat, do not @pause default!!
@REM @pause
