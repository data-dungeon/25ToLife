# Microsoft Developer Studio Project File - Name="platform" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=platform - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "toolplatform.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "toolplatform.mak" CFG="platform - Win32 PS2 EE CDRom Release"
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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn /debug -mvu0-use-vf0-vf15 /c
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
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

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

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlatformPCH.h
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

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\toolgameapp.cpp

!IF  "$(CFG)" == "platform - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "platform - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "platform - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "platform - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Release"

!ELSEIF  "$(CFG)" == "platform - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Debug"

!ELSEIF  "$(CFG)" == "platform - GameCube Release"

!ELSEIF  "$(CFG)" == "platform - GameCube CDROM"

!ELSEIF  "$(CFG)" == "platform - Win32 PS2 EE CDRom Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\toolgameapp.h
# End Source File
# End Target
# End Project
