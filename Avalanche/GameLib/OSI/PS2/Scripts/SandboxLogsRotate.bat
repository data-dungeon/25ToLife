@echo OFF
setlocal
call _SandboxConfig.bat

:: Get user input for sandbox
echo 1. TTL1
echo 2. TTL2
echo 3. TTL3
echo 4. TTLPC
echo 5. TTL1 - TTL3
echo 6. ALL
IF /i NOT [%1]==[] SET sandbox_choice=%1
IF /i [%1]==[] SET /p sandbox_choice=Which sandbox would you like to rotate logs of?
IF /i [%sandbox_choice%]==[1] CALL :TTL1
IF /i [%sandbox_choice%]==[2] CALL :TTL2
IF /i [%sandbox_choice%]==[3] CALL :TTL3
IF /i [%sandbox_choice%]==[4] CALL :TTLPC
IF /i [%sandbox_choice%]==[5] CALL :TTL1&CALL :TTL2&CALL :TTL3
IF /i [%sandbox_choice%]==[6] CALL :TTL1&CALL :TTL2&CALL :TTL3&CALL :TTLPC

GOTO End

:TTL1
SET SANDBOX_NAME=ttl1
python -u SandboxLogsRotate.py
GOTO :EOF
:TTL2
SET SANDBOX_NAME=ttl2
python -u SandboxLogsRotate.py
GOTO :EOF
:TTL3
SET SANDBOX_NAME=ttl3
python -u SandboxLogsRotate.py
GOTO :EOF
:TTLPC
SET SANDBOX_NAME=ttlpc
python -u SandboxLogsRotate.py
GOTO :EOF

:End
endlocal