@REM #change current directory to this file
@%~d0
@cd %~dp0

@rem https://github.com/google/shaderc/tree/main/glslc

call :HLSL2SPIRV test.shader
call :HLSL2SPIRV test.hlsl
call :HLSL2SPIRV terrain.shader
call :HLSL2SPIRV imgui.shader
call :HLSL2SPIRV line.shader

@pause
exit /B %ERRORLEVEL%

@REM #helper function
:HLSL2SPIRV
	glslc.exe -fshader-stage=vertex   -fentry-point=vs_main -o %1.spv_vs -x hlsl %1
	glslc.exe -fshader-stage=fragment -fentry-point=ps_main -o %1.spv_ps -x hlsl %1
goto :eof