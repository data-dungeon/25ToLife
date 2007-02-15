# Microsoft Developer Studio Project File - Name="collide" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=collide - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "collide.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "collide.mak" CFG="collide - Win32 PS2 EE CDRom Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "collide - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "collide - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/collide", NDGAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WGL_Release"
# PROP BASE Intermediate_Dir "WGL_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WGL_Release"
# PROP Intermediate_Dir "WGL_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /Yu"Collide\CollidePCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WGL_Debug"
# PROP BASE Intermediate_Dir "WGL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WGL_Debug"
# PROP Intermediate_Dir "WGL_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"Collide\CollidePCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PS2_EE_Release"
# PROP BASE Intermediate_Dir "PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gm /GX /ZI /Od /D "NDEBUG" /YX /GZ /c
# ADD CPP /nologo /w /W0 /Zi /O2 /Ob0 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn -Winline -fkeep-inline-functions -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PS2_EE_Debug"
# PROP BASE Intermediate_Dir "PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gm /GX /ZI /Od /D "_DEBUG" /YX /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /debug -Wa,-sn -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PS2_CDRom_Release"
# PROP BASE Intermediate_Dir "PS2_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_CDRom_Release"
# PROP Intermediate_Dir "PS2_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /ZI /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "PS2" /D "_DEBUG" /YX /FD /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Collide\CollidePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WDX_Release"
# PROP BASE Intermediate_Dir "WDX_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WDX_Release"
# PROP Intermediate_Dir "WDX_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /D "WIN32" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"Collide\CollidePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WDX_Debug"
# PROP BASE Intermediate_Dir "WDX_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WDX_Debug"
# PROP Intermediate_Dir "WDX_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /D "WIN32" /D "_DEBUG" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "_DEBUG" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /Yu"Collide\CollidePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PS2Direct_Debug"
# PROP BASE Intermediate_Dir "PS2Direct_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2Direct_Debug"
# PROP Intermediate_Dir "PS2Direct_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Gm /ZI /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /GZ /c
# ADD CPP /nologo /w /W0 /Gm /Zi /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Yu"Collide\CollidePCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PS2Direct_Release"
# PROP BASE Intermediate_Dir "PS2Direct_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PS2Direct_Release"
# PROP Intermediate_Dir "PS2Direct_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Yu"Collide\CollidePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "collide___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "collide___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Collide\CollidePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "collide___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "collide___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Collide\CollidePCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "collide___Win32_Max_Release"
# PROP BASE Intermediate_Dir "collide___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /D "WIN32_MAX_RELEASE" /Yu"Collide\CollidePCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "collide___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "collide___Win32_Max_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "_DEBUG" /D "WIN32" /D "WIN32_MAX_DEBUG" /FR /Yu"Collide\CollidePCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "collide___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "collide___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"Collide\CollidePCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Collide\CollidePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "collide___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "collide___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /Ob0 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -fkeep-inline-functions -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /Ob0 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" -Wa,-sn -Winline -fkeep-inline-functions -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "collide___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "collide___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Collide\CollidePCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Collide\CollidePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ENDIF 

# Begin Target

# Name "collide - Win32 OpenGL Release"
# Name "collide - Win32 OpenGL Debug"
# Name "collide - Win32 PS2 EE Release"
# Name "collide - Win32 PS2 EE Debug"
# Name "collide - PS2 CDRom Release"
# Name "collide - Win32 DirectX Release"
# Name "collide - Win32 DirectX Debug"
# Name "collide - PS2Direct Debug"
# Name "collide - PS2Direct Release"
# Name "collide - GameCube Release"
# Name "collide - GameCube Debug"
# Name "collide - Win32 Max Release"
# Name "collide - Win32 Max Debug"
# Name "collide - GameCube CDROM"
# Name "collide - Win32 PS2 EE CDRom Release"
# Name "collide - Win32 GameCube Release DVDETH"
# Begin Source File

SOURCE=.\aabb.h

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aabb.hpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AabbObject.h
# End Source File
# Begin Source File

SOURCE=.\AabbObject.hpp
# End Source File
# Begin Source File

SOURCE=.\AabbToAabb.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AabbToAabb.h
# End Source File
# Begin Source File

SOURCE=.\AabbToAabb.hpp
# End Source File
# Begin Source File

SOURCE=.\AabbToObb.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AabbToObb.h
# End Source File
# Begin Source File

SOURCE=.\AabbToObb.hpp
# End Source File
# Begin Source File

SOURCE=.\AabbToTerrain.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AabbToTerrain.h
# End Source File
# Begin Source File

SOURCE=.\AabbToTerrain.hpp
# End Source File
# Begin Source File

SOURCE=.\cdbhdr.h
# End Source File
# Begin Source File

SOURCE=.\CollidePCH.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# ADD BASE CPP /Yc"Collide\CollidePCH.h"
# ADD CPP /Yc"Collide\CollidePCH.h"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Collide\CollidePCH.h"
# ADD CPP /Yc"Collide\CollidePCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CollidePCH.h
# End Source File
# Begin Source File

SOURCE=.\CollidePCH_SN.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collsys.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collsys.h

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collsys.hpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cpsys.h
# End Source File
# Begin Source File

SOURCE=.\cube.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cube.h
# End Source File
# Begin Source File

SOURCE=.\cube.hpp
# End Source File
# Begin Source File

SOURCE=.\Cull.h
# End Source File
# Begin Source File

SOURCE=.\DynamicEntities.h
# End Source File
# Begin Source File

SOURCE=.\DynamicEntities.hpp
# End Source File
# Begin Source File

SOURCE=.\EntityList.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EntityList.h
# End Source File
# Begin Source File

SOURCE=.\EntityList.hpp
# End Source File
# Begin Source File

SOURCE=.\Event.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Event.h
# End Source File
# Begin Source File

SOURCE=.\Event.hpp
# End Source File
# Begin Source File

SOURCE=.\Intersection.h
# End Source File
# Begin Source File

SOURCE=.\Intersection.hpp
# End Source File
# Begin Source File

SOURCE=.\Line.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Line.h
# End Source File
# Begin Source File

SOURCE=.\Line.hpp
# End Source File
# Begin Source File

SOURCE=.\LineToAabb.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LineToAabb.h
# End Source File
# Begin Source File

SOURCE=.\LineToAabb.hpp
# End Source File
# Begin Source File

SOURCE=.\LineToObb.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LineToObb.h
# End Source File
# Begin Source File

SOURCE=.\LineToObb.hpp
# End Source File
# Begin Source File

SOURCE=.\LineToSphere.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LineToSphere.h
# End Source File
# Begin Source File

SOURCE=.\LineToSphere.hpp
# End Source File
# Begin Source File

SOURCE=.\LineToTerrain.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LineToTerrain.h
# End Source File
# Begin Source File

SOURCE=.\LineToTerrain.hpp
# End Source File
# Begin Source File

SOURCE=.\List.h
# End Source File
# Begin Source File

SOURCE=.\List.hpp
# End Source File
# Begin Source File

SOURCE=.\mrm.h

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mrm.hpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mrm2.h
# End Source File
# Begin Source File

SOURCE=.\mrm2.hpp
# End Source File
# Begin Source File

SOURCE=.\mrmnode.h

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mrmnode.hpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Node.h
# End Source File
# Begin Source File

SOURCE=.\obb.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1
# ADD BASE CPP /O2
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\obb.h

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\obb.hpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\obbStatics.h
# End Source File
# Begin Source File

SOURCE=.\ObbToObb.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ObbToObb.h
# End Source File
# Begin Source File

SOURCE=.\ObbToObb.hpp
# End Source File
# Begin Source File

SOURCE=.\ObbToTerrain.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ObbToTerrain.h
# End Source File
# Begin Source File

SOURCE=.\ObbToTerrain.hpp
# End Source File
# Begin Source File

SOURCE=.\Object.h
# End Source File
# Begin Source File

SOURCE=.\ObjectLayer.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ObjectLayer.h
# End Source File
# Begin Source File

SOURCE=.\ObjectQT.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ObjectQT.h
# End Source File
# Begin Source File

SOURCE=.\octtree.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\octtree.h
# End Source File
# Begin Source File

SOURCE=.\octtree.hpp
# End Source File
# Begin Source File

SOURCE=.\otnode.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\otnode.h
# End Source File
# Begin Source File

SOURCE=.\otnode.hpp
# End Source File
# Begin Source File

SOURCE=.\ottestid.h
# End Source File
# Begin Source File

SOURCE=.\ottestid.hpp
# End Source File
# Begin Source File

SOURCE=.\Pair.h
# End Source File
# Begin Source File

SOURCE=.\Pair.hpp
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

SOURCE=.\QueryEntity.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\QueryEntity.h
# End Source File
# Begin Source File

SOURCE=.\QueryEntity.hpp
# End Source File
# Begin Source File

SOURCE=.\Sphere.h
# End Source File
# Begin Source File

SOURCE=.\Sphere.hpp
# End Source File
# Begin Source File

SOURCE=.\SphereToObb.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SphereToObb.h
# End Source File
# Begin Source File

SOURCE=.\SphereToObb.hpp
# End Source File
# Begin Source File

SOURCE=.\SphereToTerrain.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SphereToTerrain.h
# End Source File
# Begin Source File

SOURCE=.\SphereToTerrain.hpp
# End Source File
# Begin Source File

SOURCE=.\Stack.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stack.h
# End Source File
# Begin Source File

SOURCE=.\Stack.hpp
# End Source File
# Begin Source File

SOURCE=.\StackArray.h
# End Source File
# Begin Source File

SOURCE=.\StackArray.hpp
# End Source File
# Begin Source File

SOURCE=.\StaticEntity.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StaticEntity.h
# End Source File
# Begin Source File

SOURCE=.\StaticEntity.hpp
# End Source File
# Begin Source File

SOURCE=.\tclayer.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tclayer.h
# End Source File
# Begin Source File

SOURCE=.\tclayer.hpp
# End Source File
# Begin Source File

SOURCE=.\tcllist.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tcllist.h
# End Source File
# Begin Source File

SOURCE=.\tcllist.hpp
# End Source File
# Begin Source File

SOURCE=.\Terrain.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Terrain.h
# End Source File
# Begin Source File

SOURCE=.\Terrain.hpp
# End Source File
# Begin Source File

SOURCE=.\TerrainEntity.h
# End Source File
# Begin Source File

SOURCE=.\terraintri.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\terraintri.h
# End Source File
# Begin Source File

SOURCE=.\terraintri.hpp
# End Source File
# Begin Source File

SOURCE=.\TriangleList.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TriangleList.h
# End Source File
# Begin Source File

SOURCE=.\TriangleList.hpp
# End Source File
# Begin Source File

SOURCE=.\tritest.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tritest.h

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tritest.hpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tsuspect.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tsuspect.h
# End Source File
# Begin Source File

SOURCE=.\xzrect.cpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xzrect.h

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xzrect.hpp

!IF  "$(CFG)" == "collide - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "collide - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "collide - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "collide - PS2Direct Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Release"

!ELSEIF  "$(CFG)" == "collide - GameCube Debug"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Release"

!ELSEIF  "$(CFG)" == "collide - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "collide - GameCube CDROM"

!ELSEIF  "$(CFG)" == "collide - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "collide - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
