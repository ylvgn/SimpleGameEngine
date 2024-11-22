@REM #change current directory to this file
%~d0
cd %~dp0

set triplets=x64-windows
set VCPKG_ROOT=externals\vcpkg
set vcpkg=%VCPKG_ROOT%\vcpkg

git clone https://github.com/microsoft/vcpkg.git externals\vcpkg

call %VCPKG_ROOT%\bootstrap-vcpkg.bat

%vcpkg% install vcpkg-tool-ninja:%triplets% --recurse
%vcpkg% install fmt:%triplets% --recurse
%vcpkg% install eastl:%triplets% --recurse
%vcpkg% install glew:%triplets%-static --recurse
%vcpkg% install nlohmann-json:%triplets% --recurse
%vcpkg% install libpng:%triplets% --recurse
@REM %vcpkg% install imgui:%triplets% --recurse
%vcpkg% install imgui[docking-experimental]:%triplets% --recurse
%vcpkg% install glad[gl-api-33]:%triplets% --recurse
@REM %vcpkg% install nuklear:%triplets% --recurse
%vcpkg% install spirv-cross:%triplets% --recurse
%vcpkg% install shaderc:%triplets% --recurse
%vcpkg% install glm:%triplets% --recurse
%vcpkg% install glfw3:%triplets% --recurse
@REM %vcpkg% install python3:%triplets% --recurse

@pause