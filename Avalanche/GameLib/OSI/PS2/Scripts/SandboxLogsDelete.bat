@echo OFF
setlocal
call _SandboxConfig.bat
@echo.

del SandboxCommands.tmp
echo open sftp://ttl:%SANDBOX_PASS%@sftp.quazal.net:9200 >>SandboxCommands.tmp

:: Get user input for sandbox
echo 1. TTL1
echo 2. TTL2
echo 3. TTL3
echo 4. TTLPC
echo 5. TTL1 - TTL3
echo 6. ALL
IF /i NOT [%1]==[] SET sandbox_choice=%1
IF /i [%1]==[] SET /p sandbox_choice=Which sandbox would you like to delete logs from?
IF /i [%sandbox_choice%]==[1] CALL :TTL1&GOTO StartProcess
IF /i [%sandbox_choice%]==[2] CALL :TTL2&GOTO StartProcess
IF /i [%sandbox_choice%]==[3] CALL :TTL3&GOTO StartProcess
IF /i [%sandbox_choice%]==[4] CALL :TTLPC&GOTO StartProcess
IF /i [%sandbox_choice%]==[5] CALL :TTL1&CALL :TTL2&CALL :TTL3&GOTO StartProcess
IF /i [%sandbox_choice%]==[6] CALL :TTL1&CALL :TTL2&CALL :TTL3&CALL :TTLPC&GOTO StartProcess

GOTO End

:TTL1
SET SANDBOX_NAME=ttl1
CALL :AddSandbox
GOTO :EOF
:TTL2
SET SANDBOX_NAME=ttl2
CALL :AddSandbox
GOTO :EOF
:TTL3
SET SANDBOX_NAME=ttl3
CALL :AddSandbox
GOTO :EOF
:TTLPC
SET SANDBOX_NAME=ttlpc
CALL :AddSandbox
GOTO :EOF

:AddSandbox
echo cd %SANDBOX_NAME%/Logs >>SandboxCommands.tmp
echo rm %SANDBOX_LOG_FILE% >>SandboxCommands.tmp
echo rm "Quazal Rendez-Vous.log" >>SandboxCommands.tmp
echo rm "QuazalAuthentication.log" >>SandboxCommands.tmp
GOTO :EOF

:StartProcess
echo exit >>SandboxCommands.tmp
%WINSCP_DIR% /console /script=SandboxCommands.tmp

:End
@echo.
@echo LOGS DELETED FOR SANDBOX %SANDBOX_NAME%!
endlocal