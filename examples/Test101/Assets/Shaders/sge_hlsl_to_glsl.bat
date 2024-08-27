@REM #change current directory to this file
@%~d0
@cd %~dp0

call .\..\..\hlsl_to_glsl.bat Assets/Shaders/test.hlsl
call .\..\..\hlsl_to_glsl.bat Assets/Shaders/test.shader
call .\..\..\hlsl_to_glsl.bat Assets/Shaders/terrain.shader
call .\..\..\hlsl_to_glsl.bat Assets/Shaders/imgui.shader
call .\..\..\hlsl_to_glsl.bat Assets/Shaders/line.shader

@pause