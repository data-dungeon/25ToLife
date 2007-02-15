OVERVIEW
-------------------------------------------------------------------------------
There are currently 8 batch files and 1 python file for RendezVous script and log management
that are checked into Perforce.

SandboxLogGetEvents.bat
SandboxLogGetMySandbox.bat
SandboxLogsDelete.bat
SandboxLogsGet.bat
SandboxLogsRotate.bat
SandboxScriptsBackup.bat
SandboxScriptsUpload.bat
SandboxShell.bat

SandboxLogsRotate.py

One file that you need that is not checked into Perforce is _SandboxConfig.bat.
This file contains the password and a few other variables you need to run the
above batch files.  You can get this from Aaron Cole if you do not have it already.

HOW TO USE THE BATCH FILES
-------------------------------------------------------------------------------
Each batch file has a menu associated with it that will be displayed if you double
click on an individual batch file.  The menu looks like this:
1. TTL1
2. TTL2
3. TTL3
4. ALL
The batch file will then prompt you for an answer.  This allows you to specify which
sandbox you would like the batch file to affect.

The batch files also take command line parameters.  So if you are using SandboxScriptUpload.bat
with TTL3 a lot you can create a shortcut to that batch file that will automatically load with TTL3 picked.
To do this you need to create a shortcut to this batch file, then right click on the shortcut and go to properties.
You can then put a 3 (or 1, 2, 4) as a command line parameter to the shortcut (I believe you have to have the
path with the file name in quotes and the parameter on the outside e.g. "C:\SandboxUpdate.bat" 3).
This will allow you to just click on that shortcut and have it perform it's operation only
on TTL3 without asking you for input.

None of these batch files will restart the server for you.


WHAT EACH BATCH FILE DOES
-------------------------------------------------------------------------------

SandboxLogGetEvents.bat
---------------------------
Gets the log file Quazal Rendez-Vous.log and downloads it to the local
directory SandboxBackup_<TTL#>.  It will create the directory for you
and override any existing log files you have in that directory.

SandboxLogGetMySandbox.bat
---------------------------
Gets the log file MySandbox.log and downloads it to the local
directory SandboxBackup_<TTL#>.  It will create the directory for you
and override any existing log files you have in that directory.

SandboxLogsDelete.bat
---------------------------
Deletes all three log files from the sandbox server.
NOTE: You must have permission to delete these files for this to work.

SandboxLogsGet.bat
---------------------------
Gets all the log files and downloads it to the local
directory SandboxBackup_<TTL#>.  It will create the directory for you
and override any existing log files you have in that directory.

SandboxLogsRotate.bat
---------------------------
Takes all the log files on the sandbox server and renames them with the
current date and time appended to them.  Effectively saving off the current
log files on the server.
NOTE: The sandbox server(s) that this is applied to must be stopped for this to work.
NOTE: This batch file calls the SandboxLogsRotate.py file after setting up some things.
      You need to make sure that Python.exe is available in your PATH or this will not work.

SandboxScriptsBackup.bat
---------------------------
Downloads all files from the scripts directory on the sandbox server(s)
to local directory SandboxBackup_<TTL#>.  It will create the directory for you
and only download files from the server that differ from the ones in SandboxBackup_<TTL#>.

SandboxScriptsUpload.bat
---------------------------
Uploads all the .py script files and the two .txt files to the sandbox server(s).
This is done by copying those files to a temp directory using the TEMP environment
variable for the path and then just synchronizing that directory with the scripts
directory on the sandbox server(s).
NOTE: You must restart the relevant sandbox server(s) yourself.

SandboxShell.bat
---------------------------
This opens up a connection to the specified sandbox using WinSCP in the scripts directory
of the specified sandbox server.  This does not allow for more then one server to be selected.
This allows you to put in commands yourself and is for manual uploading and downloading of files
using the command line WinSCP.
