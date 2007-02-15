# Microsoft Developer Studio Project File - Name="mathUtil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mathUtil - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "mathUtil.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "mathUtil.mak" CFG="mathUtil - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "mathUtil - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mathUtil - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/mathUtil", ZJKAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WGL_Release"
# PROP Intermediate_Dir "WGL_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /Yu"MathUtil\MathUtilPCH.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WGL_Debug"
# PROP Intermediate_Dir "WGL_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"MathUtil\MathUtilPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

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

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

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

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"MathUtil\MathUtilPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(AVGAME)" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"MathUtil\MathUtilPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(AVGAME)" /D "_DEBUG" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /Yu"MathUtil\MathUtilPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Yu"MathUtil\MathUtilPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

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
# ADD BASE CPP /nologo /w /W0 /Gm /ZI /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /FR /YX /Zs /GZ /c
# ADD CPP /nologo /w /W0 /Gm /Zi /Od /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Yu"MathUtil\MathUtilPCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mathUtil___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "mathUtil___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"MathUtil\MathUtilPCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mathUtil___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "mathUtil___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"MathUtil\MathUtilPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mathUtil___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "mathUtil___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"MathUtil\MathUtilPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"MathUtil\MathUtilPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mathUtil___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "mathUtil___Win32_PS2_EE_CDRom_Release"
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

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mathUtil___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "mathUtil___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"MathUtil\MathUtilPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"MathUtil\MathUtilPCH.h" /c
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

# Name "mathUtil - Win32 OpenGL Release"
# Name "mathUtil - Win32 OpenGL Debug"
# Name "mathUtil - Win32 PS2 EE Release"
# Name "mathUtil - Win32 PS2 EE Debug"
# Name "mathUtil - PS2 CDRom Release"
# Name "mathUtil - Win32 DirectX Release"
# Name "mathUtil - Win32 DirectX Debug"
# Name "mathUtil - PS2Direct Release"
# Name "mathUtil - PS2Direct Debug"
# Name "mathUtil - GameCube Debug"
# Name "mathUtil - GameCube Release"
# Name "mathUtil - GameCube CDROM"
# Name "mathUtil - Win32 PS2 EE CDRom Release"
# Name "mathUtil - Win32 GameCube Release DVDETH"
# Begin Source File

SOURCE=.\avgnum.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Bairstow.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Bairstow.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Ballistic.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Ballistic.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Cholesky.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Cholesky.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Cholesky.hpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dda.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dda.hpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\eigen.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\eigen.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\eigen.hpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gauss.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gauss.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gauss.hpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interpolate.h
# End Source File
# Begin Source File

SOURCE=.\linearsystem.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\linearsystem.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mathcons.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MathUtil.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MathUtil.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MathUtilPCH.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

# ADD BASE CPP /Yc"MathUtil\MathUtilPCH.h"
# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"MathUtil\MathUtilPCH.h"
# ADD CPP /Yc"MathUtil\MathUtilPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MathUtilPCH.h
# End Source File
# Begin Source File

SOURCE=.\MathUtilPCH_SN.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NumberRoot.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NumberRoot.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SecantSolve.cpp

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SecantSolve.h

!IF  "$(CFG)" == "mathUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "mathUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "mathUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "mathUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mathUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
