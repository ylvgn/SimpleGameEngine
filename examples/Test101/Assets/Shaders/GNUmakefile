.PHONY: all FORCE start
# "all" must be first target
all:

#----------------------

outdir ?= ../../LocalTemp/Imported/Assets/Shaders

input_files=\
	test.shader \
	terrain.shader \
	terrain_test.shader \
	imgui.shader \
	line.shader \
	test_constbuffer.shader \
	test_texture.shader \
#-------------

#input_files=test.shader # just for single test

ifeq ($(OS), Windows_NT) # is Windows_NT on XP, 2000, 7, Vista, 10...
    SGE_PLATFORM_OS := Windows

else ifeq ($(shell uname), Darwin)
	SGE_PLATFORM_OS := MacOSX

else ifeq ($(shell uname), Linux)
	SGE_PLATFORM_OS := Linux
else
	$(error Unsupported OS $(UNAME_OS))
endif

ifeq ($(SGE_PLATFORM_OS), Windows)
	sge_tools_bin = $(realpath ../../../../externals/_tools)
	sge_build_bin = $(realpath ../../../../build/SimpleGameEngine-x64-windows)

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

sgeFileCmd=$(sge_build_bin)/src/tools/file_cmd/Debug/sge_file_cmd$(exe_suffix)
export sgeFileCmd

SGE_PLATFORM_OS := $(shell $(sgeFileCmd) -platform_os)
export SGE_PLATFORM_OS

sgeShaderCompiler=$(sge_build_bin)/src/render/shader_compiler/Debug/sge_shader_compiler$(exe_suffix)
export sgeShaderCompiler

ninja=$(sge_tools_bin)/ninja$(exe_suffix)
export ninja

glslc=$(sge_tools_bin)/glslc$(exe_suffix)
export glslc

spirv_cross=$(sge_tools_bin)/spirv-cross$(exe_suffix)
export spirv_cross

all: start $(output_targets)
	@echo ===== End =====

start:
	@echo ==========================
	@echo SGE_PLATFORM_OS=$(SGE_PLATFORM_OS)
	@echo sgeFileCmd=$(sgeFileCmd)
	@echo sgeShaderCompiler=$(sgeShaderCompiler)
	@echo MAKE=$(MAKE)
	@echo ninja=$(ninja)
	@echo glslc=$(glslc)
	@echo spirv_cross=$(spirv_cross)
	@echo sge_tools_bin=$(sge_tools_bin)
	@echo output_targets=$(output_targets)
	@echo ==========================

$(outdir)/%.shader/info.json: %.shader start FORCE
	@echo [==== $(outdir)/$< ====]
	@echo [==== $(realpath $(outdir)/$<) ====]
	$(sgeShaderCompiler) -genMakefile -file="$<" -out="$(outdir)/$<"
	@"$(MAKE)" --quiet -C "$(outdir)/$<"

FORCE:
