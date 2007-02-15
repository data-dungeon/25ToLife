# Microsoft Developer Studio Project File - Name="stage_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stage_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stage_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stage_sn.mak" CFG="stage_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stage_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stage_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stage_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "stage_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/stage", FNKAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stage_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "stage_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "stage_sn___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /debug -mvu0-use-vf0-vf15 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "stage_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "stage_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "stage_sn___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "stage_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "stage_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "stage_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "SNOW_REMOVING_EA_OPENGL" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "A_COMPILER_SN" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "stage_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "stage_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "stage_sn___Win32_PS2_EE_PA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_PA"
# PROP Intermediate_Dir "PS2_EE_PA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /c
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

# Name "stage_sn - Win32 PS2 EE Debug"
# Name "stage_sn - Win32 PS2 EE Release"
# Name "stage_sn - Win32 PS2 EE CDROM"
# Name "stage_sn - Win32 PS2 EE PA"
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

SOURCE=.\CastProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\CastProfile.h
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
