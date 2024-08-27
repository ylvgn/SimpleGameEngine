@REM #change current directory to this file
@%~d0
@cd %~dp0

@REM glslc: https://github.com/google/shaderc/tree/main/glslc
@REM spirv-cross: https://github.com/KhronosGroup/SPIRV-Cross

@REM @pause
exit /B %ERRORLEVEL%

@REM #helper function ===================================
:HLSL2SPIRV
	glslc.exe -fshader-stage=vertex   -fentry-point=vs_main -o %1.spv_vs -x hlsl %1
	@echo off && call :ABORT_IF_ERROR
	glslc.exe -fshader-stage=fragment -fentry-point=ps_main -o %1.spv_ps -x hlsl %1
	@echo off && call :ABORT_IF_ERROR
goto :eof

:SPIRV2GLSL
	spirv-cross --version 330 --no-es --no-420pack-extension --output %1.spv_vs.vert %1.spv_vs
	@echo off && call :ABORT_IF_ERROR
	spirv-cross --version 330 --no-es --no-420pack-extension --output %1.spv_ps.frag %1.spv_ps
	@echo off && call :ABORT_IF_ERROR
goto :eof

:HLSL2GLSLC
	call :HLSL2SPIRV %1
	call :SPIRV2GLSL %1
goto :eof

:ABORT_IF_ERROR:
	if ERRORLEVEL 1 (
		goto :ERROR
	)
	@echo on
	goto :eof

:ERROR
    @echo on
    echo Failed!!!
	@pause
	exit /B %ERRORLEVEL%