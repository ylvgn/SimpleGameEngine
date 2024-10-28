@REM #change current directory to this file
@%~d0
@cd %~dp0

@REM glslc: https://github.com/google/shaderc/tree/main/glslc
@REM spirv-cross: https://github.com/KhronosGroup/SPIRV-Cross

@setlocal

@REM set glslc=C:\VulkanSDK\1.3.290.0\Bin\glslc.exe
@set glslc=%VK_SDK_PATH%\Bin\glslc.exe

@REM set glslc=C:\VulkanSDK\1.3.290.0\Bin\spirv-cross.exe
@set spirv_cross=%VK_SDK_PATH%\Bin\spirv-cross.exe

@set GLSL_VERSION=330

@IF NOT EXIST %glslc% (
	@echo "'glslc' path not found error: please install Vulkan SDK https://vulkan.lunarg.com/sdk/home"
	@pause
	@exit /b -1
)

@IF NOT EXIST %spirv_cross% (
	@echo "'spirv-cross' path not found error: lease install Vulkan SDK https://vulkan.lunarg.com/sdk/home"
	@pause
	@exit /b -1
)

@call :HLSL2GLSLC %1

@REM @pause
@REM exit /B %ERRORLEVEL%

@REM #helper function ===================================
:HLSL2SPIRV
	%glslc% -fshader-stage=vertex   -fentry-point=vs_main -o %1.spv_vs -x hlsl %1
	@echo off && call :ABORT_IF_ERROR
	%glslc% -fshader-stage=fragment -fentry-point=ps_main -o %1.spv_ps -x hlsl %1
	@echo off && call :ABORT_IF_ERROR
@goto :eof

:SPIRV2GLSL
	%spirv_cross% --version %GLSL_VERSION% --no-es --no-420pack-extension --output %1.spv_vs.vert %1.spv_vs
	@echo off && call :ABORT_IF_ERROR
	%spirv_cross% --version %GLSL_VERSION% --no-es --no-420pack-extension --output %1.spv_ps.frag %1.spv_ps
	@echo off && call :ABORT_IF_ERROR
@goto :eof

:HLSL2GLSLC
	@call :HLSL2SPIRV %1
	@call :SPIRV2GLSL %1
@goto :eof

:ABORT_IF_ERROR:
	if ERRORLEVEL 1 (
		@goto :ERROR
	)
	@echo on
	@goto :eof

:ERROR
    @echo on
    @echo Failed!!
	@REM @pause
	@exit /B %ERRORLEVEL%