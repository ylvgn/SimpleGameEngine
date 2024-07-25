@REM #change current directory to this file
@%~d0
@cd %~dp0

@rem https://developer.download.nvidia.cn/cg/cgc.html

cgc %1 -entry vs_main -o %1.glsl_vs -profile glslv -po version=330
cgc %1 -entry ps_main -o %1.glsl_ps -profile glslf -po version=330