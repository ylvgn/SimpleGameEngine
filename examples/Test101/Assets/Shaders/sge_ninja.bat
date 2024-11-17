@REM #change current directory to this file
%~d0
cd %~dp0

@set sge_tools_bin=..\..\..\_bin\x64-windows\Debug
@set sge_shader_compiler=%sge_tools_bin%\sge_shader_compiler.exe

@REM Ninja Official: https://ninja-build.org/manual.html
@set ninja=%sge_tools_bin%\ninja.exe
set outdir=../../LocalTemp/Imported/Assets/Shaders
set build_ninja=build.ninja
%sge_shader_compiler% -genNinjaConfigure -file=%build_ninja% -out=%outdir% -I=.

%ninja% -f %build_ninja%

@if %ERRORLEVEL% NEQ 0 (
	@pause
)

@REM @pause