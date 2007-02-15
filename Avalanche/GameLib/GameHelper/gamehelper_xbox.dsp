# Microsoft Developer Studio Project File - Name="gamehelper_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=gamehelper_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gamehelper_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gamehelper_xbox.mak" CFG="gamehelper_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gamehelper_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "gamehelper_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/gamehelper", LNKAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "gamehelper_xbox - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "XBX_Release"
# PROP Intermediate_Dir "XBX_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "WIN32" /D "_XBOX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "gamehelper_xbox - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "XBX_Debug"
# PROP Intermediate_Dir "XBX_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "WIN32" /D "_XBOX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "gamehelper_xbox - Xbox Release"
# Name "gamehelper_xbox - Xbox Debug"
# Begin Source File

SOURCE=.\ctristrip.cpp
# End Source File
# Begin Source File

SOURCE=.\ctristrip.h
# End Source File
# Begin Source File

SOURCE=.\event.cpp
# End Source File
# Begin Source File

SOURCE=.\event.h
# End Source File
# Begin Source File

SOURCE=.\loadsom.cpp
# End Source File
# Begin Source File

SOURCE=.\loadsom.h
# End Source File
# Begin Source File

SOURCE=.\lod.cpp
# End Source File
# Begin Source File

SOURCE=.\lod.h
# End Source File
# Begin Source File

SOURCE=.\metrics.h
# End Source File
# Begin Source File

SOURCE=.\RandomNumber.cpp
# End Source File
# Begin Source File

SOURCE=.\RandomNumber.h
# End Source File
# Begin Source File

SOURCE=.\SimpleType.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleType.h
# End Source File
# Begin Source File

SOURCE=.\TableLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\TableLabel.h
# End Source File
# Begin Source File

SOURCE=.\TableLookup.cpp
# End Source File
# Begin Source File

SOURCE=.\TableLookup.h
# End Source File
# Begin Source File

SOURCE=.\TableToken.cpp
# End Source File
# Begin Source File

SOURCE=.\TableToken.h
# End Source File
# End Target
# End Project
