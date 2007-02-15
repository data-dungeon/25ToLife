# Microsoft Developer Studio Project File - Name="CollideHelper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CollideHelper - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "CollideHelper.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "CollideHelper.mak" CFG="CollideHelper - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "CollideHelper - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CollideHelper - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/Avalanche/GameLib/CollideHelper"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /Yu"CollideHelper\CollideHelperPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"CollideHelper\CollideHelperPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

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

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

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

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"CollideHelper\CollideHelperPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"CollideHelper\CollideHelperPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "_DEBUG" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /Yu"CollideHelper\CollideHelperPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

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
# ADD CPP /nologo /w /W0 /Gm /Zi /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Yu"CollideHelper\CollideHelperPCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Yu"CollideHelper\CollideHelperPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CollideHelper___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "CollideHelper___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"CollideHelper\CollideHelperPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CollideHelper___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "CollideHelper___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"CollideHelper\CollideHelperPCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CollideHelper___Win32_Max_Release"
# PROP BASE Intermediate_Dir "CollideHelper___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /Yu"CollideHelper\CollideHelperPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CollideHelper___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "CollideHelper___Win32_Max_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"CollideHelper\CollideHelperPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CollideHelper___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "CollideHelper___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"CollideHelper\CollideHelperPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"CollideHelper\CollideHelperPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CollideHelper___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "CollideHelper___Win32_PS2_EE_CDRom_Release"
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

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CollideHelper___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "CollideHelper___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"CollideHelper\CollideHelperPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"CollideHelper\CollideHelperPCH.h" /c
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

# Name "CollideHelper - Win32 OpenGL Release"
# Name "CollideHelper - Win32 OpenGL Debug"
# Name "CollideHelper - Win32 PS2 EE Release"
# Name "CollideHelper - Win32 PS2 EE Debug"
# Name "CollideHelper - PS2 CDRom Release"
# Name "CollideHelper - Win32 DirectX Release"
# Name "CollideHelper - Win32 DirectX Debug"
# Name "CollideHelper - PS2Direct Debug"
# Name "CollideHelper - PS2Direct Release"
# Name "CollideHelper - GameCube Release"
# Name "CollideHelper - GameCube Debug"
# Name "CollideHelper - Win32 Max Release"
# Name "CollideHelper - Win32 Max Debug"
# Name "CollideHelper - GameCube CDROM"
# Name "CollideHelper - Win32 PS2 EE CDRom Release"
# Name "CollideHelper - Win32 GameCube Release DVDETH"
# Begin Source File

SOURCE=.\AabbEntity.h
# End Source File
# Begin Source File

SOURCE=.\CollideHelperPCH.cpp

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1
# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1
# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1
# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"CollideHelper\CollideHelperPCH.h"
# ADD CPP /Yc"CollideHelper\CollideHelperPCH.h"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\CollideHelperPCH.h
# End Source File
# Begin Source File

SOURCE=.\CollideHelperPCH_SN.cpp

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\EventMiner.cpp

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EventMiner.h
# End Source File
# Begin Source File

SOURCE=.\LineEntity.cpp

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LineEntity.h
# End Source File
# Begin Source File

SOURCE=.\LineEntity.hpp
# End Source File
# Begin Source File

SOURCE=.\LineQuery.h
# End Source File
# Begin Source File

SOURCE=.\LineQuery.hpp
# End Source File
# Begin Source File

SOURCE=.\obbchild.cpp

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\obbchild.h
# End Source File
# Begin Source File

SOURCE=.\obbchild.hpp
# End Source File
# Begin Source File

SOURCE=.\ObbChildEntity.h
# End Source File
# Begin Source File

SOURCE=.\ObbEntity.h
# End Source File
# Begin Source File

SOURCE=.\ObbList.h
# End Source File
# Begin Source File

SOURCE=.\ObbList.hpp
# End Source File
# Begin Source File

SOURCE=.\ObjectList.h
# End Source File
# Begin Source File

SOURCE=.\ObjectList.hpp
# End Source File
# Begin Source File

SOURCE=.\Particle.h
# End Source File
# Begin Source File

SOURCE=.\SphereEntity.h
# End Source File
# Begin Source File

SOURCE=.\XZEntity.cpp

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XZEntity.h
# End Source File
# Begin Source File

SOURCE=.\XZEntity.hpp
# End Source File
# Begin Source File

SOURCE=.\zone.cpp

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zone.h
# End Source File
# Begin Source File

SOURCE=.\zonemap.cpp

!IF  "$(CFG)" == "CollideHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Release"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "CollideHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CollideHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zonemap.h
# End Source File
# End Target
# End Project
