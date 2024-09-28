@REM #change current directory to this file
@%~d0
@cd %~dp0

set comp=%VK_SDK_PATH%\Bin\glslc.exe
@REM set comp=C:\VulkanSDK\1.3.290.0\Bin\glslc.exe

set stage=%1
set entry=%2
set outFile=%3
set inputFile=%4

%comp% -fshader-stage=%stage% -fentry-point=%entry% -o %outFile% -x hlsl %inputFile%

@rem pause