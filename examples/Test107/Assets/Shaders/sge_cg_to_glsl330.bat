@REM #change current directory to this file
@%~d0
@cd %~dp0

call .\cg_to_glsl330.bat test001.cg
call .\cg_to_glsl330.bat simpleColor.cg
call .\cg_to_glsl330.bat simpleLighting.cg
call .\cg_to_glsl330.bat simpleTexture.cg
call .\cg_to_glsl330.bat imgui.cg

@pause
