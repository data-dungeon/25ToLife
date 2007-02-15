# Microsoft Developer Studio Project File - Name="stage_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=stage_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stage_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stage_xbox.mak" CFG="stage_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stage_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "stage_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/stage", FNKAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "stage_xbox - Xbox Release"

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

!ELSEIF  "$(CFG)" == "stage_xbox - Xbox Debug"

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

# Name "stage_xbox - Xbox Release"
# Name "stage_xbox - Xbox Debug"
# Begin Source File

SOURCE=.\actortype.h
# End Source File
# Begin Source File

SOURCE=.\AnimatedChain.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimatedChain.h
# End Source File
# Begin Source File

SOURCE=.\cactor.cpp
# End Source File
# Begin Source File

SOURCE=.\cactor.h
# End Source File
# Begin Source File

SOURCE=.\cast.cpp
# End Source File
# Begin Source File

SOURCE=.\cast.h
# End Source File
# Begin Source File

SOURCE=.\cast.hpp
# End Source File
# Begin Source File

SOURCE=.\castmember.cpp
# End Source File
# Begin Source File

SOURCE=.\castmember.h
# End Source File
# Begin Source File

SOURCE=.\celist.h
# End Source File
# Begin Source File

SOURCE=.\celist.hpp
# End Source File
# Begin Source File

SOURCE=.\cluster.cpp
# End Source File
# Begin Source File

SOURCE=.\cluster.h
# End Source File
# Begin Source File

SOURCE=.\clustere.cpp
# End Source File
# Begin Source File

SOURCE=.\clustere.h
# End Source File
# Begin Source File

SOURCE=.\clustere.hpp
# End Source File
# Begin Source File

SOURCE=.\clustersys.cpp
# End Source File
# Begin Source File

SOURCE=.\clustersys.h
# End Source File
# Begin Source File

SOURCE=.\clustersys.hpp
# End Source File
# Begin Source File

SOURCE=.\CollisionEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisionEntity.h
# End Source File
# Begin Source File

SOURCE=.\CollisionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisionManager.h
# End Source File
# Begin Source File

SOURCE=.\cpair.h
# End Source File
# Begin Source File

SOURCE=.\cpmaster.cpp
# End Source File
# Begin Source File

SOURCE=.\cpmaster.h
# End Source File
# Begin Source File

SOURCE=.\DynamicChain.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicChain.h
# End Source File
# Begin Source File

SOURCE=.\DynamicChain.hpp
# End Source File
# Begin Source File

SOURCE=.\DynamicMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicMesh.h
# End Source File
# Begin Source File

SOURCE=.\MacroChain.cpp
# End Source File
# Begin Source File

SOURCE=.\MacroChain.h
# End Source File
# Begin Source File

SOURCE=.\moments.cpp
# End Source File
# Begin Source File

SOURCE=.\moments.h
# End Source File
# Begin Source File

SOURCE=.\protagonist.cpp
# End Source File
# Begin Source File

SOURCE=.\protagonist.h
# End Source File
# Begin Source File

SOURCE=.\record.cpp
# End Source File
# Begin Source File

SOURCE=.\record.h
# End Source File
# Begin Source File

SOURCE=.\scatter.cpp
# End Source File
# Begin Source File

SOURCE=.\scatter.h
# End Source File
# Begin Source File

SOURCE=.\stage.cpp
# End Source File
# Begin Source File

SOURCE=.\stage.h
# End Source File
# Begin Source File

SOURCE=.\StageProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\StageProfile.h
# End Source File
# Begin Source File

SOURCE=.\SurfaceLookup.cpp
# End Source File
# Begin Source File

SOURCE=.\SurfaceLookup.h
# End Source File
# Begin Source File

SOURCE=.\SurfaceType.cpp
# End Source File
# Begin Source File

SOURCE=.\SurfaceType.h
# End Source File
# Begin Source File

SOURCE=.\TerrainEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainEntity.h
# End Source File
# End Target
# End Project
