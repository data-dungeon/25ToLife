# Microsoft Developer Studio Project File - Name="WatchClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WatchClient - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "WatchClient.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "WatchClient.mak" CFG="WatchClient - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "WatchClient - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WatchClient - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/WatchClient", HOKAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WatchClient - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WatchClient - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "WatchClient - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Zs /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "_DEBUG" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Release"

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
# ADD BASE CPP /nologo /MT /w /W0 /GX /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /c
# ADD CPP /nologo /MT /w /W0 /GX /Zi /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Debug"

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
# ADD BASE CPP /nologo /MTd /w /W0 /Gm /GX /ZI /Od /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /GZ /c
# ADD CPP /nologo /MTd /w /W0 /Gm /GX /Zi /Od /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /Zs /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WatchClient___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "WatchClient___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WatchClient___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "WatchClient___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WatchClient___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "WatchClient___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /c
# SUBTRACT BASE CPP /Fr /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WatchClient___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "WatchClient___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\include" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "WatchClient - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WatchClient___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "WatchClient___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /c
# SUBTRACT BASE CPP /Fr /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
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

# Name "WatchClient - Win32 OpenGL Release"
# Name "WatchClient - Win32 OpenGL Debug"
# Name "WatchClient - Win32 PS2 EE Release"
# Name "WatchClient - Win32 PS2 EE Debug"
# Name "WatchClient - PS2 CDRom Release"
# Name "WatchClient - Win32 DirectX Release"
# Name "WatchClient - Win32 DirectX Debug"
# Name "WatchClient - PS2Direct Release"
# Name "WatchClient - PS2Direct Debug"
# Name "WatchClient - GameCube Debug"
# Name "WatchClient - GameCube Release"
# Name "WatchClient - GameCube CDROM"
# Name "WatchClient - Win32 PS2 EE CDRom Release"
# Name "WatchClient - Win32 GameCube Release DVDETH"
# Begin Source File

SOURCE=.\DataTable.h
# End Source File
# Begin Source File

SOURCE=.\DataTable.hpp
# End Source File
# Begin Source File

SOURCE=.\deci2pc.h
# End Source File
# Begin Source File

SOURCE=.\MessageQueue.cpp

!IF  "$(CFG)" == "WatchClient - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Release"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Release"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube CDROM"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\MessageQueue.h
# End Source File
# Begin Source File

SOURCE=.\WatchClient.cpp

!IF  "$(CFG)" == "WatchClient - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Release"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Release"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube CDROM"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\WatchClient.h
# End Source File
# Begin Source File

SOURCE=.\WatchClientGCN.cpp

!IF  "$(CFG)" == "WatchClient - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Release"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Release"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube CDROM"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\WatchClientGCN.h
# End Source File
# Begin Source File

SOURCE=.\WatchClientPS2.cpp

!IF  "$(CFG)" == "WatchClient - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Release"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Release"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube CDROM"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\WatchClientPS2.h
# End Source File
# Begin Source File

SOURCE=.\WatchClientWin32.cpp

!IF  "$(CFG)" == "WatchClient - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Release"

!ELSEIF  "$(CFG)" == "WatchClient - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Debug"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube Release"

!ELSEIF  "$(CFG)" == "WatchClient - GameCube CDROM"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "WatchClient - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\WatchClientWin32.h
# End Source File
# Begin Source File

SOURCE=.\WatchInfo.h
# End Source File
# Begin Source File

SOURCE=.\WatchTable.h
# End Source File
# End Target
# End Project
