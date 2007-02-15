# Microsoft Developer Studio Project File - Name="platform_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=platform_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "platform_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "platform_xbox.mak" CFG="platform_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "platform_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "platform_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/platform", ODGAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "platform_xbox - Xbox Release"

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

!ELSEIF  "$(CFG)" == "platform_xbox - Xbox Debug"

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

# Name "platform_xbox - Xbox Release"
# Name "platform_xbox - Xbox Debug"
# Begin Group "xbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Xbox\xboxapp.cpp
# End Source File
# Begin Source File

SOURCE=.\Xbox\xboxapp.h
# End Source File
# Begin Source File

SOURCE=.\Xbox\xboxcard.h
# End Source File
# Begin Source File

SOURCE=.\Xbox\xboxccd.cpp
# End Source File
# Begin Source File

SOURCE=.\Xbox\xboxccd.h
# End Source File
# Begin Source File

SOURCE=.\Xbox\xboxctrol.cpp
# End Source File
# Begin Source File

SOURCE=.\Xbox\xboxctrol.h
# End Source File
# Begin Source File

SOURCE=.\Xbox\xboxinput.cpp
# End Source File
# Begin Source File

SOURCE=.\Xbox\xboxinput.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ctroller.cpp
# End Source File
# Begin Source File

SOURCE=.\ctroller.h
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\gameapp.cpp
# End Source File
# Begin Source File

SOURCE=.\gameapp.h
# End Source File
# Begin Source File

SOURCE=.\gameinit.h
# End Source File
# Begin Source File

SOURCE=.\gameloop.h
# End Source File
# Begin Source File

SOURCE=.\keyctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\keyctrl.h
# End Source File
# Begin Source File

SOURCE=.\memcard.cpp
# End Source File
# Begin Source File

SOURCE=.\memcard.h
# End Source File
# Begin Source File

SOURCE=.\systemi.cpp
# End Source File
# Begin Source File

SOURCE=.\systemi.h
# End Source File
# End Target
# End Project
