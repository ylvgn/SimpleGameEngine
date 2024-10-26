@REM #change current directory to this file
%~d0
cd %~dp0

@REM dl: https://gnuwin32.sourceforge.net/downlinks/make.php
@REM GnuWin32 Official: https://gnuwin32.sourceforge.net/packages/make.htm
set gmake_exe="C:\Program Files (x86)\GnuWin32\bin\make.exe"

%gmake_exe% -j %*

if %ERRORLEVEL% NEQ 0 (
	@pause
)

@REM @pause
