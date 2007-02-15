# Microsoft Developer Studio Project File - Name="audio_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=audio_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "audio_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "audio_sn.mak" CFG="audio_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "audio_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "audio_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audio_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "audio_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/Audio", IAWAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "audio_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "audio_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "audio_sn___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "A_COMPILER_SN" /FD -Wa,-sn /debug -mvu0-use-vf0-vf15 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "audio_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "audio_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "audio_sn___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "A_COMPILER_SN" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "audio_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "audio_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "audio_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /D "A_COMPILER_SN" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "audio_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "audio_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "audio_sn___Win32_PS2_EE_PA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_PA"
# PROP Intermediate_Dir "PS2_EE_PA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /D "A_COMPILER_SN" /FD -mlong-calls -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
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

# Name "audio_sn - Win32 PS2 EE Debug"
# Name "audio_sn - Win32 PS2 EE Release"
# Name "audio_sn - Win32 PS2 EE CDROM"
# Name "audio_sn - Win32 PS2 EE PA"
# Begin Group "emitter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Emitter\e3dparam.cpp
# End Source File
# Begin Source File

SOURCE=.\Emitter\e3dparam.h
# End Source File
# Begin Source File

SOURCE=.\Emitter\envstrig.cpp
# End Source File
# Begin Source File

SOURCE=.\Emitter\envstrig.h
# End Source File
# Begin Source File

SOURCE=.\Emitter\semit2d.cpp
# End Source File
# Begin Source File

SOURCE=.\Emitter\semit2d.h
# End Source File
# Begin Source File

SOURCE=.\Emitter\semit3d.cpp
# End Source File
# Begin Source File

SOURCE=.\Emitter\semit3d.h
# End Source File
# Begin Source File

SOURCE=.\Emitter\semitenv.cpp
# End Source File
# Begin Source File

SOURCE=.\Emitter\semitenv.h
# End Source File
# Begin Source File

SOURCE=.\Emitter\semitter.cpp
# End Source File
# Begin Source File

SOURCE=.\Emitter\semitter.h
# End Source File
# Begin Source File

SOURCE=.\Emitter\shandle.cpp
# End Source File
# Begin Source File

SOURCE=.\Emitter\shandle.h
# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Math\doppler.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\doppler.h
# End Source File
# Begin Source File

SOURCE=.\Math\positional.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\positional.h
# End Source File
# Begin Source File

SOURCE=.\Math\semitone.h
# End Source File
# Begin Source File

SOURCE=.\Math\volume.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\volume.h
# End Source File
# End Group
# Begin Group "ps2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2\ass.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\ass.h
# End Source File
# Begin Source File

SOURCE=.\ps2\assirx.cpp
# End Source File
# Begin Source File

SOURCE=.\ps2\assirx.h
# End Source File
# Begin Source File

SOURCE=.\PS2\sbankps2.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\sbankps2.h
# End Source File
# Begin Source File

SOURCE=.\PS2\sdriver.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\sdriver.h
# End Source File
# Begin Source File

SOURCE=.\PS2\sdriver.hpp
# End Source File
# Begin Source File

SOURCE=.\PS2\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\sound.h
# End Source File
# Begin Source File

SOURCE=.\PS2\sounderror.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\sounderror.h
# End Source File
# Begin Source File

SOURCE=.\PS2\soundstream.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\soundstream.h
# End Source File
# Begin Source File

SOURCE=.\PS2\vag.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\audiolib.h
# End Source File
# Begin Source File

SOURCE=.\audiosys.cpp
# End Source File
# Begin Source File

SOURCE=.\audiosys.h
# End Source File
# Begin Source File

SOURCE=.\jukebox.cpp
# End Source File
# Begin Source File

SOURCE=.\jukebox.h
# End Source File
# Begin Source File

SOURCE=.\listener.cpp
# End Source File
# Begin Source File

SOURCE=.\listener.h
# End Source File
# Begin Source File

SOURCE=.\soundattr.cpp
# End Source File
# Begin Source File

SOURCE=.\soundattr.h
# End Source File
# Begin Source File

SOURCE=.\soundbank.cpp
# End Source File
# Begin Source File

SOURCE=.\soundbank.h
# End Source File
# Begin Source File

SOURCE=.\sounddriver.cpp
# End Source File
# Begin Source File

SOURCE=.\sounddriver.h
# End Source File
# Begin Source File

SOURCE=.\streamq.cpp
# End Source File
# Begin Source File

SOURCE=.\streamq.h
# End Source File
# Begin Source File

SOURCE=.\voiceq.cpp
# End Source File
# Begin Source File

SOURCE=.\voiceq.h
# End Source File
# End Target
# End Project
