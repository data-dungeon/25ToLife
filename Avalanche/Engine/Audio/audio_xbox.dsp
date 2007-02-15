# Microsoft Developer Studio Project File - Name="audio_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=audio_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "audio_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "audio_xbox.mak" CFG="audio_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "audio_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "audio_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/audio", IAWAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "audio_xbox - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "WIN32" /D "_XBOX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "audio_xbox - Xbox Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "WIN32" /D "_XBOX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "audio_xbox - Xbox Release"
# Name "audio_xbox - Xbox Debug"
# Begin Group "emitter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\emitter\e3dparam.cpp
# End Source File
# Begin Source File

SOURCE=.\emitter\e3dparam.h
# End Source File
# Begin Source File

SOURCE=.\emitter\envstrig.cpp
# End Source File
# Begin Source File

SOURCE=.\emitter\envstrig.h
# End Source File
# Begin Source File

SOURCE=.\emitter\semit2d.cpp
# End Source File
# Begin Source File

SOURCE=.\emitter\semit2d.h
# End Source File
# Begin Source File

SOURCE=.\emitter\semit3d.cpp
# End Source File
# Begin Source File

SOURCE=.\emitter\semit3d.h
# End Source File
# Begin Source File

SOURCE=.\emitter\semitenv.cpp
# End Source File
# Begin Source File

SOURCE=.\emitter\semitenv.h
# End Source File
# Begin Source File

SOURCE=.\emitter\semitter.cpp
# End Source File
# Begin Source File

SOURCE=.\emitter\semitter.h
# End Source File
# Begin Source File

SOURCE=.\emitter\shandle.cpp
# End Source File
# Begin Source File

SOURCE=.\emitter\shandle.h
# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\math\doppler.cpp
# End Source File
# Begin Source File

SOURCE=.\math\doppler.h
# End Source File
# Begin Source File

SOURCE=.\math\positional.cpp
# End Source File
# Begin Source File

SOURCE=.\math\positional.h
# End Source File
# Begin Source File

SOURCE=.\math\semitone.h
# End Source File
# Begin Source File

SOURCE=.\math\volume.cpp
# End Source File
# Begin Source File

SOURCE=.\math\volume.h
# End Source File
# End Group
# Begin Group "xbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xbox\sdriver.cpp
# End Source File
# Begin Source File

SOURCE=.\xbox\sdriver.h
# End Source File
# Begin Source File

SOURCE=.\xbox\sdrivercaps.cpp
# End Source File
# Begin Source File

SOURCE=.\xbox\sdrivercaps.h
# End Source File
# Begin Source File

SOURCE=.\xbox\sdriverenum.cpp
# End Source File
# Begin Source File

SOURCE=.\xbox\sdriverenum.h
# End Source File
# Begin Source File

SOURCE=.\xbox\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\xbox\sound.h
# End Source File
# Begin Source File

SOURCE=.\xbox\sounderror.cpp
# End Source File
# Begin Source File

SOURCE=.\xbox\sounderror.h
# End Source File
# Begin Source File

SOURCE=.\xbox\soundstream.h
# End Source File
# Begin Source File

SOURCE=.\xbox\wmastream.cpp
# End Source File
# Begin Source File

SOURCE=.\xbox\wmastream.h
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
