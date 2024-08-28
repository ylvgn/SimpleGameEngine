
call :HLSL2GLSLC test.hlsl
call :HLSL2GLSLC test.shader
call :HLSL2GLSLC terrain.shader
call :HLSL2GLSLC imgui.shader
call :HLSL2GLSLC line.shader

@pause
exit /B %ERRORLEVEL%

@REM #helper function ===================================
:HLSL2GLSLC
	@REM #change current directory to this file each call
	@%~d0
	@cd %~dp0
	call .\..\..\hlsl_to_glsl.bat Assets/Shaders/%1
goto :eof