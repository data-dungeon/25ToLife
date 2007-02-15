# Microsoft Developer Studio Project File - Name="platform" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=platform - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "platform.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "platform.mak" CFG="platform - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "platform - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/platform", ODGAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /Yu"Platform\PlatformPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"Platform\PlatformPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

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

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

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
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Platform\PlatformPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"Platform\PlatformPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "_DEBUG" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /Yu"Platform\PlatformPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Platform\PlatformPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

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
# ADD BASE CPP /nologo /w /W0 /Od /X /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /FR /YX /Zs /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /X /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Platform\PlatformPCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "platform___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "platform___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Platform\PlatformPCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "platform___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "platform___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Platform\PlatformPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "platform___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "platform___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"Platform\PlatformPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Platform\PlatformPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "platform___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "platform___Win32_PS2_EE_CDRom_Release"
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

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "platform___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "platform___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Platform\PlatformPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Platform\PlatformPCH.h" /c
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

# Name "platform - Win32 OpenGL Release"
# Name "platform - Win32 OpenGL Debug"
# Name "platform - Win32 PS2 EE Release"
# Name "platform - Win32 PS2 EE Debug"
# Name "platform - PS2 CDRom Release"
# Name "platform - Win32 DirectX Release"
# Name "platform - Win32 DirectX Debug"
# Name "platform - PS2Direct Release"
# Name "platform - PS2Direct Debug"
# Name "platform - GameCube Debug"
# Name "platform - GameCube Release"
# Name "platform - GameCube CDROM"
# Name "platform - Win32 PS2 EE CDRom Release"
# Name "platform - Win32 GameCube Release DVDETH"
# Begin Group "PS2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2\mwUtils.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\PS2\mwUtils_PS2.c

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\PS2\mwUtils_PS2.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\PS2\PS2app.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1
# ADD BASE CPP /O2
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\PS2\PS2app.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2card.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2card.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2ccd.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2ccd.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2ctrol.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2ctrol.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\ps2demo.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2input.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2input.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Win32\conrcids.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\console.rc

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\icon1.ico

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\resource.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\Winapp.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\Winapp.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\wincard.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\wincard.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\winccd.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\winccd.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\winctrol.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\winctrol.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\wininput.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\wininput.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "GCN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GCN\gcnapp.cpp
DEP_CPP_GCNAP=\
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
	"..\..\enginenew\Audio\PS2\ass.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sdriver.hpp"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
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
	"..\..\enginenew\display\common\displayplatform.h"\
	"..\..\enginenew\display\common\displaystate.h"\
	"..\..\enginenew\display\common\displaystate.hpp"\
	"..\..\enginenew\display\common\gfxfunc.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\GCN\Shader.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\PS2.h"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\FastVars.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\game\Misc\GameEngine.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersState.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\gcn\Vect3.hpp"\
	"..\..\enginenew\Math\gcn\Vect4.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vect3.hpp"\
	"..\..\enginenew\Math\Generic\Vect4.hpp"\
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
	"..\..\enginenew\Math\PS2\Vect3.hpp"\
	"..\..\enginenew\Math\PS2\Vect3sn.hpp"\
	"..\..\enginenew\Math\PS2\Vect4.hpp"\
	"..\..\enginenew\Math\PS2\Vect4sn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vect3.h"\
	"..\..\enginenew\Math\Vect4.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\..\enginenew\Math\win32\Vect3.hpp"\
	"..\..\enginenew\Math\win32\Vect4.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\cmdcon\vcontext.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\Object.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
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
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\log.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\auxtext.h"\
	"..\profile\Win32\auxtext.hpp"\
	"..\profile\Win32\textscrw32.h"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\basetype.h"\
	".\compilercontrol.h"\
	".\ctroller.h"\
	".\game.h"\
	".\gameapp.h"\
	".\gameinit.h"\
	".\gameloop.h"\
	".\GCN\gcnapp.h"\
	".\GCN\gcncard.h"\
	".\GCN\gcnccd.h"\
	".\GCN\gcnctrol.h"\
	".\GCN\gcninput.h"\
	".\GCN\GCNMWPort.h"\
	".\keyctrl.h"\
	".\memcard.h"\
	".\PlatformPCH.h"\
	".\PS2\mwUtils.h"\
	".\PS2\mwUtils_PS2.h"\
	".\PS2\PS2app.h"\
	".\PS2\ps2card.h"\
	".\PS2\ps2ccd.h"\
	".\PS2\ps2ctrol.h"\
	".\ps2\ps2demo.h"\
	".\PS2\ps2input.h"\
	".\systemi.h"\
	".\Win32\Winapp.h"\
	".\Win32\wincard.h"\
	".\Win32\winccd.h"\
	".\Win32\winctrol.h"\
	".\Win32\wininput.h"\
	".\xbox\xboxapp.h"\
	".\xbox\xboxcard.h"\
	".\xbox\xboxccd.h"\
	".\xbox\xboxctrol.h"\
	".\xbox\xboxinput.h"\
	
NODEP_CPP_GCNAP=\
	"..\collide\zone.h"\
	"..\collide\zonemap.h"\
	".\3d8.h"\
	".\3d8caps.h"\
	".\3d8types.h"\
	".\3dx8.h"\
	".\3dx8core.h"\
	".\3dx8effect.h"\
	".\3dx8math.h"\
	".\3dx8math.inl"\
	".\3dx8mesh.h"\
	".\3dx8shape.h"\
	".\3dx8tex.h"\
	".\acTypes.h"\
	".\ekernel.h"\
	".\eregs.h"\
	".\estalt.h"\
	".\estruct.h"\
	".\etypes.h"\
	".\evvu0.h"\
	".\evvu1.h"\
	".\ibcdvd.h"\
	".\ibdma.h"\
	".\ibgraph.h"\
	".\ibmc.h"\
	".\ibpad.h"\
	".\ibpc.h"\
	".\ibscedemo.h"\
	".\ifcmd.h"\
	".\ifdev.h"\
	".\ifrpc.h"\
	".\iles.h"\
	".\in32\glext.h"\
	".\in32\win32.h"\
	".\n_registers.h"\
	".\olphin.h"\
	".\olphin\ai.h"\
	".\olphin\ar.h"\
	".\olphin\arq.h"\
	".\olphin\ax.h"\
	".\olphin\axfx.h"\
	".\olphin\base\PPCArch.h"\
	".\olphin\base\PPCWGPipe.h"\
	".\olphin\card.h"\
	".\olphin\db.h"\
	".\olphin\db\DBInterface.h"\
	".\olphin\dsp.h"\
	".\olphin\dvd.h"\
	".\olphin\gx.h"\
	".\olphin\gx\GXBump.h"\
	".\olphin\gx\GXCommandList.h"\
	".\olphin\gx\GXCpu2Efb.h"\
	".\olphin\gx\GXCull.h"\
	".\olphin\gx\GXDispList.h"\
	".\olphin\gx\GXDraw.h"\
	".\olphin\gx\GXEmVert.h"\
	".\olphin\gx\GXEnum.h"\
	".\olphin\gx\GXFifo.h"\
	".\olphin\gx\GXFrameBuffer.h"\
	".\olphin\gx\GXGeometry.h"\
	".\olphin\gx\GXGet.h"\
	".\olphin\gx\GXLighting.h"\
	".\olphin\gx\GXManage.h"\
	".\olphin\gx\GXPerf.h"\
	".\olphin\gx\GXPixel.h"\
	".\olphin\gx\GXStruct.h"\
	".\olphin\gx\GXTev.h"\
	".\olphin\gx\GXTexture.h"\
	".\olphin\gx\GXTransform.h"\
	".\olphin\gx\GXVerify.h"\
	".\olphin\gx\GXVert.h"\
	".\olphin\mix.h"\
	".\olphin\mtx.h"\
	".\olphin\mtx\GeoTypes.h"\
	".\olphin\os.h"\
	".\olphin\os\OSAlarm.h"\
	".\olphin\os\OSAlloc.h"\
	".\olphin\os\OSCache.h"\
	".\olphin\os\OSContext.h"\
	".\olphin\os\OSDC.h"\
	".\olphin\os\OSError.h"\
	".\olphin\os\OSException.h"\
	".\olphin\os\OSExpansion.h"\
	".\olphin\os\OSFastCast.h"\
	".\olphin\os\OSFont.h"\
	".\olphin\os\OSIC.h"\
	".\olphin\os\OSInterrupt.h"\
	".\olphin\os\OSL2.h"\
	".\olphin\os\OSLC.h"\
	".\olphin\os\OSMessage.h"\
	".\olphin\os\OSModule.h"\
	".\olphin\os\OSMutex.h"\
	".\olphin\os\OSReset.h"\
	".\olphin\os\OSResetSW.h"\
	".\olphin\os\OSSemaphore.h"\
	".\olphin\os\OSSerial.h"\
	".\olphin\os\OSThread.h"\
	".\olphin\pad.h"\
	".\olphin\types.h"\
	".\olphin\vi.h"\
	".\olphin\vi\vifuncs.h"\
	".\olphin\vi\vitypes.h"\
	".\olphin\vm.h"\
	".\olphin\vm\VMMapping.h"\
	".\olphin\vm\VMPageReplacement.h"\
	".\SUtils.h"\
	".\tl.h"\
	".\xerr8.h"\
	".\xfile.h"\
	

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcnapp.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcncard.cpp
DEP_CPP_GCNCA=\
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
	"..\..\enginenew\Audio\PS2\ass.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sdriver.hpp"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
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
	"..\..\enginenew\display\common\displayplatform.h"\
	"..\..\enginenew\display\common\displaystate.h"\
	"..\..\enginenew\display\common\displaystate.hpp"\
	"..\..\enginenew\display\common\gfxfunc.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\GCN\Shader.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\PS2.h"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\FastVars.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\game\Misc\GameEngine.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersState.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\gcn\Vect3.hpp"\
	"..\..\enginenew\Math\gcn\Vect4.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vect3.hpp"\
	"..\..\enginenew\Math\Generic\Vect4.hpp"\
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
	"..\..\enginenew\Math\PS2\Vect3.hpp"\
	"..\..\enginenew\Math\PS2\Vect3sn.hpp"\
	"..\..\enginenew\Math\PS2\Vect4.hpp"\
	"..\..\enginenew\Math\PS2\Vect4sn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vect3.h"\
	"..\..\enginenew\Math\Vect4.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\..\enginenew\Math\win32\Vect3.hpp"\
	"..\..\enginenew\Math\win32\Vect4.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\cmdcon\vcontext.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\Object.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
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
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\filestream.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\log.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\auxtext.h"\
	"..\profile\Win32\auxtext.hpp"\
	"..\profile\Win32\textscrw32.h"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\basetype.h"\
	".\compilercontrol.h"\
	".\ctroller.h"\
	".\game.h"\
	".\gameapp.h"\
	".\gameinit.h"\
	".\gameloop.h"\
	".\GCN\gcnapp.h"\
	".\GCN\gcncard.h"\
	".\GCN\gcnccd.h"\
	".\GCN\gcnctrol.h"\
	".\GCN\gcninput.h"\
	".\GCN\GCNMWPort.h"\
	".\keyctrl.h"\
	".\memcard.h"\
	".\PlatformPCH.h"\
	".\PS2\mwUtils.h"\
	".\PS2\mwUtils_PS2.h"\
	".\PS2\PS2app.h"\
	".\PS2\ps2card.h"\
	".\PS2\ps2ccd.h"\
	".\PS2\ps2ctrol.h"\
	".\ps2\ps2demo.h"\
	".\PS2\ps2input.h"\
	".\systemi.h"\
	".\Win32\Winapp.h"\
	".\Win32\wincard.h"\
	".\Win32\winccd.h"\
	".\Win32\winctrol.h"\
	".\Win32\wininput.h"\
	".\xbox\xboxapp.h"\
	".\xbox\xboxcard.h"\
	".\xbox\xboxccd.h"\
	".\xbox\xboxctrol.h"\
	".\xbox\xboxinput.h"\
	
NODEP_CPP_GCNCA=\
	"..\collide\zone.h"\
	"..\collide\zonemap.h"\
	".\3d8.h"\
	".\3d8caps.h"\
	".\3d8types.h"\
	".\3dx8.h"\
	".\3dx8core.h"\
	".\3dx8effect.h"\
	".\3dx8math.h"\
	".\3dx8math.inl"\
	".\3dx8mesh.h"\
	".\3dx8shape.h"\
	".\3dx8tex.h"\
	".\acTypes.h"\
	".\ekernel.h"\
	".\eregs.h"\
	".\estalt.h"\
	".\estruct.h"\
	".\etypes.h"\
	".\evvu0.h"\
	".\evvu1.h"\
	".\ibcdvd.h"\
	".\ibdma.h"\
	".\ibgraph.h"\
	".\ibmc.h"\
	".\ibpad.h"\
	".\ibpc.h"\
	".\ibscedemo.h"\
	".\ifcmd.h"\
	".\ifdev.h"\
	".\ifrpc.h"\
	".\iles.h"\
	".\in32\glext.h"\
	".\in32\win32.h"\
	".\n_registers.h"\
	".\olphin.h"\
	".\olphin\ai.h"\
	".\olphin\ar.h"\
	".\olphin\arq.h"\
	".\olphin\ax.h"\
	".\olphin\axfx.h"\
	".\olphin\base\PPCArch.h"\
	".\olphin\base\PPCWGPipe.h"\
	".\olphin\card.h"\
	".\olphin\db.h"\
	".\olphin\db\DBInterface.h"\
	".\olphin\dsp.h"\
	".\olphin\dvd.h"\
	".\olphin\gx.h"\
	".\olphin\gx\GXBump.h"\
	".\olphin\gx\GXCommandList.h"\
	".\olphin\gx\GXCpu2Efb.h"\
	".\olphin\gx\GXCull.h"\
	".\olphin\gx\GXDispList.h"\
	".\olphin\gx\GXDraw.h"\
	".\olphin\gx\GXEmVert.h"\
	".\olphin\gx\GXEnum.h"\
	".\olphin\gx\GXFifo.h"\
	".\olphin\gx\GXFrameBuffer.h"\
	".\olphin\gx\GXGeometry.h"\
	".\olphin\gx\GXGet.h"\
	".\olphin\gx\GXLighting.h"\
	".\olphin\gx\GXManage.h"\
	".\olphin\gx\GXPerf.h"\
	".\olphin\gx\GXPixel.h"\
	".\olphin\gx\GXStruct.h"\
	".\olphin\gx\GXTev.h"\
	".\olphin\gx\GXTexture.h"\
	".\olphin\gx\GXTransform.h"\
	".\olphin\gx\GXVerify.h"\
	".\olphin\gx\GXVert.h"\
	".\olphin\mix.h"\
	".\olphin\mtx.h"\
	".\olphin\mtx\GeoTypes.h"\
	".\olphin\os.h"\
	".\olphin\os\OSAlarm.h"\
	".\olphin\os\OSAlloc.h"\
	".\olphin\os\OSCache.h"\
	".\olphin\os\OSContext.h"\
	".\olphin\os\OSDC.h"\
	".\olphin\os\OSError.h"\
	".\olphin\os\OSException.h"\
	".\olphin\os\OSExpansion.h"\
	".\olphin\os\OSFastCast.h"\
	".\olphin\os\OSFont.h"\
	".\olphin\os\OSIC.h"\
	".\olphin\os\OSInterrupt.h"\
	".\olphin\os\OSL2.h"\
	".\olphin\os\OSLC.h"\
	".\olphin\os\OSMessage.h"\
	".\olphin\os\OSModule.h"\
	".\olphin\os\OSMutex.h"\
	".\olphin\os\OSReset.h"\
	".\olphin\os\OSResetSW.h"\
	".\olphin\os\OSSemaphore.h"\
	".\olphin\os\OSSerial.h"\
	".\olphin\os\OSThread.h"\
	".\olphin\pad.h"\
	".\olphin\types.h"\
	".\olphin\vi.h"\
	".\olphin\vi\vifuncs.h"\
	".\olphin\vi\vitypes.h"\
	".\SUtils.h"\
	".\tl.h"\
	".\xerr8.h"\
	".\xfile.h"\
	

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcncard.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcnccd.cpp
DEP_CPP_GCNCC=\
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
	"..\..\enginenew\Audio\PS2\ass.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sdriver.hpp"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
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
	"..\..\enginenew\display\common\displayplatform.h"\
	"..\..\enginenew\display\common\displaystate.h"\
	"..\..\enginenew\display\common\displaystate.hpp"\
	"..\..\enginenew\display\common\gfxfunc.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\GCN\Shader.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\PS2.h"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\FastVars.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\game\Misc\GameEngine.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersState.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\gcn\Vect3.hpp"\
	"..\..\enginenew\Math\gcn\Vect4.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vect3.hpp"\
	"..\..\enginenew\Math\Generic\Vect4.hpp"\
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
	"..\..\enginenew\Math\PS2\Vect3.hpp"\
	"..\..\enginenew\Math\PS2\Vect3sn.hpp"\
	"..\..\enginenew\Math\PS2\Vect4.hpp"\
	"..\..\enginenew\Math\PS2\Vect4sn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vect3.h"\
	"..\..\enginenew\Math\Vect4.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\..\enginenew\Math\win32\Vect3.hpp"\
	"..\..\enginenew\Math\win32\Vect4.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\cmdcon\vcontext.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\Object.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
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
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\log.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\auxtext.h"\
	"..\profile\Win32\auxtext.hpp"\
	"..\profile\Win32\textscrw32.h"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\basetype.h"\
	".\compilercontrol.h"\
	".\ctroller.h"\
	".\game.h"\
	".\gameapp.h"\
	".\gameinit.h"\
	".\gameloop.h"\
	".\GCN\gcnapp.h"\
	".\GCN\gcncard.h"\
	".\GCN\gcnccd.h"\
	".\GCN\gcnctrol.h"\
	".\GCN\gcninput.h"\
	".\GCN\GCNMWPort.h"\
	".\keyctrl.h"\
	".\memcard.h"\
	".\PlatformPCH.h"\
	".\PS2\mwUtils.h"\
	".\PS2\mwUtils_PS2.h"\
	".\PS2\PS2app.h"\
	".\PS2\ps2card.h"\
	".\PS2\ps2ccd.h"\
	".\PS2\ps2ctrol.h"\
	".\ps2\ps2demo.h"\
	".\PS2\ps2input.h"\
	".\systemi.h"\
	".\Win32\Winapp.h"\
	".\Win32\wincard.h"\
	".\Win32\winccd.h"\
	".\Win32\winctrol.h"\
	".\Win32\wininput.h"\
	".\xbox\xboxapp.h"\
	".\xbox\xboxcard.h"\
	".\xbox\xboxccd.h"\
	".\xbox\xboxctrol.h"\
	".\xbox\xboxinput.h"\
	
NODEP_CPP_GCNCC=\
	"..\collide\zone.h"\
	"..\collide\zonemap.h"\
	".\3d8.h"\
	".\3d8caps.h"\
	".\3d8types.h"\
	".\3dx8.h"\
	".\3dx8core.h"\
	".\3dx8effect.h"\
	".\3dx8math.h"\
	".\3dx8math.inl"\
	".\3dx8mesh.h"\
	".\3dx8shape.h"\
	".\3dx8tex.h"\
	".\acTypes.h"\
	".\ekernel.h"\
	".\eregs.h"\
	".\estalt.h"\
	".\estruct.h"\
	".\etypes.h"\
	".\evvu0.h"\
	".\evvu1.h"\
	".\ibcdvd.h"\
	".\ibdma.h"\
	".\ibgraph.h"\
	".\ibmc.h"\
	".\ibpad.h"\
	".\ibpc.h"\
	".\ibscedemo.h"\
	".\ifcmd.h"\
	".\ifdev.h"\
	".\ifrpc.h"\
	".\iles.h"\
	".\in32\glext.h"\
	".\in32\win32.h"\
	".\n_registers.h"\
	".\olphin.h"\
	".\olphin\ai.h"\
	".\olphin\ar.h"\
	".\olphin\arq.h"\
	".\olphin\ax.h"\
	".\olphin\axfx.h"\
	".\olphin\base\PPCArch.h"\
	".\olphin\base\PPCWGPipe.h"\
	".\olphin\card.h"\
	".\olphin\db.h"\
	".\olphin\db\DBInterface.h"\
	".\olphin\dsp.h"\
	".\olphin\dvd.h"\
	".\olphin\gx.h"\
	".\olphin\gx\GXBump.h"\
	".\olphin\gx\GXCommandList.h"\
	".\olphin\gx\GXCpu2Efb.h"\
	".\olphin\gx\GXCull.h"\
	".\olphin\gx\GXDispList.h"\
	".\olphin\gx\GXDraw.h"\
	".\olphin\gx\GXEmVert.h"\
	".\olphin\gx\GXEnum.h"\
	".\olphin\gx\GXFifo.h"\
	".\olphin\gx\GXFrameBuffer.h"\
	".\olphin\gx\GXGeometry.h"\
	".\olphin\gx\GXGet.h"\
	".\olphin\gx\GXLighting.h"\
	".\olphin\gx\GXManage.h"\
	".\olphin\gx\GXPerf.h"\
	".\olphin\gx\GXPixel.h"\
	".\olphin\gx\GXStruct.h"\
	".\olphin\gx\GXTev.h"\
	".\olphin\gx\GXTexture.h"\
	".\olphin\gx\GXTransform.h"\
	".\olphin\gx\GXVerify.h"\
	".\olphin\gx\GXVert.h"\
	".\olphin\mix.h"\
	".\olphin\mtx.h"\
	".\olphin\mtx\GeoTypes.h"\
	".\olphin\os.h"\
	".\olphin\os\OSAlarm.h"\
	".\olphin\os\OSAlloc.h"\
	".\olphin\os\OSCache.h"\
	".\olphin\os\OSContext.h"\
	".\olphin\os\OSDC.h"\
	".\olphin\os\OSError.h"\
	".\olphin\os\OSException.h"\
	".\olphin\os\OSExpansion.h"\
	".\olphin\os\OSFastCast.h"\
	".\olphin\os\OSFont.h"\
	".\olphin\os\OSIC.h"\
	".\olphin\os\OSInterrupt.h"\
	".\olphin\os\OSL2.h"\
	".\olphin\os\OSLC.h"\
	".\olphin\os\OSMessage.h"\
	".\olphin\os\OSModule.h"\
	".\olphin\os\OSMutex.h"\
	".\olphin\os\OSReset.h"\
	".\olphin\os\OSResetSW.h"\
	".\olphin\os\OSSemaphore.h"\
	".\olphin\os\OSSerial.h"\
	".\olphin\os\OSThread.h"\
	".\olphin\pad.h"\
	".\olphin\types.h"\
	".\olphin\vi.h"\
	".\olphin\vi\vifuncs.h"\
	".\olphin\vi\vitypes.h"\
	".\SUtils.h"\
	".\tl.h"\
	".\xerr8.h"\
	".\xfile.h"\
	

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcnccd.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcnctrol.cpp
DEP_CPP_GCNCT=\
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
	"..\..\enginenew\Audio\PS2\ass.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sdriver.hpp"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
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
	"..\..\enginenew\display\common\displayplatform.h"\
	"..\..\enginenew\display\common\displaystate.h"\
	"..\..\enginenew\display\common\displaystate.hpp"\
	"..\..\enginenew\display\common\gfxfunc.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\GCN\Shader.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\PS2.h"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\FastVars.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\game\Misc\GameEngine.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersState.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\gcn\Vect3.hpp"\
	"..\..\enginenew\Math\gcn\Vect4.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vect3.hpp"\
	"..\..\enginenew\Math\Generic\Vect4.hpp"\
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
	"..\..\enginenew\Math\PS2\Vect3.hpp"\
	"..\..\enginenew\Math\PS2\Vect3sn.hpp"\
	"..\..\enginenew\Math\PS2\Vect4.hpp"\
	"..\..\enginenew\Math\PS2\Vect4sn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vect3.h"\
	"..\..\enginenew\Math\Vect4.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\..\enginenew\Math\win32\Vect3.hpp"\
	"..\..\enginenew\Math\win32\Vect4.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\cmdcon\vcontext.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\Object.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
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
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\log.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\auxtext.h"\
	"..\profile\Win32\auxtext.hpp"\
	"..\profile\Win32\textscrw32.h"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\basetype.h"\
	".\compilercontrol.h"\
	".\ctroller.h"\
	".\game.h"\
	".\gameapp.h"\
	".\gameinit.h"\
	".\gameloop.h"\
	".\GCN\gcnapp.h"\
	".\GCN\gcncard.h"\
	".\GCN\gcnccd.h"\
	".\GCN\gcnctrol.h"\
	".\GCN\gcninput.h"\
	".\GCN\GCNMWPort.h"\
	".\keyctrl.h"\
	".\memcard.h"\
	".\PlatformPCH.h"\
	".\PS2\mwUtils.h"\
	".\PS2\mwUtils_PS2.h"\
	".\PS2\PS2app.h"\
	".\PS2\ps2card.h"\
	".\PS2\ps2ccd.h"\
	".\PS2\ps2ctrol.h"\
	".\ps2\ps2demo.h"\
	".\PS2\ps2input.h"\
	".\systemi.h"\
	".\Win32\Winapp.h"\
	".\Win32\wincard.h"\
	".\Win32\winccd.h"\
	".\Win32\winctrol.h"\
	".\Win32\wininput.h"\
	".\xbox\xboxapp.h"\
	".\xbox\xboxcard.h"\
	".\xbox\xboxccd.h"\
	".\xbox\xboxctrol.h"\
	".\xbox\xboxinput.h"\
	
NODEP_CPP_GCNCT=\
	"..\collide\zone.h"\
	"..\collide\zonemap.h"\
	".\3d8.h"\
	".\3d8caps.h"\
	".\3d8types.h"\
	".\3dx8.h"\
	".\3dx8core.h"\
	".\3dx8effect.h"\
	".\3dx8math.h"\
	".\3dx8math.inl"\
	".\3dx8mesh.h"\
	".\3dx8shape.h"\
	".\3dx8tex.h"\
	".\acTypes.h"\
	".\ekernel.h"\
	".\eregs.h"\
	".\estalt.h"\
	".\estruct.h"\
	".\etypes.h"\
	".\evvu0.h"\
	".\evvu1.h"\
	".\ibcdvd.h"\
	".\ibdma.h"\
	".\ibgraph.h"\
	".\ibmc.h"\
	".\ibpad.h"\
	".\ibpc.h"\
	".\ibscedemo.h"\
	".\ifcmd.h"\
	".\ifdev.h"\
	".\ifrpc.h"\
	".\iles.h"\
	".\in32\glext.h"\
	".\in32\win32.h"\
	".\n_registers.h"\
	".\olphin.h"\
	".\olphin\ai.h"\
	".\olphin\ar.h"\
	".\olphin\arq.h"\
	".\olphin\ax.h"\
	".\olphin\axfx.h"\
	".\olphin\base\PPCArch.h"\
	".\olphin\base\PPCWGPipe.h"\
	".\olphin\card.h"\
	".\olphin\db.h"\
	".\olphin\db\DBInterface.h"\
	".\olphin\dsp.h"\
	".\olphin\dvd.h"\
	".\olphin\gx.h"\
	".\olphin\gx\GXBump.h"\
	".\olphin\gx\GXCommandList.h"\
	".\olphin\gx\GXCpu2Efb.h"\
	".\olphin\gx\GXCull.h"\
	".\olphin\gx\GXDispList.h"\
	".\olphin\gx\GXDraw.h"\
	".\olphin\gx\GXEmVert.h"\
	".\olphin\gx\GXEnum.h"\
	".\olphin\gx\GXFifo.h"\
	".\olphin\gx\GXFrameBuffer.h"\
	".\olphin\gx\GXGeometry.h"\
	".\olphin\gx\GXGet.h"\
	".\olphin\gx\GXLighting.h"\
	".\olphin\gx\GXManage.h"\
	".\olphin\gx\GXPerf.h"\
	".\olphin\gx\GXPixel.h"\
	".\olphin\gx\GXStruct.h"\
	".\olphin\gx\GXTev.h"\
	".\olphin\gx\GXTexture.h"\
	".\olphin\gx\GXTransform.h"\
	".\olphin\gx\GXVerify.h"\
	".\olphin\gx\GXVert.h"\
	".\olphin\mix.h"\
	".\olphin\mtx.h"\
	".\olphin\mtx\GeoTypes.h"\
	".\olphin\os.h"\
	".\olphin\os\OSAlarm.h"\
	".\olphin\os\OSAlloc.h"\
	".\olphin\os\OSCache.h"\
	".\olphin\os\OSContext.h"\
	".\olphin\os\OSDC.h"\
	".\olphin\os\OSError.h"\
	".\olphin\os\OSException.h"\
	".\olphin\os\OSExpansion.h"\
	".\olphin\os\OSFastCast.h"\
	".\olphin\os\OSFont.h"\
	".\olphin\os\OSIC.h"\
	".\olphin\os\OSInterrupt.h"\
	".\olphin\os\OSL2.h"\
	".\olphin\os\OSLC.h"\
	".\olphin\os\OSMessage.h"\
	".\olphin\os\OSModule.h"\
	".\olphin\os\OSMutex.h"\
	".\olphin\os\OSReset.h"\
	".\olphin\os\OSResetSW.h"\
	".\olphin\os\OSSemaphore.h"\
	".\olphin\os\OSSerial.h"\
	".\olphin\os\OSThread.h"\
	".\olphin\pad.h"\
	".\olphin\types.h"\
	".\olphin\vi.h"\
	".\olphin\vi\vifuncs.h"\
	".\olphin\vi\vitypes.h"\
	".\SUtils.h"\
	".\tl.h"\
	".\xerr8.h"\
	".\xfile.h"\
	

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcnctrol.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcninput.cpp
DEP_CPP_GCNIN=\
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
	"..\..\enginenew\Audio\PS2\ass.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sdriver.hpp"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
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
	"..\..\enginenew\display\common\displayplatform.h"\
	"..\..\enginenew\display\common\displaystate.h"\
	"..\..\enginenew\display\common\displaystate.hpp"\
	"..\..\enginenew\display\common\gfxfunc.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\GCN\Shader.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\PS2.h"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\FastVars.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\game\Misc\GameEngine.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersState.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\gcn\Vect3.hpp"\
	"..\..\enginenew\Math\gcn\Vect4.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vect3.hpp"\
	"..\..\enginenew\Math\Generic\Vect4.hpp"\
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
	"..\..\enginenew\Math\PS2\Vect3.hpp"\
	"..\..\enginenew\Math\PS2\Vect3sn.hpp"\
	"..\..\enginenew\Math\PS2\Vect4.hpp"\
	"..\..\enginenew\Math\PS2\Vect4sn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vect3.h"\
	"..\..\enginenew\Math\Vect4.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\..\enginenew\Math\win32\Vect3.hpp"\
	"..\..\enginenew\Math\win32\Vect4.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\cmdcon\vcontext.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\Object.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
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
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\log.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\auxtext.h"\
	"..\profile\Win32\auxtext.hpp"\
	"..\profile\Win32\textscrw32.h"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\basetype.h"\
	".\compilercontrol.h"\
	".\ctroller.h"\
	".\game.h"\
	".\gameapp.h"\
	".\gameinit.h"\
	".\gameloop.h"\
	".\GCN\gcnapp.h"\
	".\GCN\gcncard.h"\
	".\GCN\gcnccd.h"\
	".\GCN\gcnctrol.h"\
	".\GCN\gcninput.h"\
	".\GCN\GCNMWPort.h"\
	".\keyctrl.h"\
	".\memcard.h"\
	".\PlatformPCH.h"\
	".\PS2\mwUtils.h"\
	".\PS2\mwUtils_PS2.h"\
	".\PS2\PS2app.h"\
	".\PS2\ps2card.h"\
	".\PS2\ps2ccd.h"\
	".\PS2\ps2ctrol.h"\
	".\ps2\ps2demo.h"\
	".\PS2\ps2input.h"\
	".\systemi.h"\
	".\Win32\Winapp.h"\
	".\Win32\wincard.h"\
	".\Win32\winccd.h"\
	".\Win32\winctrol.h"\
	".\Win32\wininput.h"\
	".\xbox\xboxapp.h"\
	".\xbox\xboxcard.h"\
	".\xbox\xboxccd.h"\
	".\xbox\xboxctrol.h"\
	".\xbox\xboxinput.h"\
	
NODEP_CPP_GCNIN=\
	"..\collide\zone.h"\
	"..\collide\zonemap.h"\
	".\3d8.h"\
	".\3d8caps.h"\
	".\3d8types.h"\
	".\3dx8.h"\
	".\3dx8core.h"\
	".\3dx8effect.h"\
	".\3dx8math.h"\
	".\3dx8math.inl"\
	".\3dx8mesh.h"\
	".\3dx8shape.h"\
	".\3dx8tex.h"\
	".\acTypes.h"\
	".\ekernel.h"\
	".\eregs.h"\
	".\estalt.h"\
	".\estruct.h"\
	".\etypes.h"\
	".\evvu0.h"\
	".\evvu1.h"\
	".\ibcdvd.h"\
	".\ibdma.h"\
	".\ibgraph.h"\
	".\ibmc.h"\
	".\ibpad.h"\
	".\ibpc.h"\
	".\ibscedemo.h"\
	".\ifcmd.h"\
	".\ifdev.h"\
	".\ifrpc.h"\
	".\iles.h"\
	".\in32\glext.h"\
	".\in32\win32.h"\
	".\n_registers.h"\
	".\olphin.h"\
	".\olphin\ai.h"\
	".\olphin\ar.h"\
	".\olphin\arq.h"\
	".\olphin\ax.h"\
	".\olphin\axfx.h"\
	".\olphin\base\PPCArch.h"\
	".\olphin\base\PPCWGPipe.h"\
	".\olphin\card.h"\
	".\olphin\db.h"\
	".\olphin\db\DBInterface.h"\
	".\olphin\dsp.h"\
	".\olphin\dvd.h"\
	".\olphin\gx.h"\
	".\olphin\gx\GXBump.h"\
	".\olphin\gx\GXCommandList.h"\
	".\olphin\gx\GXCpu2Efb.h"\
	".\olphin\gx\GXCull.h"\
	".\olphin\gx\GXDispList.h"\
	".\olphin\gx\GXDraw.h"\
	".\olphin\gx\GXEmVert.h"\
	".\olphin\gx\GXEnum.h"\
	".\olphin\gx\GXFifo.h"\
	".\olphin\gx\GXFrameBuffer.h"\
	".\olphin\gx\GXGeometry.h"\
	".\olphin\gx\GXGet.h"\
	".\olphin\gx\GXLighting.h"\
	".\olphin\gx\GXManage.h"\
	".\olphin\gx\GXPerf.h"\
	".\olphin\gx\GXPixel.h"\
	".\olphin\gx\GXStruct.h"\
	".\olphin\gx\GXTev.h"\
	".\olphin\gx\GXTexture.h"\
	".\olphin\gx\GXTransform.h"\
	".\olphin\gx\GXVerify.h"\
	".\olphin\gx\GXVert.h"\
	".\olphin\mix.h"\
	".\olphin\mtx.h"\
	".\olphin\mtx\GeoTypes.h"\
	".\olphin\os.h"\
	".\olphin\os\OSAlarm.h"\
	".\olphin\os\OSAlloc.h"\
	".\olphin\os\OSCache.h"\
	".\olphin\os\OSContext.h"\
	".\olphin\os\OSDC.h"\
	".\olphin\os\OSError.h"\
	".\olphin\os\OSException.h"\
	".\olphin\os\OSExpansion.h"\
	".\olphin\os\OSFastCast.h"\
	".\olphin\os\OSFont.h"\
	".\olphin\os\OSIC.h"\
	".\olphin\os\OSInterrupt.h"\
	".\olphin\os\OSL2.h"\
	".\olphin\os\OSLC.h"\
	".\olphin\os\OSMessage.h"\
	".\olphin\os\OSModule.h"\
	".\olphin\os\OSMutex.h"\
	".\olphin\os\OSReset.h"\
	".\olphin\os\OSResetSW.h"\
	".\olphin\os\OSSemaphore.h"\
	".\olphin\os\OSSerial.h"\
	".\olphin\os\OSThread.h"\
	".\olphin\pad.h"\
	".\olphin\types.h"\
	".\olphin\vi.h"\
	".\olphin\vi\vifuncs.h"\
	".\olphin\vi\vitypes.h"\
	".\SUtils.h"\
	".\tl.h"\
	".\xerr8.h"\
	".\xfile.h"\
	

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\gcninput.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\GCNMWPort.cpp
DEP_CPP_GCNMW=\
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
	"..\..\enginenew\Audio\PS2\ass.h"\
	"..\..\enginenew\Audio\PS2\sdriver.h"\
	"..\..\enginenew\Audio\ps2\sdriver.hpp"\
	"..\..\enginenew\Audio\ps2\sound.h"\
	"..\..\enginenew\Audio\ps2\sounderror.h"\
	"..\..\enginenew\Audio\ps2\soundstream.h"\
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
	"..\..\enginenew\display\common\displayplatform.h"\
	"..\..\enginenew\display\common\displaystate.h"\
	"..\..\enginenew\display\common\displaystate.hpp"\
	"..\..\enginenew\display\common\gfxfunc.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\GCN\Shader.h"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
	"..\..\enginenew\display\PS2Renderer\PS2.h"\
	"..\..\enginenew\display\PS2Renderer\RenderState.hpp"\
	"..\..\enginenew\game\Database\DataTypes.h"\
	"..\..\enginenew\game\Database\DBFile.h"\
	"..\..\enginenew\game\Database\Som.h"\
	"..\..\enginenew\game\Managers\LightMgr.h"\
	"..\..\enginenew\game\Managers\RenderMgr.h"\
	"..\..\enginenew\game\managers\rendermgr.hpp"\
	"..\..\enginenew\game\Managers\SceneMgr.h"\
	"..\..\enginenew\game\Misc\Bounds.h"\
	"..\..\enginenew\game\Misc\FastVars.h"\
	"..\..\enginenew\game\Misc\Frustum.h"\
	"..\..\enginenew\game\Misc\GameEngine.h"\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersState.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
	"..\..\enginenew\layers\Timing.h"\
	"..\..\enginenew\Math\Det.h"\
	"..\..\enginenew\Math\gcn\Mat4x4.hpp"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\gcn\Vect3.hpp"\
	"..\..\enginenew\Math\gcn\Vect4.hpp"\
	"..\..\enginenew\Math\Generic\Det.hpp"\
	"..\..\enginenew\Math\Generic\Mat3x3.hpp"\
	"..\..\enginenew\Math\Generic\Mat4x4.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\Generic\Matrix.hpp"\
	"..\..\enginenew\Math\Generic\Vect2.hpp"\
	"..\..\enginenew\Math\Generic\Vect3.hpp"\
	"..\..\enginenew\Math\Generic\Vect4.hpp"\
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
	"..\..\enginenew\Math\PS2\Vect3.hpp"\
	"..\..\enginenew\Math\PS2\Vect3sn.hpp"\
	"..\..\enginenew\Math\PS2\Vect4.hpp"\
	"..\..\enginenew\Math\PS2\Vect4sn.hpp"\
	"..\..\enginenew\Math\Quatern.h"\
	"..\..\enginenew\Math\Vect2.h"\
	"..\..\enginenew\Math\Vect3.h"\
	"..\..\enginenew\Math\Vect4.h"\
	"..\..\enginenew\Math\Vector.h"\
	"..\..\enginenew\Math\win32\Mat4x4.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\..\enginenew\Math\win32\Vect3.hpp"\
	"..\..\enginenew\Math\win32\Vect4.hpp"\
	"..\cmdcon\ccondisp.h"\
	"..\cmdcon\cmdcon.h"\
	"..\cmdcon\convar.h"\
	"..\cmdcon\vcontext.h"\
	"..\collide\aabb.h"\
	"..\collide\aabb.hpp"\
	"..\collide\obb.h"\
	"..\collide\obb.hpp"\
	"..\collide\Object.h"\
	"..\collide\xzrect.h"\
	"..\collide\xzrect.hpp"\
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
	"..\container\pool.h"\
	"..\container\pool.hpp"\
	"..\enginehelper\drawutil.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\profile\codetimer.h"\
	"..\profile\codetimer.hpp"\
	"..\profile\cputimer.h"\
	"..\profile\ctpage.h"\
	"..\profile\ctpage.hpp"\
	"..\profile\GCN\gcntimer.h"\
	"..\profile\log.h"\
	"..\profile\PS2\ps2timer.h"\
	"..\profile\textpage.h"\
	"..\profile\textrect.h"\
	"..\profile\textrect.hpp"\
	"..\profile\textscr.h"\
	"..\profile\textwin.h"\
	"..\profile\textwin.hpp"\
	"..\profile\Win32\auxtext.h"\
	"..\profile\Win32\auxtext.hpp"\
	"..\profile\Win32\textscrw32.h"\
	"..\profile\Win32\w32timer.h"\
	"..\profile\Win32\w32timer.hpp"\
	"..\profile\xbox\xboxtimer.h"\
	"..\profile\xbox\xboxtimer.hpp"\
	"..\watchclient\WatchClient.h"\
	"..\watchclient\WatchInfo.h"\
	".\basetype.h"\
	".\compilercontrol.h"\
	".\ctroller.h"\
	".\game.h"\
	".\gameapp.h"\
	".\gameinit.h"\
	".\gameloop.h"\
	".\GCN\gcnapp.h"\
	".\GCN\gcncard.h"\
	".\GCN\gcnccd.h"\
	".\GCN\gcnctrol.h"\
	".\GCN\gcninput.h"\
	".\GCN\GCNMWPort.h"\
	".\keyctrl.h"\
	".\memcard.h"\
	".\PlatformPCH.h"\
	".\PS2\mwUtils.h"\
	".\PS2\mwUtils_PS2.h"\
	".\PS2\PS2app.h"\
	".\PS2\ps2card.h"\
	".\PS2\ps2ccd.h"\
	".\PS2\ps2ctrol.h"\
	".\ps2\ps2demo.h"\
	".\PS2\ps2input.h"\
	".\systemi.h"\
	".\Win32\Winapp.h"\
	".\Win32\wincard.h"\
	".\Win32\winccd.h"\
	".\Win32\winctrol.h"\
	".\Win32\wininput.h"\
	".\xbox\xboxapp.h"\
	".\xbox\xboxcard.h"\
	".\xbox\xboxccd.h"\
	".\xbox\xboxctrol.h"\
	".\xbox\xboxinput.h"\
	
NODEP_CPP_GCNMW=\
	"..\collide\zone.h"\
	"..\collide\zonemap.h"\
	".\3d8.h"\
	".\3d8caps.h"\
	".\3d8types.h"\
	".\3dx8.h"\
	".\3dx8core.h"\
	".\3dx8effect.h"\
	".\3dx8math.h"\
	".\3dx8math.inl"\
	".\3dx8mesh.h"\
	".\3dx8shape.h"\
	".\3dx8tex.h"\
	".\acTypes.h"\
	".\ekernel.h"\
	".\eregs.h"\
	".\estalt.h"\
	".\estruct.h"\
	".\etypes.h"\
	".\evvu0.h"\
	".\evvu1.h"\
	".\ibcdvd.h"\
	".\ibdma.h"\
	".\ibgraph.h"\
	".\ibmc.h"\
	".\ibpad.h"\
	".\ibpc.h"\
	".\ibscedemo.h"\
	".\ifcmd.h"\
	".\ifdev.h"\
	".\ifrpc.h"\
	".\iles.h"\
	".\in32\glext.h"\
	".\in32\win32.h"\
	".\n_registers.h"\
	".\olphin.h"\
	".\olphin\ai.h"\
	".\olphin\ar.h"\
	".\olphin\arq.h"\
	".\olphin\ax.h"\
	".\olphin\axfx.h"\
	".\olphin\base\PPCArch.h"\
	".\olphin\base\PPCWGPipe.h"\
	".\olphin\card.h"\
	".\olphin\db.h"\
	".\olphin\db\DBInterface.h"\
	".\olphin\dsp.h"\
	".\olphin\dvd.h"\
	".\olphin\gx.h"\
	".\olphin\gx\GXBump.h"\
	".\olphin\gx\GXCommandList.h"\
	".\olphin\gx\GXCpu2Efb.h"\
	".\olphin\gx\GXCull.h"\
	".\olphin\gx\GXDispList.h"\
	".\olphin\gx\GXDraw.h"\
	".\olphin\gx\GXEmVert.h"\
	".\olphin\gx\GXEnum.h"\
	".\olphin\gx\GXFifo.h"\
	".\olphin\gx\GXFrameBuffer.h"\
	".\olphin\gx\GXGeometry.h"\
	".\olphin\gx\GXGet.h"\
	".\olphin\gx\GXLighting.h"\
	".\olphin\gx\GXManage.h"\
	".\olphin\gx\GXPerf.h"\
	".\olphin\gx\GXPixel.h"\
	".\olphin\gx\GXStruct.h"\
	".\olphin\gx\GXTev.h"\
	".\olphin\gx\GXTexture.h"\
	".\olphin\gx\GXTransform.h"\
	".\olphin\gx\GXVerify.h"\
	".\olphin\gx\GXVert.h"\
	".\olphin\mix.h"\
	".\olphin\mtx.h"\
	".\olphin\mtx\GeoTypes.h"\
	".\olphin\os.h"\
	".\olphin\os\OSAlarm.h"\
	".\olphin\os\OSAlloc.h"\
	".\olphin\os\OSCache.h"\
	".\olphin\os\OSContext.h"\
	".\olphin\os\OSDC.h"\
	".\olphin\os\OSError.h"\
	".\olphin\os\OSException.h"\
	".\olphin\os\OSExpansion.h"\
	".\olphin\os\OSFastCast.h"\
	".\olphin\os\OSFont.h"\
	".\olphin\os\OSIC.h"\
	".\olphin\os\OSInterrupt.h"\
	".\olphin\os\OSL2.h"\
	".\olphin\os\OSLC.h"\
	".\olphin\os\OSMessage.h"\
	".\olphin\os\OSModule.h"\
	".\olphin\os\OSMutex.h"\
	".\olphin\os\OSReset.h"\
	".\olphin\os\OSResetSW.h"\
	".\olphin\os\OSSemaphore.h"\
	".\olphin\os\OSSerial.h"\
	".\olphin\os\OSThread.h"\
	".\olphin\pad.h"\
	".\olphin\types.h"\
	".\olphin\vi.h"\
	".\olphin\vi\vifuncs.h"\
	".\olphin\vi\vitypes.h"\
	".\SUtils.h"\
	".\tl.h"\
	".\xerr8.h"\
	".\xfile.h"\
	

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\GCNMWPort.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Win32\Avalanche.ico

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\basetype.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ctroller.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ctroller.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\game.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\game.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gameapp.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1
# ADD BASE CPP /O2
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gameapp.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gameinit.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gameloop.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keyctrl.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keyctrl.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\memcard.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\memcard.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlatformPCH.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# ADD BASE CPP /Yc"Platform\PlatformPCH.h"
# ADD CPP /Yc"Platform\PlatformPCH.h"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Platform\PlatformPCH.h"
# ADD CPP /Yc"Platform\PlatformPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlatformPCH.h
# End Source File
# Begin Source File

SOURCE=.\PlatformPCH_SN.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sn_Registers.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\systemi.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\systemi.h

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "platform - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
