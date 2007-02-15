# Microsoft Developer Studio Project File - Name="tupperware" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tupperware - Win32 GameCube Release DVDETH
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "tupperware.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "tupperware.mak" CFG="tupperware - Win32 GameCube Release DVDETH"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "tupperware - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 Max Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 Maya Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 Maya Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/Tupperware", MROAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tupperware - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OPENGL" /Yu"TupperwarePch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware___Win32_OpenGL_Debug"
# PROP BASE Intermediate_Dir "tupperware___Win32_OpenGL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WGL_Debug"
# PROP Intermediate_Dir "WGL_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tupperware___Win32_OpenGL_Release"
# PROP BASE Intermediate_Dir "tupperware___Win32_OpenGL_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WGL_Release"
# PROP Intermediate_Dir "WGL_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperwarePch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "tupperware___Win32_Max_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "tupperware___Win32_Max_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_WINDOWS" /D "_DEBUG" /D "WIN32" /D "WIN32_MAX_DEBUG" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tupperware___Win32_Max_Release"
# PROP BASE Intermediate_Dir "tupperware___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "WIN32_MAX_RELEASE" /Yu"TupperwarePch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD CPP /nologo /MTd /w /W0 /GX /Zi /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "TUPPERWARE_FOR_ENGINE" /FD /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OPENGL" /Yu"TupperwarePch.h" /FD /c
# ADD CPP /nologo /MT /w /W0 /GX /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "TUPPERWARE_FOR_ENGINE" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware___Win32_DirectX_Debug"
# PROP BASE Intermediate_Dir "tupperware___Win32_DirectX_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WDX_Debug"
# PROP Intermediate_Dir "WDX_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tupperware___Win32_DirectX_Release"
# PROP BASE Intermediate_Dir "tupperware___Win32_DirectX_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WDX_Release"
# PROP Intermediate_Dir "WDX_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OPENGL" /Yu"TupperwarePch.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

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
# ADD BASE CPP /nologo /MTd /w /W0 /GX /ZI /Od /I "$(AVGAME)" /D "PS2" /D "_DEBUG" /D "CW" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "_DEBUG" /D "TUPPERWARE_FOR_ENGINE" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:'GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GCN_Release"
# PROP BASE Intermediate_Dir "GCN_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /D "TUPPERWARE_FOR_ENGINE" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:'GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PS2_CDRom_Release"
# PROP BASE Intermediate_Dir "PS2_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PS2_CDRom_Release"
# PROP Intermediate_Dir "PS2_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /w /W0 /GX /O2 /I "$(AVGAME)" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /w /W0 /GX /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "PS2" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "TUPPERWARE_FOR_ENGINE" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "tupperware___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /Yu"TupperwarePch.h" /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C:\usr\local\sce\ee\gcc\lib\gcc-lib\ee\2.95.3\include" /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "TUPPERWARE_FOR_ENGINE" /FD -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tupperware___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "tupperware___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"TupperwarePch.h" /FD /c
# ADD CPP /nologo /MLd /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\gcc\lib\gcc-lib\ee\2.95.3\include" /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "TUPPERWARE_FOR_ENGINE" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tupperware___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "tupperware___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MLd /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MLd /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\gcc\lib\gcc-lib\ee\2.95.3\include" /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" /D "TUPPERWARE_FOR_ENGINE" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "tupperware___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /D "TUPPERWARE_FOR_ENGINE" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "tupperware___Win32_Maya_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_WINDOWS" /D "_DEBUG" /D "WIN32_MAYA_DEBUG" /D "WIN32" /Yu"TupperwarePch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tupperware___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "tupperware___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /Yu"TupperwarePch.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_WINDOWS" /D "WIN32_MAYA_RELEASE" /D "WIN32" /Yu"TupperwarePch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "tupperware___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /D "TUPPERWARE_FOR_ENGINE" /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /D "TUPPERWARE_FOR_ENGINE" /D "DVDETH" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ENDIF

# Begin Target

# Name "tupperware - Win32 Debug"
# Name "tupperware - Win32 Release"
# Name "tupperware - Win32 OpenGL Debug"
# Name "tupperware - Win32 OpenGL Release"
# Name "tupperware - Win32 Max Hybrid"
# Name "tupperware - Win32 Max Debug"
# Name "tupperware - Win32 Max Release"
# Name "tupperware - PS2Direct Debug"
# Name "tupperware - PS2Direct Release"
# Name "tupperware - Win32 DirectX Debug"
# Name "tupperware - Win32 DirectX Release"
# Name "tupperware - GameCube Debug"
# Name "tupperware - GameCube Release"
# Name "tupperware - PS2 CDRom Release"
# Name "tupperware - Win32 PS2 EE Debug"
# Name "tupperware - Win32 PS2 EE Release"
# Name "tupperware - Win32 PS2 EE CDRom Release"
# Name "tupperware - GameCube CDROM"
# Name "tupperware - Win32 Maya Debug"
# Name "tupperware - Win32 Maya Release"
# Name "tupperware - Win32 GameCube Release DVDETH"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Group "Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Log.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TupperwarePch.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

# PROP Exclude_From_Build 1
# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperwarePch.h"
# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperwarePch.h"
# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperwarePch.h"
# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperwarePch.h"
# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1
# SUBTRACT BASE CPP /YX /Yc
# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperwarePch.h"
# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

# ADD BASE CPP /Yc"TupperwarePch.h"
# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

# ADD BASE CPP /Yc"TupperwarePch.h"
# ADD CPP /Yc"TupperwarePch.h"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperwarePch.h"
# ADD CPP /Yc"TupperwarePch.h"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\TupperwarePch.h
# End Source File
# Begin Source File

SOURCE=.\TupperwarePch_SN.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# Begin Group "Atom"

# PROP Default_Filter ""
# Begin Group "Aggregate"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Aggregate.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\Aggregate.h
# End Source File
# Begin Source File

SOURCE=.\Aggregate.hpp
# End Source File
# End Group
# Begin Group "List"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\List.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\List.h
# End Source File
# End Group
# Begin Group "Scalar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Scalar.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scalar.h
# End Source File
# End Group
# Begin Group "Comment"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Comment.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Comment.h
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\compareator.h
# End Source File
# Begin Source File

SOURCE=.\compareator.hpp
# End Source File
# Begin Source File

SOURCE=.\elements.h
# End Source File
# Begin Source File

SOURCE=.\hash.h
# End Source File
# Begin Source File

SOURCE=.\Iterator.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Iterator.h
# End Source File
# Begin Source File

SOURCE=.\memorymanager.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\memorymanager.h
# End Source File
# Begin Source File

SOURCE=.\path.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\path.h
# End Source File
# Begin Source File

SOURCE=.\pool.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pool.h
# End Source File
# Begin Source File

SOURCE=.\tuparray.h
# End Source File
# Begin Source File

SOURCE=.\tuparray.hpp
# End Source File
# Begin Source File

SOURCE=.\tupSort.h
# End Source File
# Begin Source File

SOURCE=.\tupSort.hpp
# End Source File
# Begin Source File

SOURCE=.\tupstring.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tupstring.h
# End Source File
# Begin Source File

SOURCE=.\tupstring.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Atom.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Atom.h
# End Source File
# End Group
# Begin Group "Load/Save"

# PROP Default_Filter ""
# Begin Group "Binary"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\binaryloadsave.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\binaryloadsave.h
# End Source File
# Begin Source File

SOURCE=.\stringmanager.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stringmanager.h
# End Source File
# End Group
# Begin Group "Ascii"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Codec.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Codec.h
# End Source File
# Begin Source File

SOURCE=.\Reader.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Reader.h
# End Source File
# Begin Source File

SOURCE=.\Reader.hpp
# End Source File
# Begin Source File

SOURCE=.\Writer.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Writer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\loadsave.cpp

!IF  "$(CFG)" == "tupperware - Win32 Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Max Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "tupperware - PS2Direct Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Debug"

!ELSEIF  "$(CFG)" == "tupperware - GameCube Release"

!ELSEIF  "$(CFG)" == "tupperware - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tupperware - GameCube CDROM"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "tupperware - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "tupperware - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\loadsave.h
# End Source File
# End Group
# Begin Group "docs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\docs\Tupperware.txt
# End Source File
# End Group
# End Target
# End Project
