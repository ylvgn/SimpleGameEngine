@REM #change current directory to this file
@%~d0
@cd %~dp0

setlocal
set sgeShaderCompiler=..\..\build\SimpleGameEngine-x64-windows\src\render\shader_compiler\Debug\sge_shader_compiler.exe
set sge_tools_bin=..\..\externals\tools

@IF EXIST %sge_tools_bin% (
	set PATH="%PATH%;%sge_tools_bin%"
	@REM @echo %PATH%
	
	set glslc="%sge_tools_bin%\glslc.exe"
	set ninja="%sge_tools_bin%\ninja.exe"
	set spirv_cross="%sge_tools_bin%\spirv_cross.exe"
) else (
	exit /b -1
)

@echo glslc=%glslc%
@echo ninja=%ninja%
@echo spirv_cross=%spirv_cross%

@rem TODO
call Assets\Shaders\make.bat
@rem @pause