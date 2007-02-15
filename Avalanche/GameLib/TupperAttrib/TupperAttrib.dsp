# Microsoft Developer Studio Project File - Name="TupperAttrib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TupperAttrib - Win32 Maya Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "TupperAttrib.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "TupperAttrib.mak" CFG="TupperAttrib - Win32 Maya Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "TupperAttrib - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 Max Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 Maya Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 Maya Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TupperAttrib - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/TupperAttrib", LHGBAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Max Debug"
# PROP BASE Intermediate_Dir "Max Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "WIN32_MAX_DEBUG" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Max Hybrid"
# PROP BASE Intermediate_Dir "Max Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Max Release"
# PROP BASE Intermediate_Dir "Max Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "WIN32_MAX_RELEASE" /Yu"TupperAttribPch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TupperAttrib___Win32_OpenGL_Debug"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_OpenGL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WGL_Debug"
# PROP Intermediate_Dir "WGL_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TupperAttrib___Win32_OpenGL_Release"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_OpenGL_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WGL_Release"
# PROP Intermediate_Dir "WGL_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperAttribPch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD CPP /nologo /MTd /w /W0 /Gm /GX /Zi /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "PS2" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CW" /D "TUPPERWARE_FOR_ENGINE" /FD /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /c
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

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TupperAttrib___Win32_DirectX_Debug"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_DirectX_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WDX_Debug"
# PROP Intermediate_Dir "WDX_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TupperAttrib___Win32_DirectX_Release"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_DirectX_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WDX_Release"
# PROP Intermediate_Dir "WDX_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /c
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

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

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

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TupperAttrib___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C:\usr\local\sce\ee\gcc\lib\gcc-lib\ee\2.95.3\include" /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "TUPPERWARE_FOR_ENGINE" -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TupperAttrib___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"TupperAttribPch.h" /FD /c
# ADD CPP /nologo /MLd /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\gcc\lib\gcc-lib\ee\2.95.3\include" /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "TUPPERWARE_FOR_ENGINE" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"TupperAttrib___Win32_PS2_EE_Release\TupperAttrib.lib" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "_DEBUG" /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperAttribPch.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"TupperAttribPch.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "GCN" /D "CW" /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperAttribPch.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TupperAttrib___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MLd /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MLd /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\gcc\lib\gcc-lib\ee\2.95.3\include" /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" /D "TUPPERWARE_FOR_ENGINE" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /out:"TupperAttrib___Win32_PS2_EE_Release\TupperAttrib.lib" /D:SN_TARGET_PS2
# ADD LIB32 /nologo /out:"TupperAttrib___Win32_PS2_EE_Release\TupperAttrib.lib" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TupperAttrib___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "GCN" /D "CW" /Yu"TupperAttribPch.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "GCN" /D "CW" /D "CDROM" /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperAttribPch.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TupperAttrib___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_Maya_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_LIB" /D "_DEBUG" /D "WIN32_MAYA_DEBUG" /D "WIN32" /Yu"TupperAttribPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TupperAttrib___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_LIB" /Yu"TupperAttribPch.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_LIB" /D "WIN32_MAYA_RELEASE" /D "WIN32" /Yu"TupperAttribPch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TupperAttrib___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "TupperAttrib___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "GCN" /D "CW" /D "TUPPERWARE_FOR_ENGINE" /Yu"TupperAttribPch.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "GCN" /D "CW" /D "TUPPERWARE_FOR_ENGINE" /D "DVDETH" /Yu"TupperAttribPch.h" /c
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

# Name "TupperAttrib - Win32 Max Debug"
# Name "TupperAttrib - Win32 Max Hybrid"
# Name "TupperAttrib - Win32 Max Release"
# Name "TupperAttrib - Win32 OpenGL Debug"
# Name "TupperAttrib - Win32 OpenGL Release"
# Name "TupperAttrib - PS2Direct Debug"
# Name "TupperAttrib - PS2Direct Release"
# Name "TupperAttrib - Win32 DirectX Debug"
# Name "TupperAttrib - Win32 DirectX Release"
# Name "TupperAttrib - PS2 CDRom Release"
# Name "TupperAttrib - Win32 PS2 EE Debug"
# Name "TupperAttrib - Win32 PS2 EE Release"
# Name "TupperAttrib - GameCube Debug"
# Name "TupperAttrib - GameCube Release"
# Name "TupperAttrib - Win32 PS2 EE CDRom Release"
# Name "TupperAttrib - GameCube CDROM"
# Name "TupperAttrib - Win32 Maya Debug"
# Name "TupperAttrib - Win32 Maya Release"
# Name "TupperAttrib - Win32 GameCube Release DVDETH"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TupperAttribPch.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

# PROP BASE Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperAttribPch.h"
# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperAttribPch.h"
# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1
# ADD BASE CPP /Yc"TupperAttribPch.h"
# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

# ADD BASE CPP /Yc"TupperAttribPch.h"
# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

# ADD BASE CPP /Yc"TupperAttribPch.h"
# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1
# SUBTRACT BASE CPP /YX /Yc
# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

# ADD BASE CPP /Yc"TupperAttribPch.h"
# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

# ADD BASE CPP /Yc"TupperAttribPch.h"
# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

# ADD BASE CPP /Yc"TupperAttribPch.h"
# ADD CPP /Yc"TupperAttribPch.h"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"TupperAttribPch.h"
# ADD CPP /Yc"TupperAttribPch.h"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\TupperAttribPch.h
# End Source File
# Begin Source File

SOURCE=.\TupperAttribPch_SN.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# Begin Group "Config"

# PROP Default_Filter ""
# Begin Group "Override Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConfigOverride.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigOverride.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ConfigBase.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigBase.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigCheckBox.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigCheckBox.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigCheckList.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigCheckList.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigComboBox.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigComboBox.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigEditBox.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigEditBox.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigFreeColorBox.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigFreeColorBox.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigNumeric.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigNumeric.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigRadioGroup.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigRadioGroup.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigRoot.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigRoot.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigSelection.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigSelection.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigSetColorBox.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConfigSetColorBox.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "Key Value"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KeyValue.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KeyValue.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "Key Selection"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KeySelection.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KeySelection.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "Key Value Context"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KeyValueContext.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KeyValueContext.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "Access"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AccessInstance.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AccessInstance.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AccessPath.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AccessPath.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AccessRoot.cpp

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AccessRoot.h

!IF  "$(CFG)" == "TupperAttrib - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2Direct Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TupperAttrib - GameCube CDROM"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Debug"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 Maya Release"

!ELSEIF  "$(CFG)" == "TupperAttrib - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
