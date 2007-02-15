# Microsoft Developer Studio Project File - Name="Display" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Display - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "Display.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Display.mak" CFG="Display - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Display - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Display - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/Display", ETAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(HASHPATH)\Include" /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /D MMX_OK=$(MMX_OK) /FR /Yu"Display\DisplayPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "OPENGL" /D MMX_OK=$(MMX_OK) /FR /Yu"Display\DisplayPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "_DEBUG" /D "OPENGL" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /FR /Yu"Display\DisplayPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

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
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "WIN32" /D "OPENGL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /FR /Yu"Display\DisplayPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "OPENGL" /D "PS2" /D "CW" /YX /FD /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "." /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "CW" /D "AVALANCHE" /Yu"Display\DisplayPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

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
# ADD BASE CPP /nologo /w /W0 /Od /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "OPENGL" /D "DEBUG" /D "PS2" /D "_DEBUG" /D "CW" /FR /YX /FD /Zs /c
# ADD CPP /nologo /w /W0 /Zi /Od /X /I "." /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "DEBUG" /D "PS2" /D "_DEBUG" /D "CW" /D "AVALANCHE" /Yu"Display\DisplayPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Display___Win32_CDRom_Release"
# PROP BASE Intermediate_Dir "Display___Win32_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_CDRom_Release"
# PROP Intermediate_Dir "PS2_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /FI"CW_MSVC.h" /D "OPENGL" /D "PS2" /D "CW" /D "AVALANCHE" /YX /FD /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "." /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "CW" /D "AVALANCHE" /D "CDROM" /Yu"Display\DisplayPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "AVALANCHE" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "AVALANCHE" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Display___Win32_GameCube_Debug0"
# PROP BASE Intermediate_Dir "Display___Win32_GameCube_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Display\DisplayPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Display___Win32_GameCube_Release0"
# PROP BASE Intermediate_Dir "Display___Win32_GameCube_Release0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "OPENGL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Display\DisplayPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Display___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "Display___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Display\DisplayPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Display\DisplayPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Display___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "Display___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "AVALANCHE" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "AVALANCHE" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Display___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "Display___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Display\DisplayPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(AVGAME)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Display\DisplayPCH.h" /c
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

# Name "Display - Win32 OpenGL Debug"
# Name "Display - Win32 OpenGL Release"
# Name "Display - Win32 DirectX Debug"
# Name "Display - Win32 DirectX Release"
# Name "Display - PS2Direct Release"
# Name "Display - PS2Direct Debug"
# Name "Display - PS2 CDRom Release"
# Name "Display - Win32 PS2 EE Release"
# Name "Display - Win32 PS2 EE Debug"
# Name "Display - GameCube Debug"
# Name "Display - GameCube Release"
# Name "Display - GameCube CDROM"
# Name "Display - Win32 PS2 EE CDRom Release"
# Name "Display - Win32 GameCube Release DVDETH"
# Begin Group "OpenGL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OpenGL\buffcomm.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\OpenGL\DrawArray.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\opengl\glext.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\OpenGL\MatrixStack.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\OpenGL\MultiTex.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\opengl\opengl.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenGL\Platform.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenGL\RenderState.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenGL\StripBuffer.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenGL\TextureBind.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenGL\TextureState.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenGL\VtxBuffer.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenGL\VtxBuffer.hpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "DirectX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DirectX\buffcomm.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\DirectX.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\DrawArray.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\DrawList.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\MatrixStack.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\Platform.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\RenderState.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\StripBuffer.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\TextureBind.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\TextureState.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\VtxBuffer.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectX\VtxBuffer.hpp
# End Source File
# End Group
# Begin Group "GCN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GCN\buffcomm.cpp
DEP_CPP_BUFFC=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\DrawArray.cpp
DEP_CPP_DRAWA=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\FrameData.h
# End Source File
# Begin Source File

SOURCE=.\GCN\GCN.h
# End Source File
# Begin Source File

SOURCE=.\GCN\MatrixStack.cpp
DEP_CPP_MATRI=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\Platform.cpp
DEP_CPP_PLATF=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\RenderState.cpp
DEP_CPP_RENDE=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\Shader.cpp
DEP_CPP_SHADE=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"Dolphin\gd.h"\
	{$(INCLUDE)}"dolphin\gd\GDBase.h"\
	{$(INCLUDE)}"dolphin\gd\GDFile.h"\
	{$(INCLUDE)}"dolphin\gd\GDGeometry.h"\
	{$(INCLUDE)}"dolphin\gd\GDLight.h"\
	{$(INCLUDE)}"dolphin\gd\GDPixel.h"\
	{$(INCLUDE)}"dolphin\gd\GDTev.h"\
	{$(INCLUDE)}"dolphin\gd\GDTexture.h"\
	{$(INCLUDE)}"dolphin\gd\GDTransform.h"\
	{$(INCLUDE)}"dolphin\gd\GDVert.h"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\Shader.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\Shader.hpp
# End Source File
# Begin Source File

SOURCE=.\GCN\StripBuffer.cpp
DEP_CPP_STRIP=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\TextureBind.cpp
DEP_CPP_TEXTU=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\FrameData.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"Dolphin\gd.h"\
	{$(INCLUDE)}"dolphin\gd\GDBase.h"\
	{$(INCLUDE)}"dolphin\gd\GDFile.h"\
	{$(INCLUDE)}"dolphin\gd\GDGeometry.h"\
	{$(INCLUDE)}"dolphin\gd\GDLight.h"\
	{$(INCLUDE)}"dolphin\gd\GDPixel.h"\
	{$(INCLUDE)}"dolphin\gd\GDTev.h"\
	{$(INCLUDE)}"dolphin\gd\GDTexture.h"\
	{$(INCLUDE)}"dolphin\gd\GDTransform.h"\
	{$(INCLUDE)}"dolphin\gd\GDVert.h"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\TextureState.cpp
DEP_CPP_TEXTUR=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\VtxBuffer.cpp
DEP_CPP_VTXBU=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GCN\VtxBuffer.hpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Group
# Begin Group "PS2Renderer"

# PROP Default_Filter ""
# Begin Group "VUCode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\newparticle.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\newparticle.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\Sprites.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer1x3CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer1x3CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x3CD.vsm
InputName=VUStripBuffer1x3CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x3CD.vsm
InputName=VUStripBuffer1x3CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x3CD.vsm
InputName=VUStripBuffer1x3CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x3CD.vsm
InputName=VUStripBuffer1x3CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x3CD.vsm
InputName=VUStripBuffer1x3CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x3CD.vsm
InputName=VUStripBuffer1x3CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vcl

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2Direct_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(PS2_DRIVE):\usr\local\sce\ee\gcc\bin\ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDRom_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\PS2Renderer\AnimDDMAObject.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\buffcomm.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\CompilerControl.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\DDMAObject.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GIFTag.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSMemManager.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSMemManager.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSState.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSState.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSTricks.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSTricks.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\libPS2Rend_cw_dbg.a

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\MasterList.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\MatrixStack.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\Platform.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\PRIMReg.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\PS2Direct.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\PS2Renderer.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\RenderState.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\RenderState.hpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\StripBuffer.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\TextureBind.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\TextureState.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VIFInline.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VtxBuffer.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VtxBuffer.hpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\common\buffcomm.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\CRenderState.cpp
DEP_CPP_CREND=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\CTextureState.cpp
DEP_CPP_CTEXT=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DisplayPCH.cpp
DEP_CPP_DISPL=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# ADD BASE CPP /Yc"Display\DisplayPCH.h"
# ADD CPP /Yc"Display\DisplayPCH.h"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Display\DisplayPCH.h"
# ADD CPP /Yc"Display\DisplayPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DisplayPCH.h
# End Source File
# Begin Source File

SOURCE=.\DisplayPCH_SN.cpp

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\DisplayPlatform.cpp
DEP_CPP_DISPLA=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\layers\Timing.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\FrameData.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\GSTricks.h"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\displayplatform.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\DisplayState.cpp
DEP_CPP_DISPLAY=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\layers\Timing.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\displaystate.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\Draw.cpp
DEP_CPP_DRAW_=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\draw.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\drawarray.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gfxfunc.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\matrixstack.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\PatchDisp.cpp
DEP_CPP_PATCH=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\AabbObject.h"\
	"..\..\gamelibnew\collide\AabbObject.hpp"\
	"..\..\gamelibnew\collide\AabbToAabb.h"\
	"..\..\gamelibnew\collide\AabbToAabb.hpp"\
	"..\..\gamelibnew\collide\AabbToObb.h"\
	"..\..\gamelibnew\collide\AabbToObb.hpp"\
	"..\..\gamelibnew\collide\AabbToTerrain.h"\
	"..\..\gamelibnew\collide\AabbToTerrain.hpp"\
	"..\..\gamelibnew\collide\cdbhdr.h"\
	"..\..\gamelibnew\collide\cube.h"\
	"..\..\gamelibnew\collide\cube.hpp"\
	"..\..\gamelibnew\collide\Cull.h"\
	"..\..\gamelibnew\collide\Intersection.h"\
	"..\..\gamelibnew\collide\Intersection.hpp"\
	"..\..\gamelibnew\collide\Line.h"\
	"..\..\gamelibnew\collide\Line.hpp"\
	"..\..\gamelibnew\collide\LineToAabb.h"\
	"..\..\gamelibnew\collide\LineToAabb.hpp"\
	"..\..\gamelibnew\collide\LineToObb.h"\
	"..\..\gamelibnew\collide\LineToObb.hpp"\
	"..\..\gamelibnew\collide\LineToSphere.h"\
	"..\..\gamelibnew\collide\LineToSphere.hpp"\
	"..\..\gamelibnew\collide\LineToTerrain.h"\
	"..\..\gamelibnew\collide\LineToTerrain.hpp"\
	"..\..\gamelibnew\collide\Node.h"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\ObbToObb.h"\
	"..\..\gamelibnew\collide\ObbToObb.hpp"\
	"..\..\gamelibnew\collide\ObbToTerrain.h"\
	"..\..\gamelibnew\collide\ObbToTerrain.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\octtree.h"\
	"..\..\gamelibnew\collide\octtree.hpp"\
	"..\..\gamelibnew\collide\otnode.h"\
	"..\..\gamelibnew\collide\otnode.hpp"\
	"..\..\gamelibnew\collide\ottestid.h"\
	"..\..\gamelibnew\collide\ottestid.hpp"\
	"..\..\gamelibnew\collide\Pair.h"\
	"..\..\gamelibnew\collide\Pair.hpp"\
	"..\..\gamelibnew\collide\Sphere.h"\
	"..\..\gamelibnew\collide\Sphere.hpp"\
	"..\..\gamelibnew\collide\SphereToObb.h"\
	"..\..\gamelibnew\collide\SphereToObb.hpp"\
	"..\..\gamelibnew\collide\SphereToTerrain.h"\
	"..\..\gamelibnew\collide\SphereToTerrain.hpp"\
	"..\..\gamelibnew\collide\Stack.h"\
	"..\..\gamelibnew\collide\Stack.hpp"\
	"..\..\gamelibnew\collide\StackArray.h"\
	"..\..\gamelibnew\collide\StackArray.hpp"\
	"..\..\gamelibnew\collide\tclayer.h"\
	"..\..\gamelibnew\collide\tclayer.hpp"\
	"..\..\gamelibnew\collide\tcllist.h"\
	"..\..\gamelibnew\collide\tcllist.hpp"\
	"..\..\gamelibnew\collide\Terrain.h"\
	"..\..\gamelibnew\collide\Terrain.hpp"\
	"..\..\gamelibnew\collide\terraintri.h"\
	"..\..\gamelibnew\collide\terraintri.hpp"\
	"..\..\gamelibnew\collide\TriangleList.h"\
	"..\..\gamelibnew\collide\TriangleList.hpp"\
	"..\..\gamelibnew\collide\tritest.h"\
	"..\..\gamelibnew\collide\tritest.hpp"\
	"..\..\gamelibnew\collide\tsuspect.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\obbchild.h"\
	"..\..\GameLibNew\CollideHelper\obbchild.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\container\u24.h"\
	"..\..\gamelibnew\container\u24.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\geomutil\planefit.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\linearalgebra\semiseg.h"\
	"..\..\gamelibnew\linearAlgebra\semiseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\layers\Timing.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\FrameData.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
	{$(INCLUDE)}"Collide\OBBChild.h"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\patchdisp.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\RenderMisc.cpp
DEP_CPP_RENDER=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\fastrand.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\rendermisc.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\renderstate.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\SceneView.cpp
DEP_CPP_SCENE=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\SceneView.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\StripBuffer.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\TextureAnimator.cpp
DEP_CPP_TEXTURE=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\TextureAnimator.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\TextureMgr.cpp
DEP_CPP_TEXTUREM=\
	"..\..\gamelibnew\collide\aabb.h"\
	"..\..\gamelibnew\collide\aabb.hpp"\
	"..\..\gamelibnew\collide\obb.h"\
	"..\..\gamelibnew\collide\obb.hpp"\
	"..\..\gamelibnew\collide\Object.h"\
	"..\..\gamelibnew\collide\xzrect.h"\
	"..\..\gamelibnew\collide\xzrect.hpp"\
	"..\..\GameLibNew\CollideHelper\zone.h"\
	"..\..\GameLibNew\CollideHelper\zonemap.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\container\fpool.h"\
	"..\..\gamelibnew\container\fpool.hpp"\
	"..\..\gamelibnew\container\pool.h"\
	"..\..\gamelibnew\container\pool.hpp"\
	"..\..\gamelibnew\enginehelper\cinst.h"\
	"..\..\gamelibnew\enginehelper\cinst.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\enginehelper\timer.h"\
	"..\..\gamelibnew\enginehelper\timer.hpp"\
	"..\..\gamelibnew\Environ\EnvDataMgr.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\stage\actortype.h"\
	"..\..\gamelibnew\stage\cactor.h"\
	"..\..\gamelibnew\stage\CActorSoundEmitter.h"\
	"..\..\gamelibnew\stage\CategoryBit.h"\
	"..\..\gamelibnew\stage\EntityCategory.h"\
	"..\..\gamelibnew\stage\EntityType.h"\
	"..\..\gamelibnew\stage\EntityType.hpp"\
	"..\..\gamelibnew\stage\InteractionCategory.h"\
	"..\..\gamelibnew\stage\InteractionCategory.hpp"\
	"..\..\gamelibnew\stage\protagonist.h"\
	"..\..\gamelibnew\stage\StageEntity.h"\
	"..\..\gamelibnew\stage\StageEntity.hpp"\
	"..\Audio\audiolib.h"\
	"..\Audio\audiosys.h"\
	"..\Audio\emitter\e3dparam.h"\
	"..\Audio\emitter\semit2d.h"\
	"..\Audio\emitter\semit3d.h"\
	"..\Audio\emitter\semits.h"\
	"..\Audio\emitter\semitter.h"\
	"..\Audio\environment\ambient.h"\
	"..\Audio\environment\ambsound.h"\
	"..\Audio\environment\envdbl.h"\
	"..\Audio\environment\envfield.h"\
	"..\Audio\environment\envsource.h"\
	"..\Audio\environment\envstrig.h"\
	"..\Audio\environment\semitenv.h"\
	"..\Audio\function\function.h"\
	"..\Audio\function\ramp.h"\
	"..\Audio\gcn\dspadpcm.h"\
	"..\Audio\GCN\sdriver.h"\
	"..\Audio\gcn\sound.h"\
	"..\Audio\gcn\sounderror.h"\
	"..\Audio\gcn\soundstream.h"\
	"..\Audio\group\reclaim.h"\
	"..\Audio\group\schan2d.h"\
	"..\Audio\group\schan3d.h"\
	"..\Audio\group\schannel.h"\
	"..\Audio\group\sgalias.h"\
	"..\Audio\group\sgroup.h"\
	"..\Audio\group\sgroup2d.h"\
	"..\Audio\group\sgroup3d.h"\
	"..\Audio\group\shandle.h"\
	"..\Audio\jukebox.h"\
	"..\Audio\listener.h"\
	"..\Audio\PS2\ass.h"\
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sdriver.hpp"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\vag.h"\
	"..\Audio\soundattr.h"\
	"..\Audio\soundbank.h"\
	"..\Audio\sounddriver.h"\
	"..\Audio\streamq.h"\
	"..\Audio\voiceq.h"\
	"..\Audio\Win32\sdriver.h"\
	"..\Audio\Win32\sdrivercaps.h"\
	"..\Audio\Win32\sdriverenum.h"\
	"..\Audio\win32\sound.h"\
	"..\Audio\win32\sounderror.h"\
	"..\Audio\win32\soundstream.h"\
	"..\Audio\Win32\wavefile.h"\
	"..\audio\xbox\sdriver.h"\
	"..\audio\xbox\sdrivercaps.h"\
	"..\audio\xbox\sound.h"\
	"..\audio\xbox\sounderror.h"\
	"..\audio\xbox\soundstream.h"\
	"..\game\Animation\Animproc.h"\
	"..\game\Animation\bone.h"\
	"..\game\Animation\Motion.h"\
	"..\game\Animation\MotionDB.h"\
	"..\game\Animation\MotnFlags.h"\
	"..\game\Database\DataHelpers.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\database\dynamicobject.h"\
	"..\game\Database\Instance.h"\
	"..\game\Database\lightset.h"\
	"..\game\Database\LOD.h"\
	"..\game\Database\Som.h"\
	"..\game\Database\World.h"\
	"..\game\Managers\AuraMgr.h"\
	"..\game\Managers\DistortionMgr.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Managers\ShadowMgr.h"\
	"..\game\Managers\SpriteMgr.h"\
	"..\game\Misc\Actor.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\FastVars.h"\
	"..\game\Misc\Frustum.h"\
	"..\layers\Assert\Assert.h"\
	"..\layers\Debug.h"\
	"..\layers\DLinkList.h"\
	"..\layers\hog.h"\
	"..\layers\LayersStruct.h"\
	"..\layers\Media.h"\
	"..\layers\mediahal.h"\
	"..\layers\Memory.h"\
	"..\layers\SLinkList.h"\
	"..\layers\Timing.h"\
	"..\Math\Det.h"\
	"..\Math\gcn\Mat4x4.hpp"\
	"..\Math\gcn\Math.hpp"\
	"..\Math\Generic\Det.hpp"\
	"..\Math\Generic\Mat3x3.hpp"\
	"..\Math\Generic\Mat4x4.hpp"\
	"..\Math\Generic\Math.hpp"\
	"..\Math\Generic\Matrix.hpp"\
	"..\Math\Generic\Vect2.hpp"\
	"..\Math\Generic\Vector.hpp"\
	"..\Math\Mat3x3.h"\
	"..\Math\Mat4x4.h"\
	"..\Math\MathClass.h"\
	"..\Math\MathDeprecate.h"\
	"..\Math\MathStats.h"\
	"..\Math\MathStruct.h"\
	"..\Math\MathUndeprecate.h"\
	"..\Math\Matrix.h"\
	"..\Math\PS2\Mat4x4.hpp"\
	"..\Math\PS2\Mat4x4sn.hpp"\
	"..\Math\PS2\Math.hpp"\
	"..\Math\PS2\Quaternsn.hpp"\
	"..\Math\Quatern.h"\
	"..\Math\Vect2.h"\
	"..\Math\Vector.h"\
	"..\Math\Vector3GCN.hpp"\
	"..\Math\Vector3Generic.hpp"\
	"..\Math\Vector3GenericPacked.h"\
	"..\Math\Vector3GenericPadded.h"\
	"..\Math\Vector3SNPS2.hpp"\
	"..\Math\Vector3SNPS2Padded.h"\
	"..\Math\Vector3SNPS2Padded.hpp"\
	"..\Math\Vector3Template.h"\
	"..\Math\Vector3Template.hpp"\
	"..\Math\Vector3WIN32.hpp"\
	"..\Math\Vector4.h"\
	"..\Math\Vector4GCN.hpp"\
	"..\Math\Vector4Generic.hpp"\
	"..\Math\Vector4GenericAligned.h"\
	"..\Math\Vector4GenericPacked.h"\
	"..\Math\Vector4SNPS2.hpp"\
	"..\Math\Vector4SNPS2Aligned.h"\
	"..\Math\Vector4SNPS2Aligned.hpp"\
	"..\Math\Vector4SNPS2X.h"\
	"..\Math\Vector4SNPS2X.hpp"\
	"..\Math\Vector4Template.h"\
	"..\Math\Vector4Template.hpp"\
	"..\Math\Vector4WIN32.hpp"\
	"..\Math\win32\Mat4x4.hpp"\
	"..\Math\win32\Math.hpp"\
	".\common\buffcomm.h"\
	".\common\displayplatform.h"\
	".\common\displaystate.h"\
	".\common\displaystate.hpp"\
	".\common\draw.h"\
	".\common\drawarray.h"\
	".\common\gfxfunc.h"\
	".\common\matrixstack.h"\
	".\common\patchdisp.h"\
	".\common\rendermisc.h"\
	".\common\renderstate.h"\
	".\Common\SceneView.h"\
	".\Common\StripBuffer.h"\
	".\Common\TextureAnimator.h"\
	".\common\texturemgr.h"\
	".\common\texturestate.h"\
	".\Common\VtxBuffer.h"\
	".\DirectX\DirectX.h"\
	".\DirectX\VtxBuffer.hpp"\
	".\DisplayPCH.h"\
	".\GCN\GCN.h"\
	".\GCN\Shader.h"\
	".\GCN\Shader.hpp"\
	".\GCN\VtxBuffer.hpp"\
	".\opengl\glext.h"\
	".\opengl\opengl.h"\
	".\OpenGL\VtxBuffer.hpp"\
	".\PS2Renderer\AnimDDMAObject.h"\
	".\PS2Renderer\CompilerControl.h"\
	".\PS2Renderer\DDMAObject.h"\
	".\PS2Renderer\DDMAObjectClassIDs.h"\
	".\PS2Renderer\GIFInline.h"\
	".\PS2Renderer\GIFTag.h"\
	".\PS2Renderer\GIFTag.hpp"\
	".\PS2Renderer\GSMemManager.h"\
	".\PS2Renderer\GSState.h"\
	".\PS2Renderer\GSState.hpp"\
	".\PS2Renderer\MasterList.h"\
	".\PS2Renderer\PRIMReg.h"\
	".\PS2Renderer\PS2.h"\
	".\PS2Renderer\PS2Renderer.h"\
	".\PS2Renderer\PSDRenderState.h"\
	".\PS2Renderer\RenderState.hpp"\
	".\PS2Renderer\VIFInline.h"\
	".\PS2Renderer\VtxBuffer.hpp"\
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
	{$(INCLUDE)}"eekernel.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libdma.h"\
	{$(INCLUDE)}"libgraph.h"\
	{$(INCLUDE)}"libvu0.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xgraphics.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\texturemgr.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\texturestate.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\VtxBuffer.h

!IF  "$(CFG)" == "Display - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Display - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Display - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Display - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - GameCube Debug"

!ELSEIF  "$(CFG)" == "Display - GameCube Release"

!ELSEIF  "$(CFG)" == "Display - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Display - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Display - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
