# Microsoft Developer Studio Project File - Name="effects_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=effects_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "effects_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "effects_sn.mak" CFG="effects_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "effects_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "effects_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "effects_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "effects_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/effects", YOKAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "effects_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "effects_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "effects_sn___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD /debug -Wa,-sn -mvu0-use-vf0-vf15 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "effects_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "effects_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "effects_sn___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "effects_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "effects_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "effects_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "$(EAOPENGL)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "SNOW_REMOVING_EA_OPENGL" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "A_COMPILER_SN" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "effects_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "effects_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "effects_sn___Win32_PS2_EE_PA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_PA"
# PROP Intermediate_Dir "PS2_EE_PA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "$(EAOPENGL)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -mlong-calls -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "effects_sn - Win32 PS2 EE Debug"
# Name "effects_sn - Win32 PS2 EE Release"
# Name "effects_sn - Win32 PS2 EE CDROM"
# Name "effects_sn - Win32 PS2 EE PA"
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

SOURCE=.\Water.cpp
# End Source File
# Begin Source File

SOURCE=.\Water.h
# End Source File
# Begin Source File

SOURCE=.\wlight.cpp
# End Source File
# Begin Source File

SOURCE=.\wlight.h
# End Source File
# End Target
# End Project
