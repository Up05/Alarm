@echo off
set batpath=%~dp0
call java -jar %batpath%/Alarm-cli.jar %*
@echo on