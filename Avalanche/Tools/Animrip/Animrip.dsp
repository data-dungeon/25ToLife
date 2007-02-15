# Microsoft Developer Studio Project File - Name="Animrip" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Animrip - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Animrip.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Animrip.mak" CFG="Animrip - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Animrip - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Animrip - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Animrip", ABGAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Animrip - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "$(AVTOOLS)" /I "$(AVTOOLS)\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "YY_NO_SCAN_STRING" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dbwriter.lib /nologo /subsystem:console /debug /machine:I386 /libpath:"$(AVTOOLS)\apps\dbuilder.lib\release"

!ELSEIF  "$(CFG)" == "Animrip - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "$(AVTOOLS)" /I "$(AVTOOLS)\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "_DEBUG" /D "YYDEBUG" /D "YYERROR_VERBOSE" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "YY_NO_SCAN_STRING" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dbwriter.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libc.lib" /pdbtype:sept /libpath:"$(AVTOOLS)\apps\dbuilder.lib\debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Animrip - Win32 Release"
# Name "Animrip - Win32 Debug"
# Begin Group "test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\test\RIDERS_m.h
# End Source File
# Begin Source File

SOURCE=.\test\test.h
# End Source File
# Begin Source File

SOURCE=.\test\test.scr
# End Source File
# End Group
# Begin Source File

SOURCE=.\animrip.cpp
# End Source File
# Begin Source File

SOURCE=.\animrip.h
# End Source File
# Begin Source File

SOURCE=.\Anm_gram.c
# End Source File
# Begin Source File

SOURCE=.\ANM_GRAM.H
# End Source File
# Begin Source File

SOURCE=.\anm_gram.y

!IF  "$(CFG)" == "Animrip - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\anm_gram.y

BuildCmds= \
	bison -dy -oanm_gram.c  $(InputDir)\anm_gram.y

"anm_gram.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"anm_gram.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Animrip - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\anm_gram.y

BuildCmds= \
	bison -dy -oanm_gram.c  $(InputDir)\anm_gram.y

"anm_gram.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"anm_gram.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Anm_proc.c
# End Source File
# Begin Source File

SOURCE=.\Anm_scan.c
# End Source File
# Begin Source File

SOURCE=.\anm_scan.l

!IF  "$(CFG)" == "Animrip - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\anm_scan.l

"anm_scan.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -Pzz -oanm_scan.c $(InputDir)\anm_scan.l

# End Custom Build

!ELSEIF  "$(CFG)" == "Animrip - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\anm_scan.l

"anm_scan.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -Pzz -oanm_scan.c $(InputDir)\anm_scan.l

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\source\fn_wrap\fn_wrap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\GameLibNew\EngineHelper\Hash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\GameLibNew\EngineHelper\Hash.h
# End Source File
# Begin Source File

SOURCE=.\t_chunk.h
# End Source File
# End Target
# End Project
