@REM #change current directory to this file
%~d0
cd %~dp0

set triplets=x64-windows

git clone https://github.com/microsoft/vcpkg.git externals\vcpkg

call externals\vcpkg\bootstrap-vcpkg.bat

externals\vcpkg\vcpkg install vcpkg-tool-ninja:%triplets% --recurse
externals\vcpkg\vcpkg install fmt:%triplets% --recurse
externals\vcpkg\vcpkg install eastl:%triplets% --recurse
externals\vcpkg\vcpkg install glew:%triplets%-static --recurse
externals\vcpkg\vcpkg install nlohmann-json:%triplets% --recurse
externals\vcpkg\vcpkg install libpng:%triplets% --recurse
externals\vcpkg\vcpkg install imgui:%triplets% --recurse
externals\vcpkg\vcpkg install glad[gl-api-33]:%triplets% --recurse
@REM externals\vcpkg\vcpkg install nuklear:%triplets% --recurse
externals\vcpkg\vcpkg install spirv-cross:%triplets% --recurse
externals\vcpkg\vcpkg install shaderc:%triplets% --recurse
externals\vcpkg\vcpkg install glm:%triplets% --recurse
externals\vcpkg\vcpkg install glfw3:%triplets% --recurse

@pause