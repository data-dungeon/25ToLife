@echo OFF
setlocal
call _SandboxConfig.bat

del /s /q %TEMP%\SandboxUpload
md %TEMP%\SandboxUpload
copy ..\..\OSI.bpt %TEMP%\SandboxUpload
copy EidosAccounts.py %TEMP%\SandboxUpload
copy EidosFilter.py %TEMP%\SandboxUpload
copy EidosLocales.py %TEMP%\SandboxUpload
copy TTLParticipation.py %TEMP%\SandboxUpload
copy TTLMatchMaking.py %TEMP%\SandboxUpload
copy TTLRanking.py %TEMP%\SandboxUpload
copy TTLTypeExtension.py %TEMP%\SandboxUpload
copy UserConfiguration.py %TEMP%\SandboxUpload
copy EidosFilterExact.txt %TEMP%\SandboxUpload
copy EidosFilterSubst.txt %TEMP%\SandboxUpload

del SandboxCommands.tmp
echo open sftp://ttl:%SANDBOX_PASS%@sftp.quazal.net:9200 >>SandboxCommands.tmp
echo lcd %TEMP%\SandboxUpload >>SandboxCommands.tmp

:: Get user input for sandbox
echo 1. TTL1
echo 2. TTL2
echo 3. TTL3
echo 4. TTLPC
echo 5. TTL1 - TTL3
IF /i NOT [%1]==[] SET sandbox_choice=%1
IF /i [%1]==[] SET /p sandbox_choice=Which sandbox would you like to upload to?
IF /i [%sandbox_choice%]==[1] CALL :TTL1&GOTO StartProcess
IF /i [%sandbox_choice%]==[2] CALL :TTL2&GOTO StartProcess
IF /i [%sandbox_choice%]==[3] CALL :TTL3&GOTO StartProcess
IF /i [%sandbox_choice%]==[4] CALL :TTLPC&GOTO StartProcess
IF /i [%sandbox_choice%]==[5] CALL :TTL1&CALL :TTL2&CALL :TTL3&GOTO StartProcess

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
echo cd %SANDBOX_NAME%/Scripts >>SandboxCommands.tmp
echo synchronize remote >>SandboxCommands.tmp
echo cd ../..  >>SandboxCommands.tmp
GOTO :EOF

:StartProcess
echo exit >>SandboxCommands.tmp
%WINSCP_DIR% /console /script=SandboxCommands.tmp

:End
endlocal
@echo.
@echo RESTART SANDBOX %SANDBOX_NAME% NOW!
