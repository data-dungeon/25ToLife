# Microsoft Developer Studio Project File - Name="effects_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=effects_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "effects_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "effects_xbox.mak" CFG="effects_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "effects_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "effects_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/effects", YOKAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "effects_xbox - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "WIN32" /D "_XBOX" /D MMX_OK=$(MMX_OK) /D "DIRECTX" /YX /FD /G6 /Zvc6 /c
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "effects_xbox - Xbox Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "WIN32" /D "_XBOX" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "effects_xbox - Xbox Release"
# Name "effects_xbox - Xbox Debug"
# Begin Source File

SOURCE=.\ahfmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ahfmgr.h
# End Source File
# Begin Source File

SOURCE=.\ahfnode.h
# End Source File
# Begin Source File

SOURCE=.\ahfnode.hpp
# End Source File
# Begin Source File

SOURCE=.\anihf.cpp
# End Source File
# Begin Source File

SOURCE=.\anihf.h
# End Source File
# Begin Source File

SOURCE=.\canihf.cpp
# End Source File
# Begin Source File

SOURCE=.\canihf.h
# End Source File
# Begin Source File

SOURCE=.\CollisionFXHandler.h
# End Source File
# Begin Source File

SOURCE=.\CollisionFXHandlerList.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisionFXHandlerList.h
# End Source File
# Begin Source File

SOURCE=.\collisionparticle.cpp
# End Source File
# Begin Source File

SOURCE=.\collisionparticle.h
# End Source File
# Begin Source File

SOURCE=.\dropshad.cpp
# End Source File
# Begin Source File

SOURCE=.\dropshad.h
# End Source File
# Begin Source File

SOURCE=.\fastnoise.cpp
# End Source File
# Begin Source File

SOURCE=.\fastnoise.h
# End Source File
# Begin Source File

SOURCE=.\fastnoise.hpp
# End Source File
# Begin Source File

SOURCE=.\footstep.cpp
# End Source File
# Begin Source File

SOURCE=.\footstep.h
# End Source File
# Begin Source File

SOURCE=.\halo.cpp
# End Source File
# Begin Source File

SOURCE=.\halo.h
# End Source File
# Begin Source File

SOURCE=.\haloauto.h
# End Source File
# Begin Source File

SOURCE=.\haloele.cpp
# End Source File
# Begin Source File

SOURCE=.\haloele.h
# End Source File
# Begin Source File

SOURCE=.\leaf.cpp
# End Source File
# Begin Source File

SOURCE=.\leaf.h
# End Source File
# Begin Source File

SOURCE=.\leafblow.cpp
# End Source File
# Begin Source File

SOURCE=.\leafblow.h
# End Source File
# Begin Source File

SOURCE=.\leafblow.hpp
# End Source File
# Begin Source File

SOURCE=.\lensele.cpp
# End Source File
# Begin Source File

SOURCE=.\lensele.h
# End Source File
# Begin Source File

SOURCE=.\lensflare.cpp
# End Source File
# Begin Source File

SOURCE=.\lensflare.h
# End Source File
# Begin Source File

SOURCE=.\motionblur.cpp
# End Source File
# Begin Source File

SOURCE=.\motionblur.h
# End Source File
# Begin Source File

SOURCE=.\pnoise.cpp
# End Source File
# Begin Source File

SOURCE=.\pnoise.h
# End Source File
# Begin Source File

SOURCE=.\pnoise.hpp
# End Source File
# Begin Source File

SOURCE=.\siggen.cpp
# End Source File
# Begin Source File

SOURCE=.\siggen.h
# End Source File
# Begin Source File

SOURCE=.\siggen.hpp
# End Source File
# Begin Source File

SOURCE=.\simplerb.cpp
# End Source File
# Begin Source File

SOURCE=.\simplerb.h
# End Source File
# Begin Source File

SOURCE=.\simplerb.hpp
# End Source File
# Begin Source File

SOURCE=.\splinefog.cpp
# End Source File
# Begin Source File

SOURCE=.\splinefog.h
# End Source File
# Begin Source File

SOURCE=.\sseffect.cpp
# End Source File
# Begin Source File

SOURCE=.\sseffect.h
# End Source File
# Begin Source File

SOURCE=.\sseffect.hpp
# End Source File
# Begin Source File

SOURCE=.\ssemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ssemgr.h
# End Source File
# Begin Source File

SOURCE=.\ttconst.h
# End Source File
# Begin Source File

SOURCE=.\ttdisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\ttdisplay.h
# End Source File
# Begin Source File

SOURCE=.\ttemit.cpp
# End Source File
# Begin Source File

SOURCE=.\ttemit.h
# End Source File
# Begin Source File

SOURCE=.\ttemit.hpp
# End Source File
# Begin Source File

SOURCE=.\ttrack.cpp
# End Source File
# Begin Source File

SOURCE=.\ttrack.h
# End Source File
# Begin Source File

SOURCE=.\ttrack.hpp
# End Source File
# Begin Source File

SOURCE=.\ttread.cpp
# End Source File
# Begin Source File

SOURCE=.\ttread.h
# End Source File
# Begin Source File

SOURCE=.\ttread.hpp
# End Source File
# Begin Source File

SOURCE=.\ttsys.cpp
# End Source File
# Begin Source File

SOURCE=.\ttsys.h
# End Source File
# Begin Source File

SOURCE=.\wlight.cpp
# End Source File
# Begin Source File

SOURCE=.\wlight.h
# End Source File
# End Target
# End Project
