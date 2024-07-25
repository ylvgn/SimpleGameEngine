@REM #change current directory to this file
@%~d0
@cd %~dp0


@rem cg_to_glsl330.bat test001.cg
@rem cg_to_glsl330.bat simpleColog.cg
cg_to_glsl330.bat simpleLighting.cg
@rem cg_to_glsl330.bat simpleTexture.cg

@pause
