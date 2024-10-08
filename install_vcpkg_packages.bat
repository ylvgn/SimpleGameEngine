@REM #change current directory to this file
%~d0
cd %~dp0

git clone https://github.com/microsoft/vcpkg.git externals\vcpkg

call externals\vcpkg\bootstrap-vcpkg.bat

externals\vcpkg\vcpkg install fmt:x64-windows --recurse
externals\vcpkg\vcpkg install eastl:x64-windows --recurse
externals\vcpkg\vcpkg install glew:x64-windows-static --recurse
externals\vcpkg\vcpkg install nlohmann-json:x64-windows --recurse
externals\vcpkg\vcpkg install libpng:x64-windows --recurse
externals\vcpkg\vcpkg install imgui:x64-windows --recurse
externals\vcpkg\vcpkg install glad[gl-api-33]:x64-windows --recurse
@REM externals\vcpkg\vcpkg install nuklear:x64-windows --recurse
externals\vcpkg\vcpkg install spirv-cross:x64-windows --recurse
externals\vcpkg\vcpkg install glm:x64-windows --recurse
externals\vcpkg\vcpkg install glfw3:x64-windows --recurse

@pause