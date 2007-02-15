@echo OFF
setlocal
call _SandboxConfig.bat

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
IF /i [%1]==[] SET /p sandbox_choice=Which sandbox would you like to backup logs for?
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
md SandboxBackup_%SANDBOX_NAME%
echo cd %SANDBOX_NAME%/Scripts >>SandboxCommands.tmp
echo lcd SandboxBackup_%SANDBOX_NAME% >>SandboxCommands.tmp
echo synchronize local >>SandboxCommands.tmp
echo lcd ../ >>SandboxCommands.tmp
GOTO :EOF

:StartProcess
echo exit >>SandboxCommands.tmp
%WINSCP_DIR% /console /script=SandboxCommands.tmp

:End
endlocal