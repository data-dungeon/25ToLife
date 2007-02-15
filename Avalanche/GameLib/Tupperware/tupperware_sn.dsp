# Microsoft Developer Studio Project File - Name="tupperware_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tupperware_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tupperware_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tupperware_sn.mak" CFG="tupperware_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tupperware_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperware_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/tupperware", FNKAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tupperware_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "tupperware_sn___Win32_PS2_EE_Debug"
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

!ELSEIF  "$(CFG)" == "tupperware_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "tupperware_sn___Win32_PS2_EE_Release"
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

!ELSEIF  "$(CFG)" == "tupperware_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "tupperware_sn___Win32_PS2_EE_CDROM"
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

!ELSEIF  "$(CFG)" == "tupperware_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperware_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "tupperware_sn___Win32_PS2_EE_PA"
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

# Name "tupperware_sn - Win32 PS2 EE Debug"
# Name "tupperware_sn - Win32 PS2 EE Release"
# Name "tupperware_sn - Win32 PS2 EE CDROM"
# Name "tupperware_sn - Win32 PS2 EE PA"
# Begin Source File

SOURCE=.\Aggregate.cpp
# End Source File
# Begin Source File

SOURCE=.\Aggregate.h
# End Source File
# Begin Source File

SOURCE=.\Atom.cpp
# End Source File
# Begin Source File

SOURCE=.\Atom.h
# End Source File
# Begin Source File

SOURCE=.\binaryloadsave.cpp
# End Source File
# Begin Source File

SOURCE=.\binaryloadsave.h
# End Source File
# Begin Source File

SOURCE=.\Codec.cpp
# End Source File
# Begin Source File

SOURCE=.\Codec.h
# End Source File
# Begin Source File

SOURCE=.\elements.h
# End Source File
# Begin Source File

SOURCE=.\hash.h
# End Source File
# Begin Source File

SOURCE=.\Iterator.cpp
# End Source File
# Begin Source File

SOURCE=.\Iterator.h
# End Source File
# Begin Source File

SOURCE=.\List.cpp
# End Source File
# Begin Source File

SOURCE=.\List.h
# End Source File
# Begin Source File

SOURCE=.\loadsave.cpp
# End Source File
# Begin Source File

SOURCE=.\loadsave.h
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\memorymanager.cpp
# End Source File
# Begin Source File

SOURCE=.\memorymanager.h
# End Source File
# Begin Source File

SOURCE=.\path.cpp
# End Source File
# Begin Source File

SOURCE=.\path.h
# End Source File
# Begin Source File

SOURCE=.\pool.cpp
# End Source File
# Begin Source File

SOURCE=.\pool.h
# End Source File
# Begin Source File

SOURCE=.\PrintTup.cpp
# End Source File
# Begin Source File

SOURCE=.\Reader.cpp
# End Source File
# Begin Source File

SOURCE=.\Reader.h
# End Source File
# Begin Source File

SOURCE=.\Scalar.cpp
# End Source File
# Begin Source File

SOURCE=.\Scalar.h
# End Source File
# Begin Source File

SOURCE=.\stringmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\stringmanager.h
# End Source File
# Begin Source File

SOURCE=.\tuparray.h
# End Source File
# Begin Source File

SOURCE=.\TupperwarePch.cpp
# End Source File
# Begin Source File

SOURCE=.\TupperwarePch.h
# End Source File
# Begin Source File

SOURCE=.\tupstring.cpp
# End Source File
# Begin Source File

SOURCE=.\tupstring.h
# End Source File
# Begin Source File

SOURCE=.\Writer.cpp
# End Source File
# Begin Source File

SOURCE=.\Writer.h
# End Source File
# End Target
# End Project
