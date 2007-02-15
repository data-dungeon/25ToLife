# Microsoft Developer Studio Project File - Name="EngineHelper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=EngineHelper - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EngineHelper.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EngineHelper.mak" CFG="EngineHelper - Win32 PS2 EE CDRom Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EngineHelper - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EngineHelper - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/EngineHelper", JJKAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "WIN32" /D "OPENGL" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /Yu"EngineHelper\EngineHelperPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "WIN32" /D "OPENGL" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /FR /Yu"EngineHelper\EngineHelperPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

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

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"EngineHelper\EngineHelperPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /Yu"EngineHelper\EngineHelperPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "_DEBUG" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /FR /Yu"EngineHelper\EngineHelperPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /D "SNOW_REMOVING_EA_OPENGL" /Yu"EngineHelper\EngineHelperPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

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
# ADD BASE CPP /nologo /w /W0 /Gm /ZI /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /GZ /c
# ADD CPP /nologo /w /W0 /Gm /Zi /Od /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /D "SNOW_REMOVING_EA_OPENGL" /Yu"EngineHelper\EngineHelperPCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "EngineHelper___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "EngineHelper___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"EngineHelper\EngineHelperPCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "EngineHelper___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "EngineHelper___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"EngineHelper\EngineHelperPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "EngineHelper___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "EngineHelper___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"EngineHelper\EngineHelperPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"EngineHelper\EngineHelperPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "EngineHelper___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "EngineHelper___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "EngineHelper___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "EngineHelper___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"EngineHelper\EngineHelperPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"EngineHelper\EngineHelperPCH.h" /c
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

# Name "EngineHelper - Win32 OpenGL Release"
# Name "EngineHelper - Win32 OpenGL Debug"
# Name "EngineHelper - Win32 PS2 EE Release"
# Name "EngineHelper - Win32 PS2 EE Debug"
# Name "EngineHelper - PS2 CDRom Release"
# Name "EngineHelper - Win32 DirectX Release"
# Name "EngineHelper - Win32 DirectX Debug"
# Name "EngineHelper - PS2Direct Release"
# Name "EngineHelper - PS2Direct Debug"
# Name "EngineHelper - GameCube Debug"
# Name "EngineHelper - GameCube Release"
# Name "EngineHelper - GameCube CDROM"
# Name "EngineHelper - Win32 PS2 EE CDRom Release"
# Name "EngineHelper - Win32 GameCube Release DVDETH"
# Begin Source File

SOURCE=.\cinst.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cinst.h

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cinst.hpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\drawutil.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\drawutil.h

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EngineHelperPCH.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

# ADD BASE CPP /Yc"EngineHelper\EngineHelperPCH.h"
# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"EngineHelper\EngineHelperPCH.h"
# ADD CPP /Yc"EngineHelper\EngineHelperPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EngineHelperPCH.h
# End Source File
# Begin Source File

SOURCE=.\EngineHelperPCH_SN.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\filestream.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\filestream.h

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Hash.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Hash.h
# End Source File
# Begin Source File

SOURCE=.\hog.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hog.h

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NameLibrary.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NameLibrary.h

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Nestle.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Nestle.h

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pathfile.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pathfile.h

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sfx.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sfx.h
# End Source File
# Begin Source File

SOURCE=.\soundcollide.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soundcollide.h
# End Source File
# Begin Source File

SOURCE=.\timer.cpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\timer.h

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\timer.hpp

!IF  "$(CFG)" == "EngineHelper - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Release"

!ELSEIF  "$(CFG)" == "EngineHelper - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Debug"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube Release"

!ELSEIF  "$(CFG)" == "EngineHelper - GameCube CDROM"

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "EngineHelper - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
