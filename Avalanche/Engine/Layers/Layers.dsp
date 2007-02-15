# Microsoft Developer Studio Project File - Name="Layers" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Layers - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Layers.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Layers.mak" CFG="Layers - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Layers - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Layers - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/Layers", AAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /D MMX_OK=$(MMX_OK) /FR /Yu"Layers/LayersPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "OPENGL" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"Layers/LayersPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /FR /Yu"Layers/LayersPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"Layers/LayersPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "$(PS2_ROOT)\common\include" /FI"CW_MSVC.h" /D "PS2" /D "CW" /Yu"Layers/LayersPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /X /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "_DEBUG" /D "CW" /Yu"Layers/LayersPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 "PS2" "CW" /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(PS2_ROOT)\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "PS2" /D "CW" /D "CDROM" /Yu"Layers/LayersPCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G8 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PS2_EE_Release\Layers_sn.lib" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

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
# ADD CPP /nologo /w /W0 /Zi /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" -Wa,-sn /debug -mvu0-use-vf0-vf15 -fbyte-bool /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PS2_EE_Debug\Layers_sn.lib" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Layers___Win32_GameCube_Debug"
# PROP BASE Intermediate_Dir "Layers___Win32_GameCube_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GCN_Debug"
# PROP Intermediate_Dir "GCN_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "GCN" /D "CW" /Yu"Layers/LayersPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Layers___Win32_GameCube_Release"
# PROP BASE Intermediate_Dir "Layers___Win32_GameCube_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release"
# PROP Intermediate_Dir "GCN_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /D "WIN32" /D "OPENGL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "GCN" /D "CW" /Yu"Layers/LayersPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Layers___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "Layers___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "GCN" /D "CW" /Yu"Layers/LayersPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "GCN" /D "CW" /D "CDROM" /Yu"Layers/LayersPCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Layers___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "Layers___Win32_PS2_EE_CDRom_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDRom_Release"
# PROP Intermediate_Dir "PS2_EE_CDRom_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /w /W0 /Zi /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "C:\usr\local\sce\ee\gcc\ee\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "CDROM" -Wa,-sn -Winline -mvu0-use-vf0-vf15 -fbyte-bool -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions -G16 -mgpopt -fdefault-single-float /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"PS2_EE_Release\Layers_sn.lib" /D:SN_TARGET_PS2
# ADD LIB32 /nologo /out:"PS2_EE_CDRom_Release\Layers_sn.lib" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Layers___Win32_Debug"
# PROP BASE Intermediate_Dir "Layers___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Layers___Win32_Debug"
# PROP Intermediate_Dir "Layers___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D "OPENGL" /D MMX_OK=$(MMX_OK) /FR /Yu"Layers/LayersPCH.h" /FD /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "WIN32" /D "_DEBUG" /D MMX_OK=$(MMX_OK) /D "TOOLS" /FR /Yu"Layers/LayersPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Layers___Win32_Release"
# PROP BASE Intermediate_Dir "Layers___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Layers___Win32_Release"
# PROP Intermediate_Dir "Layers___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "OPENGL" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TUPPERWARE_FOR_ENGINE" /Yu"Layers/LayersPCH.h" /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D MMX_OK=$(MMX_OK) /D "TOOLS" /Yu"Layers/LayersPCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=BSCMake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Layers___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "Layers___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "GCN" /D "CW" /Yu"Layers/LayersPCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "GCN" /D "CW" /D "DVDETH" /Yu"Layers/LayersPCH.h" /c
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

# Name "Layers - Win32 OpenGL Debug"
# Name "Layers - Win32 OpenGL Release"
# Name "Layers - Win32 DirectX Debug"
# Name "Layers - Win32 DirectX Release"
# Name "Layers - PS2Direct Release"
# Name "Layers - PS2Direct Debug"
# Name "Layers - PS2 CDRom Release"
# Name "Layers - Win32 PS2 EE Release"
# Name "Layers - Win32 PS2 EE Debug"
# Name "Layers - GameCube Debug"
# Name "Layers - GameCube Release"
# Name "Layers - GameCube CDROM"
# Name "Layers - Win32 PS2 EE CDRom Release"
# Name "Layers - Win32 Debug"
# Name "Layers - Win32 Release"
# Name "Layers - Win32 GameCube Release DVDETH"
# Begin Group "Assert"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Assert\assert.cpp
DEP_CPP_ASSER=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Assert\Assert.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Assert\assert_gcn.cpp
DEP_CPP_ASSERT=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\ctroller.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\platform\systemi.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Assert\assert_ps2.cpp

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Assert\assert_win32.cpp

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Assert\parseelf.cpp

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Assert\parseelf.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Context\cont386.asm

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\WGL_Debug
InputPath=.\Context\cont386.asm
InputName=cont386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe -Zi -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\WGL_Release
InputPath=.\Context\cont386.asm
InputName=cont386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe -Zi -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\WDX_Debug
InputPath=.\Context\cont386.asm
InputName=cont386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe -Zi -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\WDX_Release
InputPath=.\Context\cont386.asm
InputName=cont386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe -Zi -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\Layers___Win32_Debug
InputPath=.\Context\cont386.asm
InputName=cont386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe -Zi -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\Layers___Win32_Release
InputPath=.\Context\cont386.asm
InputName=cont386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml.exe -Zi -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\context.cpp
DEP_CPP_CONTE=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Context.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Context\contgcn.s

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\GCN_Debug
InputPath=.\Context\contgcn.s
InputName=contgcn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CWFOLDER)\PowerPC_EABI_Tools\Command_Line_tools\mwasmeppc.exe" -g -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\GCN_Release
InputPath=.\Context\contgcn.s
InputName=contgcn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CWFOLDER)\PowerPC_EABI_Tools\Command_Line_tools\mwasmeppc.exe" -g -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\GCN_CDROM
InputPath=.\Context\contgcn.s
InputName=contgcn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CWFOLDER)\PowerPC_EABI_Tools\Command_Line_tools\mwasmeppc.exe" -g -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\GCN_Release_DVDETH
InputPath=.\Context\contgcn.s
InputName=contgcn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CWFOLDER)\PowerPC_EABI_Tools\Command_Line_tools\mwasmeppc.exe" -g -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Context\contps2.s

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2Direct_Release
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CWFOLDER)\PS2_Tools\Command_Line_tools\asm_r5900_elf.exe" -endian little -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2Direct_Debug
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CWFOLDER)\PS2_Tools\Command_Line_tools\asm_r5900_elf.exe" -endian little -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2_CDRom_Release
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CWFOLDER)\PS2_Tools\Command_Line_tools\asm_r5900_elf.exe" -endian little -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2_EE_Release
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%PS2_ROOT%\ee\gcc\ee\bin\ps2eeas.exe -sn -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2_EE_Debug
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%PS2_ROOT%\ee\gcc\ee\bin\ps2eeas.exe -sn -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2_EE_CDRom_Release
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%PS2_ROOT%\ee\gcc\ee\bin\ps2eeas.exe -sn -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\debug.cpp
DEP_CPP_DEBUG=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Debug.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DLinkList.cpp
DEP_CPP_DLINK=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DLinkList.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DMASom.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\event.cpp

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Event.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hog.cpp
DEP_CPP_HOG_C=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hog.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hogtoc.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\joystick.cpp
DEP_CPP_JOYST=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Joystick.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LayersPCH.cpp
DEP_CPP_LAYER=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# ADD BASE CPP /Yc"Layers/LayersPCH.h"
# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# ADD BASE CPP /Yc"Layers/LayersPCH.h"
# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# ADD BASE CPP /Yc"Layers/LayersPCH.h"
# ADD CPP /Yc"Layers/LayersPCH.h"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Layers/LayersPCH.h"
# ADD CPP /Yc"Layers/LayersPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LayersPCH.h
# End Source File
# Begin Source File

SOURCE=.\LayersPCH_SN.cpp

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LayersState.cpp
DEP_CPP_LAYERS=\
	"..\..\gamelibnew\cmdcon\ccondisp.h"\
	"..\..\gamelibnew\cmdcon\cmdcon.h"\
	"..\..\gamelibnew\cmdcon\convar.h"\
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
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\enginehelper\hog.h"\
	"..\..\gamelibnew\enginehelper\timer.h"\
	"..\..\gamelibnew\enginehelper\timer.hpp"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\ctroller.h"\
	"..\..\gamelibnew\platform\game.h"\
	"..\..\gamelibnew\platform\gameapp.h"\
	"..\..\gamelibnew\platform\gameinit.h"\
	"..\..\gamelibnew\platform\gameloop.h"\
	"..\..\gamelibnew\platform\GCN\gcnapp.h"\
	"..\..\gamelibnew\platform\GCN\gcncard.h"\
	"..\..\gamelibnew\platform\GCN\gcnccd.h"\
	"..\..\gamelibnew\platform\GCN\gcnctrol.h"\
	"..\..\gamelibnew\platform\GCN\gcninput.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\..\gamelibnew\platform\memcard.h"\
	"..\..\gamelibnew\platform\PS2\PS2app.h"\
	"..\..\gamelibnew\platform\PS2\ps2card.h"\
	"..\..\gamelibnew\platform\PS2\ps2ccd.h"\
	"..\..\gamelibnew\platform\PS2\ps2ctrol.h"\
	"..\..\gamelibnew\platform\PS2\ps2input.h"\
	"..\..\gamelibnew\platform\systemi.h"\
	"..\..\gamelibnew\platform\Win32\Winapp.h"\
	"..\..\gamelibnew\platform\Win32\wincard.h"\
	"..\..\gamelibnew\platform\Win32\winccd.h"\
	"..\..\gamelibnew\platform\Win32\winctrol.h"\
	"..\..\gamelibnew\platform\Win32\wininput.h"\
	"..\..\gamelibnew\platform\xbox\xboxapp.h"\
	"..\..\gamelibnew\platform\xbox\xboxcard.h"\
	"..\..\gamelibnew\platform\xbox\xboxccd.h"\
	"..\..\gamelibnew\platform\xbox\xboxctrol.h"\
	"..\..\gamelibnew\platform\xbox\xboxinput.h"\
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
	"..\Audio\PS2\sdriver.h"\
	"..\Audio\ps2\sound.h"\
	"..\Audio\ps2\sounderror.h"\
	"..\Audio\ps2\soundstream.h"\
	"..\Audio\PS2\streamirx.h"\
	"..\Audio\PS2\streamlib.h"\
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
	"..\display\common\matrixstack.h"\
	"..\display\common\renderstate.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\GSState.h"\
	"..\display\PS2Renderer\GSState.hpp"\
	"..\display\PS2Renderer\RenderState.hpp"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\DBFile.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dolphin\vm.h"\
	{$(INCLUDE)}"dolphin\vm\VMMapping.h"\
	{$(INCLUDE)}"dolphin\vm\VMPageReplacement.h"\
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
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
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LayersState.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LayersStruct.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\media.cpp
DEP_CPP_MEDIA=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Media.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mediahal.cpp
DEP_CPP_MEDIAH=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\enginehelper\timer.h"\
	"..\..\gamelibnew\enginehelper\timer.hpp"\
	"..\..\gamelibnew\Environ\EnvDataMgr.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\common\renderstate.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\display\PS2Renderer\CompilerControl.h"\
	"..\display\PS2Renderer\GSState.h"\
	"..\display\PS2Renderer\GSState.hpp"\
	"..\display\PS2Renderer\RenderState.hpp"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\LightMgr.h"\
	"..\game\Managers\RenderMgr.h"\
	"..\game\managers\rendermgr.hpp"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\mhal_gcn.cpp"\
	".\mhal_ps2.cpp"\
	".\mhal_w32.cpp"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dolphin\base\PPCArch.h"\
	{$(INCLUDE)}"dolphin\base\PPCWGPipe.h"\
	{$(INCLUDE)}"dolphin\card.h"\
	{$(INCLUDE)}"dolphin\db.h"\
	{$(INCLUDE)}"dolphin\db\DBInterface.h"\
	{$(INCLUDE)}"dolphin\dsp.h"\
	{$(INCLUDE)}"dolphin\dvd.h"\
	{$(INCLUDE)}"dolphin\dvdeth.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eestruct.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mediahal.h
# End Source File
# Begin Source File

SOURCE=.\memory.cpp
DEP_CPP_MEMOR=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\enginehelper\filestream.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Memory.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mhal_gcn.cpp

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mhal_ps2.cpp

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mhal_w32.cpp

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SLinkList.cpp
DEP_CPP_SLINK=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SLinkList.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stack.cpp
DEP_CPP_STACK=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stack.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tasking.cpp
DEP_CPP_TASKI=\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Tasking.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Timing.cpp
DEP_CPP_TIMIN=\
	"..\..\gamelibnew\cmdcon\ccondisp.h"\
	"..\..\gamelibnew\cmdcon\cmdcon.h"\
	"..\..\gamelibnew\cmdcon\convar.h"\
	"..\..\gamelibnew\container\Darray.h"\
	"..\..\gamelibnew\container\darray.hpp"\
	"..\..\gamelibnew\container\dllist.h"\
	"..\..\gamelibnew\container\dllist.hpp"\
	"..\..\gamelibnew\container\dynlist.h"\
	"..\..\gamelibnew\container\dynlist.hpp"\
	"..\..\gamelibnew\enginehelper\drawutil.h"\
	"..\..\gamelibnew\enginehelper\filestream.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.h"\
	"..\..\gamelibnew\linearalgebra\rayseg.hpp"\
	"..\..\gamelibnew\platform\basetype.h"\
	"..\..\gamelibnew\platform\compilercontrol.h"\
	"..\..\gamelibnew\platform\GCN\GCNMWPort.h"\
	"..\display\common\matrixstack.h"\
	"..\display\Common\SceneView.h"\
	"..\display\common\texturemgr.h"\
	"..\display\DirectX\DirectX.h"\
	"..\display\GCN\GCN.h"\
	"..\display\opengl\glext.h"\
	"..\display\opengl\opengl.h"\
	"..\game\Database\DataTypes.h"\
	"..\game\Database\Som.h"\
	"..\game\Managers\SceneMgr.h"\
	"..\game\Misc\Bounds.h"\
	"..\game\Misc\Frustum.h"\
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
	".\Assert\Assert.h"\
	".\Context.h"\
	".\Debug.h"\
	".\DLinkList.h"\
	".\hog.h"\
	".\hogtoc.h"\
	".\Joystick.h"\
	".\LayersPCH.h"\
	".\LayersState.h"\
	".\LayersStruct.h"\
	".\Media.h"\
	".\mediahal.h"\
	".\Memory.h"\
	".\SLinkList.h"\
	".\Stack.h"\
	".\Tasking.h"\
	".\Timing.h"\
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
	{$(INCLUDE)}"dxerr8.h"\
	{$(INCLUDE)}"dxfile.h"\
	{$(INCLUDE)}"eeregs.h"\
	{$(INCLUDE)}"eetypes.h"\
	{$(INCLUDE)}"Files.h"\
	{$(INCLUDE)}"Gestalt.h"\
	{$(INCLUDE)}"libcdvd.h"\
	{$(INCLUDE)}"libpc.h"\
	{$(INCLUDE)}"MacTypes.h"\
	{$(INCLUDE)}"OSUtils.h"\
	{$(INCLUDE)}"sifdev.h"\
	{$(INCLUDE)}"sn_registers.h"\
	{$(INCLUDE)}"win32\glext.h"\
	{$(INCLUDE)}"win32\win32.h"\
	{$(INCLUDE)}"xtl.h"\
	

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Timing.h

!IF  "$(CFG)" == "Layers - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Layers - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Layers - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - GameCube Debug"

!ELSEIF  "$(CFG)" == "Layers - GameCube Release"

!ELSEIF  "$(CFG)" == "Layers - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Layers - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Layers - Win32 Debug"

!ELSEIF  "$(CFG)" == "Layers - Win32 Release"

!ELSEIF  "$(CFG)" == "Layers - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
