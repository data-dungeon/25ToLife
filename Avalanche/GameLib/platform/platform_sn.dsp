# Microsoft Developer Studio Project File - Name="platform_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=platform_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "platform_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "platform_sn.mak" CFG="platform_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "platform_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "platform_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "platform_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/platform", ODGAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "platform_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "platform_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "platform_sn___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /debug -mvu0-use-vf0-vf15 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "platform_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "platform_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "platform_sn___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "platform_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "platform_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "platform_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "SNOW_REMOVING_EA_OPENGL" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "A_COMPILER_SN" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "platform_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "platform_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "platform_sn___Win32_PS2_EE_PA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_PA"
# PROP Intermediate_Dir "PS2_EE_PA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -mlong-calls -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "platform_sn - Win32 PS2 EE Debug"
# Name "platform_sn - Win32 PS2 EE Release"
# Name "platform_sn - Win32 PS2 EE CDROM"
# Name "platform_sn - Win32 PS2 EE PA"
# Begin Group "ps2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2\PS2app.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\PS2app.h
# End Source File
# Begin Source File

SOURCE=.\PS2\ps2card.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\ps2card.h
# End Source File
# Begin Source File

SOURCE=.\PS2\ps2ccd.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\ps2ccd.h
# End Source File
# Begin Source File

SOURCE=.\PS2\ps2ctrol.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\ps2ctrol.h
# End Source File
# Begin Source File

SOURCE=.\PS2\ps2input.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\ps2input.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BaseType.h
# End Source File
# Begin Source File

SOURCE=.\ctroller.cpp
# End Source File
# Begin Source File

SOURCE=.\ctroller.h
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\gameapp.cpp
# End Source File
# Begin Source File

SOURCE=.\gameapp.h
# End Source File
# Begin Source File

SOURCE=.\gameinit.h
# End Source File
# Begin Source File

SOURCE=.\gameloop.h
# End Source File
# Begin Source File

SOURCE=.\keyctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\keyctrl.h
# End Source File
# Begin Source File

SOURCE=.\memcard.cpp
# End Source File
# Begin Source File

SOURCE=.\memcard.h
# End Source File
# Begin Source File

SOURCE=.\systemi.cpp
# End Source File
# Begin Source File

SOURCE=.\systemi.h
# End Source File
# End Target
# End Project
