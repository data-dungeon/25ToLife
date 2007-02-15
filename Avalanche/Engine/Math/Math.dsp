# Microsoft Developer Studio Project File - Name="Math" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Math - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Math.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Math.mak" CFG="Math - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Math - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Max Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Maya Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Maya Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Math - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/Math", WEAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /FR /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /FR /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

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
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /D "WIN32" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "PS2" /D "CW" /YX /FD /Zs /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "CW" /Yu"Math/MathPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

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
# ADD BASE CPP /nologo /w /W0 /Od /X /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "CW" /FR /YX /FD /Zs /c
# ADD CPP /nologo /w /W0 /Zi /Od /X /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "CW" /Yu"Math/MathPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "PS2" /D "CW" /YX /FD /Zs /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Math/MathPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Math___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "Math___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Math/MathPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Math___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "Math___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /D "WIN32" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Math/MathPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Math___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "Math___Win32_Max_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /FR /YX /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D MMX_OK=$(MMX_OK) /D "_DEBUG" /D "WIN32" /D "WIN32_MAX_DEBUG" /FR /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Math___Win32_Max_Release"
# PROP BASE Intermediate_Dir "Math___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D MMX_OK=$(MMX_OK) /D "WIN32" /D "WIN32_MAX_RELEASE" /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Math___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "Math___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Math/MathPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Math/MathPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Math___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "Math___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Math___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "Math___Win32_Max_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /FR /Yu"Math/MathPCH.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /FR /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Math___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "Math___Win32_Maya_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Math___Win32_Maya_Debug"
# PROP Intermediate_Dir "Math___Win32_Maya_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /FR /Yu"Math/MathPCH.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D MMX_OK=$(MMX_OK) /D "_DEBUG" /D "WIN32_MAYA_DEBUG" /D "WIN32" /FR /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Math___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "Math___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Math___Win32_Maya_Release"
# PROP Intermediate_Dir "Math___Win32_Maya_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"Math/MathPCH.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D MMX_OK=$(MMX_OK) /D "WIN32_MAYA_RELEASE" /D "WIN32" /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Math___Win32_Release"
# PROP BASE Intermediate_Dir "Math___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"Math/MathPCH.h" /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TOOLS" /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Math___Win32_Debug"
# PROP BASE Intermediate_Dir "Math___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /FR /Yu"Math/MathPCH.h" /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /D "TOOLS" /FR /Yu"Math/MathPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Math___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "Math___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Math/MathPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Math/MathPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ENDIF 

# Begin Target

# Name "Math - Win32 OpenGL Debug"
# Name "Math - Win32 OpenGL Release"
# Name "Math - Win32 DirectX Debug"
# Name "Math - Win32 DirectX Release"
# Name "Math - PS2Direct Release"
# Name "Math - PS2Direct Debug"
# Name "Math - PS2 CDRom Release"
# Name "Math - Win32 PS2 EE Release"
# Name "Math - Win32 PS2 EE Debug"
# Name "Math - GameCube Debug"
# Name "Math - GameCube Release"
# Name "Math - Win32 Max Debug"
# Name "Math - Win32 Max Release"
# Name "Math - GameCube CDROM"
# Name "Math - Win32 PS2 EE CDRom Release"
# Name "Math - Win32 Max Hybrid"
# Name "Math - Win32 Maya Debug"
# Name "Math - Win32 Maya Release"
# Name "Math - Win32 Release"
# Name "Math - Win32 Debug"
# Name "Math - Win32 GameCube Release DVDETH"
# Begin Group "Generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generic\BuildView.cpp
DEP_CPP_BUILD=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Det.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\DistFunc.cpp
DEP_CPP_DISTF=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\fastrand.cpp
DEP_CPP_FASTR=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Mat3x3.cpp
DEP_CPP_MAT3X=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Mat3x3.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Mat4x4.cpp
DEP_CPP_MAT4X=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Mat4x4.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Math.cpp
DEP_CPP_MATH_=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Math.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Matrix.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Quatern.cpp
DEP_CPP_QUATE=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Vect2.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Vect3.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Vect4.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Vector.cpp
DEP_CPP_VECTO=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generic\Vector.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "PS2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2\CollideMicro.vcl

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\CollideMicro.vsm

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Mat4x4.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Mat4x4PS2.cpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Mat4x4sn.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Math.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\MathInit.cpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\MathMicro.dsm

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\QuaternMicro.vcl

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\QuaternMicro.vsm

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Quaternsn.hpp
# End Source File
# Begin Source File

SOURCE=.\PS2\TrigMicro.vcl

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\TrigMicro.vsm

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Vect3.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Vect3sn.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Vect4.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Vect4sn.hpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32\Mat4x4.hpp
# End Source File
# Begin Source File

SOURCE=.\win32\Mat4x4WIN32.cpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32\Math.hpp
# End Source File
# Begin Source File

SOURCE=.\win32\Vect3.hpp
# End Source File
# Begin Source File

SOURCE=.\win32\Vect4.hpp
# End Source File
# End Group
# Begin Group "GCN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gcn\Mat4x4.hpp
# End Source File
# Begin Source File

SOURCE=.\gcn\Mat4x4GCN.cpp
DEP_CPP_MAT4X4=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gcn\Math.hpp
# End Source File
# Begin Source File

SOURCE=.\gcn\Vect3.hpp
# End Source File
# Begin Source File

SOURCE=.\gcn\Vect4.hpp
# End Source File
# End Group
# Begin Group "3DNow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3DNow\3DNow.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BuildView.h
# End Source File
# Begin Source File

SOURCE=.\Det.h
# End Source File
# Begin Source File

SOURCE=.\DistFunc.h
# End Source File
# Begin Source File

SOURCE=.\fastrand.h
# End Source File
# Begin Source File

SOURCE=.\Macros.h
# End Source File
# Begin Source File

SOURCE=.\Mat3x3.h
# End Source File
# Begin Source File

SOURCE=.\Mat4x4.h
# End Source File
# Begin Source File

SOURCE=.\MathClass.h
# End Source File
# Begin Source File

SOURCE=.\MathDeprecate.h
# End Source File
# Begin Source File

SOURCE=.\MathPCH.cpp
DEP_CPP_MATHP=\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\MasterList.h"\
	"..\layers\Assert\Assert.h"\
	".\BuildView.h"\
	".\Det.h"\
	".\DistFunc.h"\
	".\fastrand.h"\
	".\gcn\Mat4x4.hpp"\
	".\gcn\Math.hpp"\
	".\Generic\Det.hpp"\
	".\Generic\Mat3x3.hpp"\
	".\Generic\Mat4x4.hpp"\
	".\Generic\Math.hpp"\
	".\Generic\Matrix.hpp"\
	".\Generic\Vect2.hpp"\
	".\Generic\Vector.hpp"\
	".\Mat3x3.h"\
	".\Mat4x4.h"\
	".\MathClass.h"\
	".\MathDeprecate.h"\
	".\MathPCH.h"\
	".\MathStats.h"\
	".\MathStruct.h"\
	".\Matrix.h"\
	".\PS2\Mat4x4.hpp"\
	".\PS2\Mat4x4sn.hpp"\
	".\PS2\Math.hpp"\
	".\PS2\Quaternsn.hpp"\
	".\Quatern.h"\
	".\Vect2.h"\
	".\Vector.h"\
	".\Vector3GCN.hpp"\
	".\Vector3Generic.hpp"\
	".\Vector3GenericPacked.h"\
	".\Vector3GenericPadded.h"\
	".\Vector3SNPS2.hpp"\
	".\Vector3SNPS2Padded.h"\
	".\Vector3SNPS2Padded.hpp"\
	".\Vector3Template.h"\
	".\Vector3Template.hpp"\
	".\Vector3WIN32.hpp"\
	".\Vector4GCN.hpp"\
	".\Vector4Generic.hpp"\
	".\Vector4GenericAligned.h"\
	".\Vector4GenericPacked.h"\
	".\Vector4SNPS2.hpp"\
	".\Vector4SNPS2Aligned.h"\
	".\Vector4SNPS2Aligned.hpp"\
	".\Vector4SNPS2X.h"\
	".\Vector4SNPS2X.hpp"\
	".\Vector4Template.h"\
	".\Vector4Template.hpp"\
	".\Vector4WIN32.hpp"\
	".\win32\Mat4x4.hpp"\
	".\win32\Math.hpp"\
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
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# ADD BASE CPP /Yc"Math/MathPCH.h"
# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# ADD BASE CPP /Yc"Math/MathPCH.h"
# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# ADD BASE CPP /Yc"Math/MathPCH.h"
# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# ADD BASE CPP /Yc"Math/MathPCH.h"
# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# ADD BASE CPP /Yc"Math/MathPCH.h"
# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# ADD BASE CPP /Yc"Math/MathPCH.h"
# ADD CPP /Yc"Math/MathPCH.h"

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Math/MathPCH.h"
# ADD CPP /Yc"Math/MathPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MathPCH.h
# End Source File
# Begin Source File

SOURCE=.\MathPCH_SN.cpp

!IF  "$(CFG)" == "Math - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "Math - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "Math - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Math - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MathStats.h
# End Source File
# Begin Source File

SOURCE=.\MathStruct.h
# End Source File
# Begin Source File

SOURCE=.\MathUndeprecate.h
# End Source File
# Begin Source File

SOURCE=.\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Quatern.h
# End Source File
# Begin Source File

SOURCE=.\Vect2.h
# End Source File
# Begin Source File

SOURCE=.\Vector.h
# End Source File
# Begin Source File

SOURCE=.\Vector3GenericPacked.h
# End Source File
# Begin Source File

SOURCE=.\Vector3GenericPadded.h
# End Source File
# Begin Source File

SOURCE=.\Vector3Template.h
# End Source File
# Begin Source File

SOURCE=.\Vector3Template.hpp
# End Source File
# Begin Source File

SOURCE=.\Vector4GenericAligned.h
# End Source File
# Begin Source File

SOURCE=.\Vector4GenericPacked.h
# End Source File
# Begin Source File

SOURCE=.\Vector4Template.h
# End Source File
# Begin Source File

SOURCE=.\Vector4Template.hpp
# End Source File
# End Target
# End Project
