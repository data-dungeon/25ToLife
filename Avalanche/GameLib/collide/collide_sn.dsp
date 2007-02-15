# Microsoft Developer Studio Project File - Name="collide_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=collide_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "collide_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "collide_sn.mak" CFG="collide_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "collide_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "collide_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "collide_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/collide", NDGAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "collide_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "collide_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "collide_sn___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD /debug -Wa,-sn -mvu0-use-vf0-vf15 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "collide_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "collide_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "collide_sn___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /O2 /Ob0 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -fkeep-inline-functions -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "collide_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "collide_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "collide_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "A_COMPILER_SN" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "collide_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "collide_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "collide_sn___Win32_PS2_EE_PA"
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

# Name "collide_sn - Win32 PS2 EE Debug"
# Name "collide_sn - Win32 PS2 EE Release"
# Name "collide_sn - Win32 PS2 EE CDROM"
# Name "collide_sn - Win32 PS2 EE PA"
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

SOURCE=.\CollideProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\CollideProfile.h
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

SOURCE=.\mrm.h
# End Source File
# Begin Source File

SOURCE=.\mrm.hpp
# End Source File
# Begin Source File

SOURCE=.\mrmnode.h
# End Source File
# Begin Source File

SOURCE=.\mrmnode.hpp
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
