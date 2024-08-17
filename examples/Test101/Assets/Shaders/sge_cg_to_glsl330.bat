@REM #change current directory to this file
@%~d0
@cd %~dp0

call .\..\..\cg_to_glsl330.bat Assets/Shaders/test001.cg

@pause
