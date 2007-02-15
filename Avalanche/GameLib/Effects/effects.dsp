# Microsoft Developer Studio Project File - Name="effects" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=effects - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "effects.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "effects.mak" CFG="effects - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "effects - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "effects - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/effects", YOKAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /D "OPENGL" /Yu"Effects\EffectsPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /D "OPENGL" /FR /Yu"Effects\EffectsPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

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
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /debug -Wa,-sn -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Effects\EffectsPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "DIRECTX" /Yu"Effects\EffectsPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "_DEBUG" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "DIRECTX" /FR /Yu"Effects\EffectsPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Effects\EffectsPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

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
# ADD CPP /nologo /w /W0 /Gm /Zi /Od /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Effects\EffectsPCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "effects___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "effects___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Effects\EffectsPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "effects___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "effects___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Effects\EffectsPCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "effects___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "effects___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"Effects\EffectsPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Effects\EffectsPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "effects___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "effects___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "effects___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "effects___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Effects\EffectsPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Effects\EffectsPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ENDIF

# Begin Target

# Name "effects - Win32 OpenGL Release"
# Name "effects - Win32 OpenGL Debug"
# Name "effects - Win32 PS2 EE Release"
# Name "effects - Win32 PS2 EE Debug"
# Name "effects - PS2 CDRom Release"
# Name "effects - Win32 DirectX Release"
# Name "effects - Win32 DirectX Debug"
# Name "effects - PS2Direct Release"
# Name "effects - PS2Direct Debug"
# Name "effects - GameCube Release"
# Name "effects - GameCube Debug"
# Name "effects - GameCube CDROM"
# Name "effects - Win32 PS2 EE CDRom Release"
# Name "effects - Win32 GameCube Release DVDETH"
# Begin Group "Water"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Water\Water.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Water\Water.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Water\Water.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Water\WaterAffector.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Water\WaterCollide.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Water\WaterEquation.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Water\WaterEquation.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Water\WaterGrid.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterMgr.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterMgr.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterPhysics.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterPool.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterPool.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\Water\WaterPS2-2.vsm"

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2_EE_Release
InputPath=".\Water\WaterPS2-2.vsm"
InputName=WaterPS2-2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2_EE_Debug
InputPath=".\Water\WaterPS2-2.vsm"
InputName=WaterPS2-2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2_CDRom_Release
InputPath=".\Water\WaterPS2-2.vsm"
InputName=WaterPS2-2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2Direct_Release
InputPath=".\Water\WaterPS2-2.vsm"
InputName=WaterPS2-2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2Direct_Debug
InputPath=".\Water\WaterPS2-2.vsm"
InputName=WaterPS2-2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2_EE_CDRom_Release
InputPath=".\Water\WaterPS2-2.vsm"
InputName=WaterPS2-2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterPS2.vsm

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2_EE_Release
InputPath=.\Water\WaterPS2.vsm
InputName=WaterPS2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2_EE_Debug
InputPath=.\Water\WaterPS2.vsm
InputName=WaterPS2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2_CDRom_Release
InputPath=.\Water\WaterPS2.vsm
InputName=WaterPS2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2Direct_Release
InputPath=.\Water\WaterPS2.vsm
InputName=WaterPS2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2Direct_Debug
InputPath=.\Water\WaterPS2.vsm
InputName=WaterPS2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\Water
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\Water\WaterPS2.vsm
InputName=WaterPS2

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterRender.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterRenderPS2.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterSprite.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterSprite.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterSpriteEffect.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterSpriteEffect.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterSubdivision.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water\WaterUpdate.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ahfmgr.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ahfmgr.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ahfnode.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ahfnode.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\anihf.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\anihf.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\canihf.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\canihf.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CollisionFXHandler.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CollisionFXHandlerList.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CollisionFXHandlerList.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collisionparticle.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\collisionparticle.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dropshad.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dropshad.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EffectsPCH.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

# ADD BASE CPP /Yc"Effects\EffectsPCH.h"
# ADD CPP /Yc"Effects\EffectsPCH.h"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Effects\EffectsPCH.h"
# ADD CPP /Yc"Effects\EffectsPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EffectsPCH.h
# End Source File
# Begin Source File

SOURCE=.\EffectsPCH_SN.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fastnoise.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fastnoise.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fastnoise.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\footstep.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\footstep.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fullscreendistort.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fullscreendistort.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\halo.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\halo.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\haloauto.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\haloele.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\haloele.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\leaf.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\leaf.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\leafblow.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\leafblow.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\leafblow.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lensele.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lensele.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lensflare.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lensflare.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\motionblur.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\motionblur.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pnoise.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pnoise.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pnoise.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\siggen.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\siggen.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\siggen.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\simplerb.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\simplerb.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\simplerb.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\splinefog.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\splinefog.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sseffect.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sseffect.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sseffect.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ssemgr.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ssemgr.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttconst.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttemit.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttemit.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttemit.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttrack.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttrack.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttrack.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttread.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttread.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttread.hpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttsys.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttsys.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wlight.cpp

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wlight.h

!IF  "$(CFG)" == "effects - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "effects - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "effects - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Release"

!ELSEIF  "$(CFG)" == "effects - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube Release"

!ELSEIF  "$(CFG)" == "effects - GameCube Debug"

!ELSEIF  "$(CFG)" == "effects - GameCube CDROM"

!ELSEIF  "$(CFG)" == "effects - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "effects - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
