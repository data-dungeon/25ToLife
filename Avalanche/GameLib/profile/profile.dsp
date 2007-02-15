# Microsoft Developer Studio Project File - Name="Profile" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Profile - Win32 PS2 EE CDRom Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "profile.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "profile.mak" CFG="Profile - Win32 PS2 EE CDRom Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Profile - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - PS2 CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - Win32 DirectX Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - Win32 DirectX Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - PS2Direct Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - PS2Direct Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - GameCube Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - GameCube Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - GameCube CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - Win32 PS2 EE CDRom Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Profile - Win32 GameCube Release DVDETH" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/profile", VGQAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /Yu"Profile\ProfilePCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MMX_OK=$(MMX_OK) /D "WIN32" /FR /Yu"Profile\ProfilePCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

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

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

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

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

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
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "CW" /D "CDROM" /D "SNOW_REMOVING_EA_OPENGL" /Yu"Profile\ProfilePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2LINK:$(AVPATH)\Game\Avalanche.cmd
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

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
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /Yu"Profile\ProfilePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

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
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /I "$(HASHPATH)\Include" /D "_DEBUG" /D "DIRECTX" /D "WIN32" /D MMX_OK=$(MMX_OK) /FR /Yu"Profile\ProfilePCH.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

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
# ADD BASE CPP /nologo /w /W0 /O2 /X /I "$(AVPATH)" /I "$(HASHPATH)\Include" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Zs /c
# ADD CPP /nologo /w /W0 /Zi /O2 /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "NDEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Yu"Profile\ProfilePCH.h" /Zs /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

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
# ADD CPP /nologo /w /W0 /Gm /Zi /Od /X /I "$(HASHPATH)\Include" /I "$(AVPATH)" /I "$(PS2_ROOT)/ee/gcc/lib/gcc-lib/ee/3.0.3/include" /I "$(PS2_ROOT)/ee/gcc/ee/include" /I "$(PS2_ROOT)\ee\include" /I "$(AVGAME)" /FI"CW_MSVC.h" /D "_DEBUG" /D "PS2" /D "_MBCS" /D "_LIB" /D "CW" /Yu"Profile\ProfilePCH.h" /Zs /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf
# ADD LIB32 /nologo /PS2CWLINK:LinkSegment_PS2.lcf

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "_DEBUG" /Yu"Profile\ProfilePCH.h" /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

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
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Profile\ProfilePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Profile___Win32_GameCube_CDROM"
# PROP BASE Intermediate_Dir "Profile___Win32_GameCube_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_CDROM"
# PROP Intermediate_Dir "GCN_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"Profile\ProfilePCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "CDROM" /Yu"Profile\ProfilePCH.h" /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /include:"GCN" /include:"CW"
# ADD LIB32 /nologo /include:"GCN" /include:"CW"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Profile___Win32_PS2_EE_CDRom_Release"
# PROP BASE Intermediate_Dir "Profile___Win32_PS2_EE_CDRom_Release"
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

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Profile___Win32_GameCube_Release_DVDETH"
# PROP BASE Intermediate_Dir "Profile___Win32_GameCube_Release_DVDETH"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GCN_Release_DVDETH"
# PROP Intermediate_Dir "GCN_Release_DVDETH"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /Yu"Profile\ProfilePCH.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /ZI /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "GCN" /D "CW" /D "NDEBUG" /D "DVDETH" /Yu"Profile\ProfilePCH.h" /c
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

# Name "Profile - Win32 OpenGL Release"
# Name "Profile - Win32 OpenGL Debug"
# Name "Profile - Win32 PS2 EE Release"
# Name "Profile - Win32 PS2 EE Debug"
# Name "Profile - PS2 CDRom Release"
# Name "Profile - Win32 DirectX Release"
# Name "Profile - Win32 DirectX Debug"
# Name "Profile - PS2Direct Release"
# Name "Profile - PS2Direct Debug"
# Name "Profile - GameCube Debug"
# Name "Profile - GameCube Release"
# Name "Profile - GameCube CDROM"
# Name "Profile - Win32 PS2 EE CDRom Release"
# Name "Profile - Win32 GameCube Release DVDETH"
# Begin Group "GCN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GCN\gcntimer.cpp
DEP_CPP_GCNTI=\
	"..\..\enginenew\layers\Assert\Assert.h"\
	"..\..\enginenew\layers\LayersStruct.h"\
	"..\..\enginenew\layers\Memory.h"\
	"..\..\enginenew\Math\gcn\Math.hpp"\
	"..\..\enginenew\Math\Generic\Math.hpp"\
	"..\..\enginenew\Math\MathClass.h"\
	"..\..\enginenew\Math\MathDeprecate.h"\
	"..\..\enginenew\Math\PS2\Math.hpp"\
	"..\..\enginenew\Math\win32\Math.hpp"\
	"..\container\dllist.h"\
	"..\container\dllist.hpp"\
	"..\platform\basetype.h"\
	".\codetimer.h"\
	".\codetimer.hpp"\
	".\cputimer.h"\
	".\ctpage.h"\
	".\ctpage.hpp"\
	".\GCN\gcntimer.h"\
	".\log.h"\
	".\logtime.h"\
	".\ProfilePCH.h"\
	".\PS2\ps2timer.h"\
	".\textpage.h"\
	".\textrect.h"\
	".\textrect.hpp"\
	".\textscr.h"\
	".\textwin.h"\
	".\textwin.hpp"\
	".\Win32\auxtext.h"\
	".\Win32\auxtext.hpp"\
	".\Win32\textscrw32.h"\
	".\Win32\w32timer.h"\
	".\Win32\w32timer.hpp"\
	".\xbox\xboxtimer.h"\
	".\xbox\xboxtimer.hpp"\

NODEP_CPP_GCNTI=\
	".\acTypes.h"\
	".\etypes.h"\
	".\n_registers.h"\
	".\olphin\gx\GXEnum.h"\
	".\olphin\gx\GXStruct.h"\
	".\olphin\os.h"\
	".\olphin\os\OSAlarm.h"\
	".\olphin\os\OSAlloc.h"\
	".\olphin\os\OSCache.h"\
	".\olphin\os\OSContext.h"\
	".\olphin\os\OSDC.h"\
	".\olphin\os\OSError.h"\
	".\olphin\os\OSException.h"\
	".\olphin\os\OSExpansion.h"\
	".\olphin\os\OSFastCast.h"\
	".\olphin\os\OSFont.h"\
	".\olphin\os\OSIC.h"\
	".\olphin\os\OSInterrupt.h"\
	".\olphin\os\OSL2.h"\
	".\olphin\os\OSLC.h"\
	".\olphin\os\OSMessage.h"\
	".\olphin\os\OSModule.h"\
	".\olphin\os\OSMutex.h"\
	".\olphin\os\OSReset.h"\
	".\olphin\os\OSResetSW.h"\
	".\olphin\os\OSSemaphore.h"\
	".\olphin\os\OSSerial.h"\
	".\olphin\os\OSThread.h"\
	".\olphin\types.h"\
	".\olphin\vi\vitypes.h"\
	".\tl.h"\


!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\GCN\gcntimer.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF

# End Source File
# End Group
# Begin Group "PS2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2\ps2timer.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\PS2\ps2timer.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Win32\auxtext.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\auxtext.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\auxtext.hpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\textscrw32.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\textscrw32.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\w32timer.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\w32timer.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win32\w32timer.hpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Analyze.h
# End Source File
# Begin Source File

SOURCE=.\codetimer.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\codetimer.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\codetimer.hpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cputimer.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ctpage.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ctpage.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ctpage.hpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\log.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\log.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\logtime.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\logtime.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProfilePCH.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# ADD BASE CPP /Yc"Profile\ProfilePCH.h"
# ADD CPP /Yc"Profile\ProfilePCH.h"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# ADD BASE CPP /Yc"Profile\ProfilePCH.h"
# ADD CPP /Yc"Profile\ProfilePCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProfilePCH.h
# End Source File
# Begin Source File

SOURCE=.\ProfilePCH_SN.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textpage.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textpage.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textrect.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textrect.hpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textscr.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textscr.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textwin.cpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textwin.h

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\textwin.hpp

!IF  "$(CFG)" == "Profile - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 OpenGL Debug"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - PS2 CDRom Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Release"

!ELSEIF  "$(CFG)" == "Profile - Win32 DirectX Debug"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Release"

!ELSEIF  "$(CFG)" == "Profile - PS2Direct Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Debug"

!ELSEIF  "$(CFG)" == "Profile - GameCube Release"

!ELSEIF  "$(CFG)" == "Profile - GameCube CDROM"

!ELSEIF  "$(CFG)" == "Profile - Win32 PS2 EE CDRom Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Profile - Win32 GameCube Release DVDETH"

!ENDIF 

# End Source File
# End Target
# End Project
