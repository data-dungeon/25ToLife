# Microsoft Developer Studio Project File - Name="FMV" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FMV - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fmv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fmv.mak" CFG="FMV - Win32 PS2 EE CDRom Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FMV - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - PS2 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - PS2 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FMV - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLibNew/fmv", FJECAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PS2_Debug"
# PROP BASE Intermediate_Dir "PS2_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_Debug"
# PROP Intermediate_Dir "PS2_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Gm /ZI /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/2.9-ee-991111-01/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/iop/install\include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PS2_Release"
# PROP BASE Intermediate_Dir "PS2_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PS2_Release"
# PROP Intermediate_Dir "PS2_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/2.9-ee-991111-01/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/iop/install\include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /O2 /X /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /Zs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "DIRECTX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "_DEBUG" /D "DIRECTX" /FR /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/2.9-ee-991111-01/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/iop/install\include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# ADD CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

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
# ADD BASE CPP /nologo /w /W0 /Gm /ZI /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/2.9-ee-991111-01/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)/iop/install\include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /GZ /c
# ADD CPP /nologo /w /W0 /Gm /ZI /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /YX /Zs /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GameCube_Debug"
# PROP BASE Intermediate_Dir "GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /FR /YX /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GameCube_Release"
# PROP BASE Intermediate_Dir "GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /YX /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FMV___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "FMV___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /YX /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FMV___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "FMV___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FMV___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "FMV___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /YX /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /YX /c
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

# Name "FMV - Win32 OpenGL Release"
# Name "FMV - Win32 OpenGL Debug"
# Name "FMV - PS2 Debug"
# Name "FMV - PS2 Release"
# Name "FMV - Win32 PS2 EE Release"
# Name "FMV - Win32 PS2 EE Debug"
# Name "FMV - PS2 CDRom Release"
# Name "FMV - Win32 DirectX Release"
# Name "FMV - Win32 DirectX Debug"
# Name "FMV - PS2Direct Release"
# Name "FMV - PS2Direct Debug"
# Name "FMV - GameCube Debug"
# Name "FMV - GameCube Release"
# Name "FMV - GameCube CDROM"
# Name "FMV - Win32 PS2 EE CDRom Release"
# Name "FMV - Win32 GameCube Release DVDETH"
# Begin Group "GCN"

# PROP Default_Filter ""
# Begin Group "bink"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gcn\bink\bink.h
# End Source File
# Begin Source File

SOURCE=.\gcn\bink\radbase.h
# End Source File
# Begin Source File

SOURCE=.\gcn\bink\radmem.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\gcn\fmvdriver.cpp
DEP_CPP_FMVDR=\
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
	"..\..\enginenew\Audio\PS2\streamlib.h"\
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
	"..\..\enginenew\display\common\drawarray.h"\
	"..\..\enginenew\display\common\gfxfunc.h"\
	"..\..\enginenew\display\common\matrixstack.h"\
	"..\..\enginenew\display\common\renderstate.h"\
	"..\..\enginenew\display\Common\SceneView.h"\
	"..\..\enginenew\display\common\texturemgr.h"\
	"..\..\enginenew\display\common\texturestate.h"\
	"..\..\enginenew\display\DirectX\DirectX.h"\
	"..\..\enginenew\display\GCN\GCN.h"\
	"..\..\enginenew\display\GCN\Shader.h"\
	"..\..\enginenew\display\GCN\Shader.hpp"\
	"..\..\enginenew\display\opengl\glext.h"\
	"..\..\enginenew\display\opengl\opengl.h"\
	"..\..\enginenew\display\PS2Renderer\CompilerControl.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.h"\
	"..\..\enginenew\display\PS2Renderer\GSState.hpp"\
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
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\Debug.h"\
	"..\..\enginenew\layers\DLinkList.h"\
	"..\..\enginenew\layers\hog.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Media.h"\
	"..\..\enginenew\layers\mediahal.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\layers\SLinkList.h"\
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
	"..\enginehelper\hog.h"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\GCN\GCNMWPort.h"\
	".\gcn\bink\bink.h"\
	".\gcn\bink\radbase.h"\
	".\gcn\bink\radmem.h"\
	".\gcn\fmvdriver.h"\
	".\gcn\THPPlayer.h"\
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
	{$(INCLUDE)}"dolphin\thp.h"\
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
	

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\fmvdriver.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPAudioDecode.c
DEP_CPP_THPAU=\
	".\gcn\THPAudioDecode.h"\
	".\gcn\THPPlayer.h"\
	".\gcn\THPRead.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
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
	{$(INCLUDE)}"dolphin\thp.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPAudioDecode.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPDraw.c
DEP_CPP_THPDR=\
	".\gcn\THPDraw.h"\
	{$(INCLUDE)}"charPipeline\fileCache.h"\
	{$(INCLUDE)}"charPipeline\structures.h"\
	{$(INCLUDE)}"charpipeline\structures\dolphinString.h"\
	{$(INCLUDE)}"charpipeline\structures\htable.h"\
	{$(INCLUDE)}"charpipeline\structures\list.h"\
	{$(INCLUDE)}"charpipeline\structures\tree.h"\
	{$(INCLUDE)}"charPipeline\texPalette.h"\
	{$(INCLUDE)}"demo.h"\
	{$(INCLUDE)}"demo\DEMOPad.h"\
	{$(INCLUDE)}"demo\DEMOPuts.h"\
	{$(INCLUDE)}"demo\DEMOStats.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
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
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPDraw.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPPlayer.c
DEP_CPP_THPPL=\
	".\gcn\THPAudioDecode.h"\
	".\gcn\THPDraw.h"\
	".\gcn\THPPlayer.h"\
	".\gcn\THPRead.h"\
	".\gcn\THPVideoDecode.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
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
	{$(INCLUDE)}"dolphin\thp.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPPlayer.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPRead.c
DEP_CPP_THPRE=\
	".\gcn\THPPlayer.h"\
	".\gcn\THPRead.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
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
	{$(INCLUDE)}"dolphin\thp.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPRead.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPVideoDecode.c
DEP_CPP_THPVI=\
	".\gcn\THPPlayer.h"\
	".\gcn\THPRead.h"\
	".\gcn\THPVideoDecode.h"\
	{$(INCLUDE)}"dolphin.h"\
	{$(INCLUDE)}"dolphin\ai.h"\
	{$(INCLUDE)}"dolphin\ar.h"\
	{$(INCLUDE)}"dolphin\arq.h"\
	{$(INCLUDE)}"dolphin\ax.h"\
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
	{$(INCLUDE)}"dolphin\thp.h"\
	{$(INCLUDE)}"dolphin\types.h"\
	{$(INCLUDE)}"dolphin\vi.h"\
	{$(INCLUDE)}"dolphin\vi\vifuncs.h"\
	{$(INCLUDE)}"dolphin\vi\vitypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\THPVideoDecode.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "PS2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ps2\audiodec.cpp

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\audiodec.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\fmvdriver.cpp

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\fmvdriver.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\ldimage.cpp

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\playpss.cpp

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\playpss.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\playpssi.h
# End Source File
# Begin Source File

SOURCE=.\ps2\strfile.cpp

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ps2\strfile.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32\fmvdriver.cpp

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\fmvdriver.h

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\fmv.cpp
DEP_CPP_FMV_C=\
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
	"..\..\enginenew\Audio\PS2\streamlib.h"\
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
	"..\..\enginenew\display\common\matrixstack.h"\
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
	"..\enginehelper\hog.h"\
	"..\enginehelper\timer.h"\
	"..\enginehelper\timer.hpp"\
	"..\linearalgebra\rayseg.h"\
	"..\linearalgebra\rayseg.hpp"\
	"..\platform\basetype.h"\
	"..\platform\compilercontrol.h"\
	"..\platform\ctroller.h"\
	"..\platform\game.h"\
	"..\platform\gameapp.h"\
	"..\platform\gameinit.h"\
	"..\platform\gameloop.h"\
	"..\platform\GCN\gcnapp.h"\
	"..\platform\GCN\gcncard.h"\
	"..\platform\GCN\gcnccd.h"\
	"..\platform\GCN\gcnctrol.h"\
	"..\platform\GCN\gcninput.h"\
	"..\platform\GCN\GCNMWPort.h"\
	"..\platform\memcard.h"\
	"..\platform\PS2\PS2app.h"\
	"..\platform\PS2\ps2card.h"\
	"..\platform\PS2\ps2ccd.h"\
	"..\platform\PS2\ps2ctrol.h"\
	"..\platform\PS2\ps2input.h"\
	"..\platform\systemi.h"\
	"..\platform\Win32\Winapp.h"\
	"..\platform\Win32\wincard.h"\
	"..\platform\Win32\winccd.h"\
	"..\platform\Win32\winctrol.h"\
	"..\platform\Win32\wininput.h"\
	"..\platform\xbox\xboxapp.h"\
	"..\platform\xbox\xboxcard.h"\
	"..\platform\xbox\xboxccd.h"\
	"..\platform\xbox\xboxctrol.h"\
	"..\platform\xbox\xboxinput.h"\
	".\fmv.h"\
	".\gcn\fmvdriver.h"\
	".\ps2\fmvdriver.h"\
	".\win32\fmvdriver.h"\
	".\xbox\fmvdriver.h"\
	".\xbox\wmvplayer.h"\
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
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpad.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"libscf.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sifrpc.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"wmvxmo.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "FMV - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Release"

!ELSEIF  "$(CFG)" == "FMV - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Debug"

!ELSEIF  "$(CFG)" == "FMV - GameCube Release"

!ELSEIF  "$(CFG)" == "FMV - GameCube CDROM"

!ELSEIF  "$(CFG)" == "FMV - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "FMV - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fmv.h
# End Source File
# End Target
# End Project
