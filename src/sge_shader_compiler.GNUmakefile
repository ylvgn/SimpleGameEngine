.PHONY: all FORCE start
# "all" must be first target
all:

#----------------------

outdir ?= ../../LocalTemp/Imported/Assets/Shaders
input_files ?= test.shader
CMAKE_BUILD_TYPE ?= Debug

ifeq ($(OS), Windows_NT) # is Windows_NT on XP, 2000, 7, Vista, 10, 11...
    SGE_PLATFORM_OS := Windows
	VCPKG_TARGET_TRIPLET := x64-windows
	CMAKE_RUNTIME_OUTPUT_DIRECTORY := $(realpath ../../../../examples/_bin)/$(VCPKG_TARGET_TRIPLET)/$(CMAKE_BUILD_TYPE)
else ifeq ($(shell uname), Darwin)
	SGE_PLATFORM_OS := MacOSX

else ifeq ($(shell uname), Linux)
	SGE_PLATFORM_OS := Linux
else
	$(error Unsupported OS $(UNAME_OS))
endif

ifeq ($(SGE_PLATFORM_OS), Windows)
	sge_tools_bin = $(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

	exe_suffix=.exe
	export exe_suffix

	BUILD_DX11				:= true
	BUILD_SPIRV				:= true
	BUILD_GLSL				:= true

else ifeq ($(SGE_PLATFORM_OS), MacOSX)
	$(error TODO)
else ifeq ($(SGE_PLATFORM_OS), Linux)
	$(error TODO)
endif

export BUILD_DX11
export BUILD_SPIRV
export BUILD_GLSL

output_targets=$(addprefix $(outdir)/, $(addsuffix /info.json, $(input_files)))

sgeFileCmd=$(sge_tools_bin)/sge_file_cmd$(exe_suffix)
export sgeFileCmd

SGE_PLATFORM_OS := $(shell $(sgeFileCmd) -platform_os)
export SGE_PLATFORM_OS

sgeShaderCompiler=$(sge_tools_bin)/sge_shader_compiler$(exe_suffix)
export sgeShaderCompiler

#ninja=$(realpath ../../../../externals/vcpkg/installed/x64-windows/tools/ninja/ninja.exe)
ninja=$(sge_tools_bin)/ninja$(exe_suffix)
export ninja

#glslc=C:/VulkanSDK/1.3.290.0/Bin/glslc.exe
#glslc=$(shell echo ${VK_SDK_PATH}/Bin/glslc)$(exe_suffix)
#glslc=$(realpath ../../../../externals/vcpkg/installed/x64-windows/tools/shaderc/glslc.exe)
glslc=$(sge_tools_bin)/glslc$(exe_suffix)
export glslc

#spirv_cross=C:/VulkanSDK/1.3.290.0/Bin/spirv-cross.exe
#spirv_cross=$(shell echo ${VK_SDK_PATH}/Bin/spirv-cross)$(exe_suffix)
#spirv_cross=$(realpath ../../../../externals/vcpkg/installed/x64-windows/tools/spirv-cross/spirv-cross.exe)
spirv_cross=$(sge_tools_bin)/spirv-cross$(exe_suffix)
export spirv_cross

all: start $(output_targets)
	@echo ===== End =====

start:
	@echo ==========================
	@echo sge_tools_bin=$(sge_tools_bin)
	@echo SGE_PLATFORM_OS=$(SGE_PLATFORM_OS)
	@echo sgeFileCmd=$(sgeFileCmd)
	@echo sgeShaderCompiler=$(sgeShaderCompiler)
	@echo MAKE=$(MAKE)
	@echo ninja=$(ninja)
	@echo glslc=$(glslc)
	@echo spirv_cross=$(spirv_cross)
	@echo outdir=$(outdir)
	@echo input_files=$(input_files)
	@echo output_targets=$(output_targets)
	@echo ==========================

$(outdir)/%.shader/info.json: %.shader start FORCE
	@echo [==== $(realpath $(outdir)/$<) ====]
	$(sgeShaderCompiler) -genMakefile -file="$<" -out="$(outdir)/$<"
	@"$(MAKE)" --quiet -C "$(outdir)/$<"

FORCE:
