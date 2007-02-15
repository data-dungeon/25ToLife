# Microsoft Developer Studio Project File - Name="mathutil_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=mathutil_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mathutil_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mathutil_xbox.mak" CFG="mathutil_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mathutil_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "mathutil_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/mathutil", ZJKAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "mathutil_xbox - Xbox Release"

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
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mathutil_xbox - Xbox Debug"

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
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "mathutil_xbox - Xbox Release"
# Name "mathutil_xbox - Xbox Debug"
# Begin Source File

SOURCE=.\avgnum.h
# End Source File
# Begin Source File

SOURCE=.\Bairstow.cpp
# End Source File
# Begin Source File

SOURCE=.\Bairstow.h
# End Source File
# Begin Source File

SOURCE=.\Ballistic.cpp
# End Source File
# Begin Source File

SOURCE=.\Ballistic.h
# End Source File
# Begin Source File

SOURCE=.\bilerp.h
# End Source File
# Begin Source File

SOURCE=.\Cholesky.cpp
# End Source File
# Begin Source File

SOURCE=.\Cholesky.h
# End Source File
# Begin Source File

SOURCE=.\Cholesky.hpp
# End Source File
# Begin Source File

SOURCE=.\dda.h
# End Source File
# Begin Source File

SOURCE=.\dda.hpp
# End Source File
# Begin Source File

SOURCE=.\eigen.cpp
# End Source File
# Begin Source File

SOURCE=.\eigen.h
# End Source File
# Begin Source File

SOURCE=.\eigen.hpp
# End Source File
# Begin Source File

SOURCE=.\Gauss.cpp
# End Source File
# Begin Source File

SOURCE=.\Gauss.h
# End Source File
# Begin Source File

SOURCE=.\Gauss.hpp
# End Source File
# Begin Source File

SOURCE=.\linearsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\linearsystem.h
# End Source File
# Begin Source File

SOURCE=.\mathcons.h
# End Source File
# Begin Source File

SOURCE=.\NumberRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\NumberRoot.h
# End Source File
# Begin Source File

SOURCE=.\SecantSolve.cpp
# End Source File
# Begin Source File

SOURCE=.\SecantSolve.h
# End Source File
# End Target
# End Project
