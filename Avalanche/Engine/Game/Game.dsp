# Microsoft Developer Studio Project File - Name="Game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Game - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "Game.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Game.mak" CFG="Game - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Game - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Game - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/Game", FAAAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /D MMX_OK=$(MMX_OK) /FR /Yu"Game\GamePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(AVGAME)" /D "WIN32" /D "OPENGL" /D MMX_OK=$(MMX_OK) /Yu"Game\GamePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "_DEBUG" /D "OPENGL" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /FR /Yu"Game\GamePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

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
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "OPENGL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(AVGAME)" /D "WIN32" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /Yu"Game\GamePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PS2Direct_Release"
# PROP BASE Intermediate_Dir "PS2Direct_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2Direct_Release"
# PROP Intermediate_Dir "PS2Direct_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "OPENGL" /D "PS2" /D "CW" /YX /FD /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "CW" /Yu"Game\GamePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

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
# ADD BASE CPP /nologo /w /W0 /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "OPENGL" /D "PS2" /D "_DEBUG" /D "CW" /FR /YX /FD /Zs /c
# ADD CPP /nologo /w /W0 /Zi /Od /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "CW" /Yu"Game\GamePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CDRom_Release"
# PROP BASE Intermediate_Dir "CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_CDRom_Release"
# PROP Intermediate_Dir "PS2_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "OPENGL" /D "PS2" /D "CW" /YX /FD /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /Yu"Game\GamePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Game___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "Game___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "_DEBUG" /D "OPENGL" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Game\GamePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Game___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "Game___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "OPENGL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Game\GamePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Game___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "Game___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Game\GamePCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Game\GamePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Game___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "Game___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Game___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "Game___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Game\GamePCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Game\GamePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ENDIF

# Begin Target

# Name "Game - Win32 OpenGL Debug"
# Name "Game - Win32 OpenGL Release"
# Name "Game - Win32 DirectX Debug"
# Name "Game - Win32 DirectX Release"
# Name "Game - PS2Direct Release"
# Name "Game - PS2Direct Debug"
# Name "Game - PS2 CDRom Release"
# Name "Game - Win32 PS2 EE Release"
# Name "Game - Win32 PS2 EE Debug"
# Name "Game - GameCube Debug"
# Name "Game - GameCube Release"
# Name "Game - GameCube CDROM"
# Name "Game - Win32 PS2 EE CDRom Release"
# Name "Game - Win32 GameCube Release DVDETH"
# Begin Group "Database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Database\anm_load.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Database\ChunkLoaders.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Database\DataHelpers.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Database\DataHelpers.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Database\DataMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Database\DataMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Database\DataTypes.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Database\DBFile.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\DBFile.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\database\dynamicobject.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\EnviroInstance.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\Instance.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\Instance.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\lightset.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\lightset.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\LOD.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshInstance.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshInstance.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshList.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshList.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshLOD.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshLOD.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshParam.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshParam.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\MeshSOM.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\mot_load.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\SceneNode.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\SceneNode.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\Som.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\som_load.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\UberFiles.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\VisRegion.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\VisRegion.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\World.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Database\world_load.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "Managers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Managers\AttachMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\AttachMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\AuraMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\AuraMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\DistortionMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\DistortionMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\idmanager.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\IdManager.h
# End Source File
# Begin Source File

SOURCE=.\Managers\LightMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\LightMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\MiscPools.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\MiscPools.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\NameMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\NameMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\ParticleDB.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\ParticleMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\ParticleMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\ReflectionMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\ReflectionMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\RenderMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\RenderMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\managers\rendermgr.hpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\replayMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\replayMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\SceneMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\SceneMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\ShadowMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\ShadowMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\SpriteMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\SpriteMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\SwooshMgr.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Managers\SwooshMgr.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "Animation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Animation\Animproc.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\Animproc.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\ApplyKFM.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\ApplyKFM.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\bone.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\bone.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\bonegroup.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\BoneGroup.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\MotCtrl.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\motion.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\Motion.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\motionchannel.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\MotionChannel.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\MotionDB.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\MotnFlags.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\ScriptCallback.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\ScriptDebug.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Animation\ScriptDebug.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Misc\Actor.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\Actor.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\Bounds.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\Bounds.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\FastVars.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\FastVars.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\fontinfo.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\FontInfo.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\Frustum.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\Frustum.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\GameEngine.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\GameEngine.h

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GamePCH.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

# ADD BASE CPP /Yc"Game\GamePCH.h"
# ADD CPP /Yc"Game\GamePCH.h"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Game\GamePCH.h"
# ADD CPP /Yc"Game\GamePCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GamePCH.h
# End Source File
# Begin Source File

SOURCE=.\GamePCH_SN.cpp

!IF  "$(CFG)" == "Game - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "Game - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Game - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "Game - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
