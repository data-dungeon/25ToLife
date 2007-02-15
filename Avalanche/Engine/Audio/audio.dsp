# Microsoft Developer Studio Project File - Name="audio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=audio - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "audio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "audio.mak" CFG="audio - Win32 PS2 EE CDRom Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "audio - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audio - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/Audio", IAWAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(AVGAME)\Include" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /Yu"Audio\AudioPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(AVGAME)\Include" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"Audio\AudioPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "A_COMPILER_SN" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "A_COMPILER_SN" -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Audio\AudioPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"Audio\AudioPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "_DEBUG" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /Yu"Audio\AudioPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

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
# ADD BASE CPP /nologo /MT /w /W0 /O2 /X /I "$(AVGAME)" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /c
# ADD CPP /nologo /MT /w /W0 /Zi /O2 /X /I "$(AVGAME)" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Yu"Audio\AudioPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

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
# ADD BASE CPP /nologo /MTd /w /W0 /Gm /ZI /Od /X /I "$(AVGAME)" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /GZ /c
# ADD CPP /nologo /MTd /w /W0 /Gm /Zi /Od /X /I "$(AVGAME)" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)/common/include" /I "$(AVPATH)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Yu"Audio\AudioPCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GCN_Debug"
# PROP BASE Intermediate_Dir "GCN_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(AVGAME)\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Audio\AudioPCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GCN_Release"
# PROP BASE Intermediate_Dir "GCN_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(AVGAME)\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Audio\AudioPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "audio___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "audio___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"Audio\AudioPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Audio\AudioPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "audio___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "audio___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "A_COMPILER_SN" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
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

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "audio___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "audio___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Audio\AudioPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Audio\AudioPCH.h" /c
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

# Name "audio - Win32 OpenGL Release"
# Name "audio - Win32 OpenGL Debug"
# Name "audio - Win32 PS2 EE Release"
# Name "audio - Win32 PS2 EE Debug"
# Name "audio - PS2 CDRom Release"
# Name "audio - Win32 DirectX Release"
# Name "audio - Win32 DirectX Debug"
# Name "audio - PS2Direct Release"
# Name "audio - PS2Direct Debug"
# Name "audio - GameCube Debug"
# Name "audio - GameCube Release"
# Name "audio - GameCube CDROM"
# Name "audio - Win32 PS2 EE CDRom Release"
# Name "audio - Win32 GameCube Release DVDETH"
# Begin Group "PS2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ps2\sdriver.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\sdriver.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\sound.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\sound.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\sounderror.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\sounderror.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\soundstream.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\soundstream.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\streamirx.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\streamirx.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\streamlib.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\streamlib.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\vag.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32\sdriver.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\sdriver.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\sdrivercaps.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\sdrivercaps.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\sdriverenum.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\sdriverenum.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\sound.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\sound.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\sounderror.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\sounderror.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\soundstream.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\soundstream.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\wavefile.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\wavefile.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "GCN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gcn\dspadpcm.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\HWSoundStream.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\HWSoundStream.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\sbankgcn.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\sbankgcn.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\sdriver.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\sdriver.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\sound.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\sound.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\sounderror.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\sounderror.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\soundstream.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\SWSoundStream.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\SWSoundStream.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "Emitter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\emitter\e3dparam.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\emitter\e3dparam.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\emitter\semit2d.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\emitter\semit2d.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\emitter\semit3d.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\emitter\semit3d.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\emitter\semits.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\emitter\semits.h
# End Source File
# Begin Source File

SOURCE=.\emitter\semitter.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\emitter\semitter.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\math\doppler.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\math\doppler.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Math\occlude.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Math\occlude.h
# End Source File
# Begin Source File

SOURCE=.\math\positional.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\math\positional.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\math\semitone.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\math\volume.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\math\volume.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "Group"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\group\reclaim.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\reclaim.h
# End Source File
# Begin Source File

SOURCE=.\group\schan2d.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\schan2d.h
# End Source File
# Begin Source File

SOURCE=.\group\schan3d.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\schan3d.h
# End Source File
# Begin Source File

SOURCE=.\group\schannel.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\schannel.h
# End Source File
# Begin Source File

SOURCE=.\group\sgalias.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\sgalias.h
# End Source File
# Begin Source File

SOURCE=.\group\sgroup.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\sgroup.h
# End Source File
# Begin Source File

SOURCE=.\group\sgroup2d.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\sgroup2d.h
# End Source File
# Begin Source File

SOURCE=.\group\sgroup3d.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\sgroup3d.h
# End Source File
# Begin Source File

SOURCE=.\group\shandle.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\group\shandle.h
# End Source File
# End Group
# Begin Group "Evironment"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\environment\ambient.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\environment\ambient.h
# End Source File
# Begin Source File

SOURCE=.\environment\ambsound.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\environment\ambsound.h
# End Source File
# Begin Source File

SOURCE=.\environment\envdbl.h
# End Source File
# Begin Source File

SOURCE=.\environment\envfield.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\environment\envfield.h
# End Source File
# Begin Source File

SOURCE=.\environment\envsource.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\environment\envsource.h
# End Source File
# Begin Source File

SOURCE=.\environment\envstrig.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\environment\envstrig.h
# End Source File
# Begin Source File

SOURCE=.\environment\semitenv.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\environment\semitenv.h
# End Source File
# End Group
# Begin Group "Function"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\function\function.h
# End Source File
# Begin Source File

SOURCE=.\function\lfo.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\function\lfo.h
# End Source File
# Begin Source File

SOURCE=.\function\ramp.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\function\ramp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\audiolib.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AudioPCH.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# ADD BASE CPP /Yc"Audio\AudioPCH.h"
# ADD CPP /Yc"Audio\AudioPCH.h"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Audio\AudioPCH.h"
# ADD CPP /Yc"Audio\AudioPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AudioPCH.h
# End Source File
# Begin Source File

SOURCE=.\AudioPCH_SN.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\audiosys.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\audiosys.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jukebox.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jukebox.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\listener.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\listener.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soundattr.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soundattr.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soundbank.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soundbank.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounddriver.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sounddriver.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soundmodexport.h
# End Source File
# Begin Source File

SOURCE=.\streamq.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\streamq.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\voiceq.cpp

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\voiceq.h

!IF  "$(CFG)" == "audio - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "audio - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "audio - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Release"

!ELSEIF  "$(CFG)" == "audio - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Debug"

!ELSEIF  "$(CFG)" == "audio - GameCube Release"

!ELSEIF  "$(CFG)" == "audio - GameCube CDROM"

!ELSEIF  "$(CFG)" == "audio - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "audio - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
