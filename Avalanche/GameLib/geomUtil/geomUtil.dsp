# Microsoft Developer Studio Project File - Name="geomUtil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=geomUtil - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "geomUtil.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "geomUtil.mak" CFG="geomUtil - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "geomUtil - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geomUtil - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/geomUtil", XKKAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /Yu"GeomUtil\GeomUtilPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"GeomUtil\GeomUtilPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

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

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

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

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"GeomUtil\GeomUtilPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(AVGAME)" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"GeomUtil\GeomUtilPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(AVGAME)" /D "_DEBUG" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /Yu"GeomUtil\GeomUtilPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /FR /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Yu"GeomUtil\GeomUtilPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

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
# ADD CPP /nologo /w /W0 /Gm /Zi /Od /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Yu"GeomUtil\GeomUtilPCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geomUtil___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "geomUtil___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"GeomUtil\GeomUtilPCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "geomUtil___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "geomUtil___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"GeomUtil\GeomUtilPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "geomUtil___Win32_Max_Release"
# PROP BASE Intermediate_Dir "geomUtil___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /D "WIN32_MAX_RELEASE" /Yu"GeomUtil\GeomUtilPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geomUtil___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "geomUtil___Win32_Max_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "_DEBUG" /D "WIN32" /D "WIN32_MAX_DEBUG" /FR /Yu"GeomUtil\GeomUtilPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "geomUtil___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "geomUtil___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"GeomUtil\GeomUtilPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"GeomUtil\GeomUtilPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geomUtil___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "geomUtil___Win32_PS2_EE_CDRom_Release"
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

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "geomUtil___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "geomUtil___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"GeomUtil\GeomUtilPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"GeomUtil\GeomUtilPCH.h" /c
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

# Name "geomUtil - Win32 OpenGL Release"
# Name "geomUtil - Win32 OpenGL Debug"
# Name "geomUtil - Win32 PS2 EE Release"
# Name "geomUtil - Win32 PS2 EE Debug"
# Name "geomUtil - PS2 CDRom Release"
# Name "geomUtil - Win32 DirectX Release"
# Name "geomUtil - Win32 DirectX Debug"
# Name "geomUtil - PS2Direct Release"
# Name "geomUtil - PS2Direct Debug"
# Name "geomUtil - GameCube Debug"
# Name "geomUtil - GameCube Release"
# Name "geomUtil - Win32 Max Release"
# Name "geomUtil - Win32 Max Debug"
# Name "geomUtil - GameCube CDROM"
# Name "geomUtil - Win32 PS2 EE CDRom Release"
# Name "geomUtil - Win32 GameCube Release DVDETH"
# Begin Source File

SOURCE=.\AnimCurve.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\AnimCurve.h
# End Source File
# Begin Source File

SOURCE=.\chord.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\chord.h

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\chord.hpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\clipseg.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\clipseg.h

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\clipseg.hpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GeomUtilPCH.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

# ADD BASE CPP /Yc"GeomUtil\GeomUtilPCH.h"
# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"GeomUtil\GeomUtilPCH.h"
# ADD CPP /Yc"GeomUtil\GeomUtilPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GeomUtilPCH.h
# End Source File
# Begin Source File

SOURCE=.\GeomUtilPCH_SN.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\plane.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\plane.h
# End Source File
# Begin Source File

SOURCE=.\plane.hpp
# End Source File
# Begin Source File

SOURCE=.\planefit.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\planefit.h

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlaneReflect.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlaneReflect.h

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PointToLine.h

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Spline.cpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Spline.h

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tween.h

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tween.hpp

!IF  "$(CFG)" == "geomUtil - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Release"

!ELSEIF  "$(CFG)" == "geomUtil - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Release"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "geomUtil - GameCube CDROM"

!ELSEIF  "$(CFG)" == "geomUtil - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "geomUtil - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
