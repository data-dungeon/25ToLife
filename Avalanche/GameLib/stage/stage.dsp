# Microsoft Developer Studio Project File - Name="stage" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stage - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "stage.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "stage.mak" CFG="stage - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "stage - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stage - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/stage", FNKAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir " WGL_Release"
# PROP BASE Intermediate_Dir " WGL_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WGL_Release"
# PROP Intermediate_Dir "WGL_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "OPENGL" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"Stage\StagePCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "OPENGL" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"Stage\StagePCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

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
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

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
# ADD BASE CPP /nologo /W3 /GX /ZI /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "PS2" /D "_DEBUG" /D "OPENGL" /YX /FD /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Stage\StagePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"Stage\StagePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "DIRECTX" /D "_DEBUG" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /Yu"Stage\StagePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

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
# ADD BASE CPP /nologo /MT /w /W0 /GX /O2 /X /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /FR /Zs /c
# ADD CPP /nologo /MT /w /W0 /GX /Zi /O2 /X /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Stage\StagePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

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
# ADD BASE CPP /nologo /MTd /w /W0 /Gm /GX /ZI /Od /X /I "$(AVGAME)" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /FR /Zs /GZ /c
# ADD CPP /nologo /MTd /w /W0 /Gm /GX /Zi /Od /X /I "$(AVGAME)" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Stage\StagePCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "stage___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "stage___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Stage\StagePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "stage___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "stage___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Stage\StagePCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "stage___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "stage___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"Stage\StagePCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Stage\StagePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "stage___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "stage___Win32_PS2_EE_CDRom_Release"
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
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "stage___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "stage___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Stage\StagePCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Stage\StagePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ENDIF

# Begin Target

# Name "stage - Win32 OpenGL Release"
# Name "stage - Win32 OpenGL Debug"
# Name "stage - Win32 PS2 EE Release"
# Name "stage - Win32 PS2 EE Debug"
# Name "stage - PS2 CDRom Release"
# Name "stage - Win32 DirectX Release"
# Name "stage - Win32 DirectX Debug"
# Name "stage - PS2Direct Release"
# Name "stage - PS2Direct Debug"
# Name "stage - GameCube Release"
# Name "stage - GameCube Debug"
# Name "stage - GameCube CDROM"
# Name "stage - Win32 PS2 EE CDRom Release"
# Name "stage - Win32 GameCube Release DVDETH"
# Begin Source File

SOURCE=.\AnimatedChain.cpp
DEP_CPP_ANIMA=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AnimatedChain.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BasicCastMemberPool.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cactor.cpp
DEP_CPP_CACTO=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\common\texturestate.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\PS2MaterialInfo.h"\
	"..\..\enginenew\display\PS2Renderer\PS2Renderer.h"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\IdManager.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Managers\ShadowMgr.h"\
	"..\..\enginenew\game\Managers\SpriteMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\NameLibrary.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\hud\animator.h"\
	"..\hud\callback.h"\
	"..\hud\font.h"\
	"..\hud\hud.h"\
	"..\hud\newmenu.h"\
	"..\hud\node.h"\
	"..\hud\sprite.h"\
	"..\hud\stringex.h"\
	"..\hud\text.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cactor.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CActorSoundEmitter.cpp
DEP_CPP_CACTOR=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\sfx.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CActorSoundEmitter.h
# End Source File
# Begin Source File

SOURCE=.\cast.cpp
DEP_CPP_CAST_=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cast.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cast.hpp

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\castmember.cpp
DEP_CPP_CASTM=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\castmember.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CastMemberPool.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CastProfile.cpp
DEP_CPP_CASTP=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CastProfile.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CategoryBit.h
# End Source File
# Begin Source File

SOURCE=.\celist.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\celist.hpp

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Chain.cpp
DEP_CPP_CHAIN=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Chain.h
# End Source File
# Begin Source File

SOURCE=.\Chain.hpp
# End Source File
# Begin Source File

SOURCE=.\cluster.cpp
DEP_CPP_CLUST=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cluster.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\clustere.cpp
DEP_CPP_CLUSTE=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\clustere.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\clustere.hpp

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ClusterElementPool.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\clustersys.cpp
DEP_CPP_CLUSTER=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataMgr.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\FastVars.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\Effects\dropshad.h"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\Environ\Simple.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\lod.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\clustersys.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\clustersys.hpp

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CollisionEntity.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CollisionManager.cpp
DEP_CPP_COLLI=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CollisionManager.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DynamicChain.cpp
DEP_CPP_DYNAM=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\ObjectList.h"\
	"..\CollideHelper\ObjectList.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\ParamMgr.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DynamicChain.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DynamicChain.hpp

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EntityCategory.cpp

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EntityCategory.h
# End Source File
# Begin Source File

SOURCE=.\EntityType.cpp

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EntityType.h
# End Source File
# Begin Source File

SOURCE=.\EntityType.hpp
# End Source File
# Begin Source File

SOURCE=.\InteractionCategory.h
# End Source File
# Begin Source File

SOURCE=.\InteractionCategory.hpp
# End Source File
# Begin Source File

SOURCE=.\MacroChain.cpp
DEP_CPP_MACRO=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MacroChain.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\protagonist.cpp
DEP_CPP_PROTA=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\AabbEntity.h"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\ObjectList.h"\
	"..\CollideHelper\ObjectList.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\protagonist.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\record.cpp
DEP_CPP_RECOR=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\record.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\scatter.cpp
DEP_CPP_SCATT=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataMgr.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\FastVars.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\Effects\dropshad.h"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\Environ\Simple.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\lod.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\scatter.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stage.cpp
DEP_CPP_STAGE=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stage.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StageEntity.cpp
DEP_CPP_STAGEE=\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\platform\basetype.h"\
	".\CategoryBit.h"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StageEntity.h
# End Source File
# Begin Source File

SOURCE=.\StageEntity.hpp
# End Source File
# Begin Source File

SOURCE=.\StagePCH.cpp
DEP_CPP_STAGEP=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

# ADD BASE CPP /Yc"Stage\StagePCH.h"
# ADD CPP /Yc"Stage\StagePCH.h"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Stage\StagePCH.h"
# ADD CPP /Yc"Stage\StagePCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StagePCH.h
# End Source File
# Begin Source File

SOURCE=.\StagePCH_SN.cpp

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StageProfile.cpp
DEP_CPP_STAGEPR=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StageProfile.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SurfaceLookup.cpp
DEP_CPP_SURFA=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SurfaceLookup.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SurfaceType.cpp
DEP_CPP_SURFAC=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SurfaceType.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TerrainEntity.cpp
DEP_CPP_TERRA=\
	"..\..\enginenew\Audio\audiolib.h"\
	"..\..\enginenew\Audio\audiosys.h"\
	"..\..\enginenew\Audio\emitter\e3dparam.h"\
	"..\..\enginenew\Audio\emitter\semit2d.h"\
	"..\..\enginenew\Audio\emitter\semit3d.h"\
	"..\..\enginenew\Audio\emitter\semits.h"\
	"..\..\enginenew\Audio\emitter\semitter.h"\
	"..\..\enginenew\Audio\environment\ambient.h"\
	"..\..\enginenew\Audio\environment\ambsound.h"\
	"..\..\enginenew\Audio\environment\envdbl.h"\
	"..\..\enginenew\Audio\environment\envfield.h"\
	"..\..\enginenew\Audio\environment\envsource.h"\
	"..\..\enginenew\Audio\environment\envstrig.h"\
	"..\..\enginenew\Audio\environment\semitenv.h"\
	"..\..\enginenew\Audio\function\function.h"\
	"..\..\enginenew\Audio\function\ramp.h"\
	"..\..\enginenew\Audio\gcn\dspadpcm.h"\
	"..\..\enginenew\Audio\GCN\sdriver.h"\
	"..\..\enginenew\Audio\gcn\sound.h"\
	"..\..\enginenew\Audio\gcn\sounderror.h"\
	"..\..\enginenew\Audio\gcn\soundstream.h"\
	"..\..\enginenew\Audio\group\reclaim.h"\
	"..\..\enginenew\Audio\group\schan2d.h"\
	"..\..\enginenew\Audio\group\schan3d.h"\
	"..\..\enginenew\Audio\group\schannel.h"\
	"..\..\enginenew\Audio\group\sgalias.h"\
	"..\..\enginenew\Audio\group\sgroup.h"\
	"..\..\enginenew\Audio\group\sgroup2d.h"\
	"..\..\enginenew\Audio\group\sgroup3d.h"\
	"..\..\enginenew\Audio\group\shandle.h"\
	"..\..\enginenew\Audio\jukebox.h"\
	"..\..\enginenew\Audio\listener.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
	"..\..\enginenew\Audio\PS2\streamirx.h"\
	"..\..\enginenew\Audio\PS2\vag.h"\
	"..\..\enginenew\Audio\soundattr.h"\
	"..\..\enginenew\Audio\soundbank.h"\
	"..\..\enginenew\Audio\sounddriver.h"\
	"..\..\enginenew\Audio\streamq.h"\
	"..\..\enginenew\Audio\voiceq.h"\
	"..\..\enginenew\Audio\Win32\sdriver.h"\
	"..\..\enginenew\Audio\Win32\sdrivercaps.h"\
	"..\..\enginenew\Audio\Win32\sdriverenum.h"\
	"..\..\enginenew\Audio\win32\sound.h"\
	"..\..\enginenew\Audio\win32\sounderror.h"\
	"..\..\enginenew\Audio\win32\soundstream.h"\
	"..\..\enginenew\Audio\Win32\wavefile.h"\
	"..\..\enginenew\audio\xbox\sdriver.h"\
	"..\..\enginenew\audio\xbox\sdrivercaps.h"\
	"..\..\enginenew\audio\xbox\sound.h"\
	"..\..\enginenew\audio\xbox\sounderror.h"\
	"..\..\enginenew\audio\xbox\soundstream.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\patchdisp.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\game\Animation\Animproc.h"\
	"..\..\enginenew\game\Animation\bone.h"\
	"..\..\enginenew\game\Animation\BoneGroup.h"\
	"..\..\enginenew\game\Animation\Motion.h"\
	"..\..\enginenew\game\Animation\MotionChannel.h"\
	"..\..\enginenew\game\Animation\MotionDB.h"\
	"..\..\enginenew\game\Animation\MotnFlags.h"\
	"..\..\enginenew\game\Database\DataHelpers.h"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\database\dynamicobject.h"\
	"..\..\enginenew\game\Database\Instance.h"\
	"..\..\enginenew\game\Database\lightset.h"\
	"..\..\enginenew\game\Database\LOD.h"\
	"..\..\enginenew\game\Database\MeshList.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Database\World.h"\
	"..\..\enginenew\game\Managers\AttachMgr.h"\
	"..\..\enginenew\game\Managers\DistortionMgr.h"\
	"..\..\enginenew\game\Managers\replayMgr.h"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Actor.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vector.hpp"\
	"..\..\enginenew\Math\Mat3x3.h"\
	"..\..\enginenew\Math\Mat4x4.h"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\MathStats.h"\
	"..\..\enginenew\Math\MathStruct.h"\
	"..\..\enginenew\Math\MathUndeprecate.h"\
	"..\..\enginenew\Math\Matrix.h"\
	"..\..\enginenew\Math\PS2\Mat4x4.hpp"\
	"..\..\enginenew\Math\PS2\Mat4x4sn.hpp"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\PS2\Quaternsn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\Vector3GCN.hpp"\
	"..\..\enginenew\Math\Vector3Generic.hpp"\
	"..\..\enginenew\Math\Vector3GenericPacked.h"\
	"..\..\enginenew\Math\Vector3GenericPadded.h"\
	"..\..\enginenew\Math\Vector3SNPS2.hpp"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.h"\
	"..\..\enginenew\Math\Vector3SNPS2Padded.hpp"\
	"..\..\enginenew\Math\Vector3Template.h"\
	"..\..\enginenew\Math\Vector3Template.hpp"\
	"..\..\enginenew\Math\Vector3WIN32.hpp"\
	"..\..\enginenew\Math\Vector4GCN.hpp"\
	"..\..\enginenew\Math\Vector4Generic.hpp"\
	"..\..\enginenew\Math\Vector4GenericAligned.h"\
	"..\..\enginenew\Math\Vector4GenericPacked.h"\
	"..\..\enginenew\Math\Vector4SNPS2.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.h"\
	"..\..\enginenew\Math\Vector4SNPS2Aligned.hpp"\
	"..\..\enginenew\Math\Vector4SNPS2X.h"\
	"..\..\enginenew\Math\Vector4SNPS2X.hpp"\
	"..\..\enginenew\Math\Vector4Template.h"\
	"..\..\enginenew\Math\Vector4Template.hpp"\
	"..\..\enginenew\Math\Vector4WIN32.hpp"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\AabbObject.h"\
	"..\collide\AabbObject.hpp"\
	"..\collide\AabbToAabb.h"\
	"..\collide\AabbToAabb.hpp"\
	"..\collide\AabbToObb.h"\
	"..\collide\AabbToObb.hpp"\
	"..\collide\AabbToTerrain.h"\
	"..\collide\AabbToTerrain.hpp"\
	"..\collide\cdbhdr.h"\
	"..\collide\collsys.h"\
	"..\Collide\collsys.hpp"\
	"..\collide\cube.h"\
	"..\collide\cube.hpp"\
	"..\collide\Cull.h"\
	"..\Collide\DynamicEntities.h"\
	"..\Collide\DynamicEntities.hpp"\
	"..\Collide\EntityList.h"\
	"..\Collide\EntityList.hpp"\
	"..\Collide\Event.h"\
	"..\Collide\Event.hpp"\
	"..\collide\Intersection.h"\
	"..\collide\Intersection.hpp"\
	"..\collide\Line.h"\
	"..\collide\Line.hpp"\
	"..\collide\LineToAabb.h"\
	"..\collide\LineToAabb.hpp"\
	"..\collide\LineToObb.h"\
	"..\collide\LineToObb.hpp"\
	"..\collide\LineToSphere.h"\
	"..\collide\LineToSphere.hpp"\
	"..\collide\LineToTerrain.h"\
	"..\collide\LineToTerrain.hpp"\
	"..\Collide\List.h"\
	"..\Collide\List.hpp"\
	"..\Collide\mrm.h"\
	"..\Collide\mrm.hpp"\
	"..\Collide\mrmnode.h"\
	"..\Collide\mrmnode.hpp"\
	"..\collide\Node.h"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\ObbToObb.h"\
	"..\collide\ObbToObb.hpp"\
	"..\collide\ObbToTerrain.h"\
	"..\collide\ObbToTerrain.hpp"\
	"..\collide\Object.h"\
	"..\Collide\ObjectLayer.h"\
	"..\Collide\ObjectQT.h"\
	"..\collide\octtree.h"\
	"..\collide\octtree.hpp"\
	"..\collide\otnode.h"\
	"..\collide\otnode.hpp"\
	"..\collide\ottestid.h"\
	"..\collide\ottestid.hpp"\
	"..\collide\Pair.h"\
	"..\collide\Pair.hpp"\
	"..\Collide\quadtree.h"\
	"..\Collide\QueryEntity.h"\
	"..\Collide\QueryEntity.hpp"\
	"..\collide\Sphere.h"\
	"..\collide\Sphere.hpp"\
	"..\collide\SphereToObb.h"\
	"..\collide\SphereToObb.hpp"\
	"..\collide\SphereToTerrain.h"\
	"..\collide\SphereToTerrain.hpp"\
	"..\collide\Stack.h"\
	"..\collide\Stack.hpp"\
	"..\collide\StackArray.h"\
	"..\collide\StackArray.hpp"\
	"..\Collide\StaticEntity.h"\
	"..\Collide\StaticEntity.hpp"\
	"..\collide\tclayer.h"\
	"..\collide\tclayer.hpp"\
	"..\collide\tcllist.h"\
	"..\collide\tcllist.hpp"\
	"..\collide\Terrain.h"\
	"..\collide\Terrain.hpp"\
	"..\Collide\TerrainEntity.h"\
	"..\collide\terraintri.h"\
	"..\collide\terraintri.hpp"\
	"..\collide\TriangleList.h"\
	"..\collide\TriangleList.hpp"\
	"..\collide\tritest.h"\
	"..\collide\tritest.hpp"\
	"..\collide\tsuspect.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
	"..\CollideHelper\obbchild.h"\
	"..\CollideHelper\obbchild.hpp"\
	"..\CollideHelper\zone.h"\
	"..\CollideHelper\zonemap.h"\
	"..\container\Darray.h"\
	"..\container\darray.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\container\dynlist.h"\
	"..\container\dynlist.hpp"\
	"..\container\fpool.h"\
	"..\container\fpool.hpp"\
	"..\container\nllist.h"\
	"..\container\nllist.hpp"\
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\container\u24.h"\
	"..\container\u24.hpp"\
	"..\enginehelper\cinst.h"\
	"..\enginehelper\cinst.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\hog.h"\
	"..\gamehelper\event.h"\
	"..\gamehelper\loadsom.h"\
	"..\gamehelper\TableLabel.h"\
	"..\gamehelper\TableLookup.h"\
	"..\gamehelper\TableToken.h"\
	"..\geomutil\planefit.h"\
	"..\linearalgebra\cquat.h"\
	"..\linearalgebra\cquat.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\linearalgebra\semiseg.h"\
	"..\linearAlgebra\semiseg.hpp"\
	"..\mathutil\mathcons.h"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\AnimatedChain.h"\
	".\BasicCastMemberPool.h"\
	".\cactor.h"\
	".\CActorSoundEmitter.h"\
	".\Cast.h"\
	".\cast.hpp"\
	".\CastMember.h"\
	".\CastMemberPool.h"\
	".\CastProfile.h"\
	".\CategoryBit.h"\
	".\celist.h"\
	".\celist.hpp"\
	".\Chain.h"\
	".\Chain.hpp"\
	".\cluster.h"\
	".\clustere.h"\
	".\clustere.hpp"\
	".\ClusterElementPool.h"\
	".\clustersys.h"\
	".\clustersys.hpp"\
	".\CollisionEntity.h"\
	".\CollisionManager.h"\
	".\DynamicChain.h"\
	".\DynamicChain.hpp"\
	".\EntityCategory.h"\
	".\EntityType.h"\
	".\EntityType.hpp"\
	".\InteractionCategory.h"\
	".\InteractionCategory.hpp"\
	".\MacroChain.h"\
	".\protagonist.h"\
	".\record.h"\
	".\scatter.h"\
	".\stage.h"\
	".\StageEntity.h"\
	".\StageEntity.hpp"\
	".\StagePCH.h"\
	".\StageProfile.h"\
	".\SurfaceLookup.h"\
	".\SurfaceType.h"\
	".\TerrainEntity.h"\
	{$(INCLUDE)}"d3d8.h"\
	{$(INCLUDE)}"d3d8caps.h"\
	{$(INCLUDE)}"d3d8types.h"\
	{$(INCLUDE)}"d3dx8.h"\
	{$(INCLUDE)}"d3dx8core.h"\
	{$(INCLUDE)}"d3dx8effect.h"\
	{$(INCLUDE)}"d3dx8math.h"\
	{$(INCLUDE)}"d3dx8math.inl"\
	{$(INCLUDE)}"d3dx8mesh.h"\
	{$(INCLUDE)}"d3dx8shape.h"\
	{$(INCLUDE)}"d3dx8tex.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
	{$(INCLUDE)}"dolphin\axfx.h"\
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\gx.h"\
	{$(INCLUDE)}"dolphin\gx\GXBump.h"\
	{$(INCLUDE)}"dolphin\gx\GXCommandList.h"\
	{$(INCLUDE)}"dolphin\gx\GXCpu2Efb.h"\
	{$(INCLUDE)}"dolphin\gx\GXCull.h"\
	{$(INCLUDE)}"dolphin\gx\GXDispList.h"\
	{$(INCLUDE)}"dolphin\gx\GXDraw.h"\
	{$(INCLUDE)}"dolphin\gx\GXEmVert.h"\
	{$(INCLUDE)}"dolphin\gx\GXEnum.h"\
	{$(INCLUDE)}"dolphin\gx\GXFifo.h"\
	{$(INCLUDE)}"dolphin\gx\GXFrameBuffer.h"\
	{$(INCLUDE)}"dolphin\gx\GXGeometry.h"\
	{$(INCLUDE)}"dolphin\gx\GXGet.h"\
	{$(INCLUDE)}"dolphin\gx\GXLighting.h"\
	{$(INCLUDE)}"dolphin\gx\GXManage.h"\
	{$(INCLUDE)}"dolphin\gx\GXPerf.h"\
	{$(INCLUDE)}"dolphin\gx\GXPixel.h"\
	{$(INCLUDE)}"dolphin\gx\GXStruct.h"\
	{$(INCLUDE)}"dolphin\gx\GXTev.h"\
	{$(INCLUDE)}"dolphin\gx\GXTexture.h"\
	{$(INCLUDE)}"dolphin\gx\GXTransform.h"\
	{$(INCLUDE)}"dolphin\gx\GXVerify.h"\
	{$(INCLUDE)}"dolphin\gx\GXVert.h"\
	{$(INCLUDE)}"dolphin\mix.h"\
	{$(INCLUDE)}"dolphin\mtx.h"\
	{$(INCLUDE)}"dolphin\mtx\GeoTypes.h"\
	{$(INCLUDE)}"dolphin\os.h"\
	{$(INCLUDE)}"dolphin\os\OSAlarm.h"\
	{$(INCLUDE)}"dolphin\os\OSAlloc.h"\
	{$(INCLUDE)}"dolphin\os\OSCache.h"\
	{$(INCLUDE)}"dolphin\os\OSContext.h"\
	{$(INCLUDE)}"dolphin\os\OSDC.h"\
	{$(INCLUDE)}"dolphin\os\OSError.h"\
	{$(INCLUDE)}"dolphin\os\OSException.h"\
	{$(INCLUDE)}"dolphin\os\OSExpansion.h"\
	{$(INCLUDE)}"dolphin\os\OSFastCast.h"\
	{$(INCLUDE)}"dolphin\os\OSFont.h"\
	{$(INCLUDE)}"dolphin\os\OSIC.h"\
	{$(INCLUDE)}"dolphin\os\OSInterrupt.h"\
	{$(INCLUDE)}"dolphin\os\OSL2.h"\
	{$(INCLUDE)}"dolphin\os\OSLC.h"\
	{$(INCLUDE)}"dolphin\os\OSMessage.h"\
	{$(INCLUDE)}"dolphin\os\OSModule.h"\
	{$(INCLUDE)}"dolphin\os\OSMutex.h"\
	{$(INCLUDE)}"dolphin\os\OSReset.h"\
	{$(INCLUDE)}"dolphin\os\OSResetSW.h"\
	{$(INCLUDE)}"dolphin\os\OSSemaphore.h"\
	{$(INCLUDE)}"dolphin\os\OSSerial.h"\
	{$(INCLUDE)}"dolphin\os\OSThread.h"\
	{$(INCLUDE)}"dolphin\pad.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TerrainEntity.h

!IF  "$(CFG)" == "stage - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "stage - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stage - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "stage - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Release"

!ELSEIF  "$(CFG)" == "stage - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube Release"

!ELSEIF  "$(CFG)" == "stage - GameCube Debug"

!ELSEIF  "$(CFG)" == "stage - GameCube CDROM"

!ELSEIF  "$(CFG)" == "stage - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "stage - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
