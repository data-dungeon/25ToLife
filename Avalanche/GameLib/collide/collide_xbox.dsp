# Microsoft Developer Studio Project File - Name="collide_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=collide_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "collide_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "collide_xbox.mak" CFG="collide_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "collide_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "collide_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/collide", NDGAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "collide_xbox - Xbox Release"

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

!ELSEIF  "$(CFG)" == "collide_xbox - Xbox Debug"

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

# Name "collide_xbox - Xbox Release"
# Name "collide_xbox - Xbox Debug"
# Begin Source File

SOURCE=.\aabb.cpp
# End Source File
# Begin Source File

SOURCE=.\aabb.h
# End Source File
# Begin Source File

SOURCE=.\aabb.hpp
# End Source File
# Begin Source File

SOURCE=.\bsphere.h
# End Source File
# Begin Source File

SOURCE=.\bsphere.hpp
# End Source File
# Begin Source File

SOURCE=.\captureTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\captureTerrain.h
# End Source File
# Begin Source File

SOURCE=.\chord.cpp
# End Source File
# Begin Source File

SOURCE=.\chord.h
# End Source File
# Begin Source File

SOURCE=.\chord.hpp
# End Source File
# Begin Source File

SOURCE=.\clipseg.cpp
# End Source File
# Begin Source File

SOURCE=.\clipseg.h
# End Source File
# Begin Source File

SOURCE=.\clipseg.hpp
# End Source File
# Begin Source File

SOURCE=.\collcache.cpp
# End Source File
# Begin Source File

SOURCE=.\collcache.h
# End Source File
# Begin Source File

SOURCE=.\collcache.hpp
# End Source File
# Begin Source File

SOURCE=.\collision.cpp
# End Source File
# Begin Source File

SOURCE=.\collision.h
# End Source File
# Begin Source File

SOURCE=.\collision.hpp
# End Source File
# Begin Source File

SOURCE=.\collsys.cpp
# End Source File
# Begin Source File

SOURCE=.\collsys.h
# End Source File
# Begin Source File

SOURCE=.\collsys.hpp
# End Source File
# Begin Source File

SOURCE=.\LoopSubdiv.cpp
# End Source File
# Begin Source File

SOURCE=.\LoopSubdiv.h
# End Source File
# Begin Source File

SOURCE=.\obb.cpp
# End Source File
# Begin Source File

SOURCE=.\obb.h
# End Source File
# Begin Source File

SOURCE=.\obb.hpp
# End Source File
# Begin Source File

SOURCE=.\obbchild.cpp
# End Source File
# Begin Source File

SOURCE=.\obbchild.h
# End Source File
# Begin Source File

SOURCE=.\obbchild.hpp
# End Source File
# Begin Source File

SOURCE=.\obbcl.cpp
# End Source File
# Begin Source File

SOURCE=.\obbcl.h
# End Source File
# Begin Source File

SOURCE=.\obbcl.hpp
# End Source File
# Begin Source File

SOURCE=.\obblist.h
# End Source File
# Begin Source File

SOURCE=.\obbplane.cpp
# End Source File
# Begin Source File

SOURCE=.\obbplane.h
# End Source File
# Begin Source File

SOURCE=.\obbqt.cpp
# End Source File
# Begin Source File

SOURCE=.\obbqt.h
# End Source File
# Begin Source File

SOURCE=.\quadtest.h
# End Source File
# Begin Source File

SOURCE=.\quadtest.hpp
# End Source File
# Begin Source File

SOURCE=.\quadtree.h
# End Source File
# Begin Source File

SOURCE=.\scanhit.h
# End Source File
# Begin Source File

SOURCE=.\scanhit.hpp
# End Source File
# Begin Source File

SOURCE=.\tcluster.cpp
# End Source File
# Begin Source File

SOURCE=.\tcluster.h
# End Source File
# Begin Source File

SOURCE=.\tcollide.cpp
# End Source File
# Begin Source File

SOURCE=.\tcollide.h
# End Source File
# Begin Source File

SOURCE=.\tcollide.hpp
# End Source File
# Begin Source File

SOURCE=.\tcolliter.h
# End Source File
# Begin Source File

SOURCE=.\tgeo.h
# End Source File
# Begin Source File

SOURCE=.\tpdb.cpp
# End Source File
# Begin Source File

SOURCE=.\tpdb.h
# End Source File
# Begin Source File

SOURCE=.\tperim.cpp
# End Source File
# Begin Source File

SOURCE=.\tperim.h
# End Source File
# Begin Source File

SOURCE=.\tritest.cpp
# End Source File
# Begin Source File

SOURCE=.\tritest.h
# End Source File
# Begin Source File

SOURCE=.\tritest.hpp
# End Source File
# Begin Source File

SOURCE=.\tsectgeo.h
# End Source File
# Begin Source File

SOURCE=.\tsector.cpp
# End Source File
# Begin Source File

SOURCE=.\tsector.h
# End Source File
# Begin Source File

SOURCE=.\tsector.hpp
# End Source File
# Begin Source File

SOURCE=.\xzrect.h
# End Source File
# Begin Source File

SOURCE=.\xzrect.hpp
# End Source File
# End Target
# End Project
