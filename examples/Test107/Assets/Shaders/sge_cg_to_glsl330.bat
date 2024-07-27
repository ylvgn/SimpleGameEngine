@REM #change current directory to this file
@%~d0
@cd %~dp0


start .\cg_to_glsl330.bat test001.cg
start .\cg_to_glsl330.bat simpleColor.cg
start .\cg_to_glsl330.bat simpleLighting.cg
start .\cg_to_glsl330.bat simpleTexture.cg

@pause
