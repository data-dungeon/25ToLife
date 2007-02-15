# Microsoft Developer Studio Project File - Name="tupperattrib_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tupperattrib_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TupperAttrib_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TupperAttrib_sn.mak" CFG="tupperattrib_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tupperattrib_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperattrib_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperattrib_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "tupperattrib_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/GameLib/tupperattrib", FNKAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tupperattrib_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperattrib_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "tupperattrib_sn___Win32_PS2_EE_Debug"
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

!ELSEIF  "$(CFG)" == "tupperattrib_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperattrib_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "tupperattrib_sn___Win32_PS2_EE_Release"
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

!ELSEIF  "$(CFG)" == "tupperattrib_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperattrib_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "tupperattrib_sn___Win32_PS2_EE_CDROM"
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

!ELSEIF  "$(CFG)" == "tupperattrib_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tupperattrib_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "tupperattrib_sn___Win32_PS2_EE_PA"
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

# Name "tupperattrib_sn - Win32 PS2 EE Debug"
# Name "tupperattrib_sn - Win32 PS2 EE Release"
# Name "tupperattrib_sn - Win32 PS2 EE CDROM"
# Name "tupperattrib_sn - Win32 PS2 EE PA"
# Begin Source File

SOURCE=.\AccessInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\AccessInstance.h
# End Source File
# Begin Source File

SOURCE=.\AccessPath.cpp
# End Source File
# Begin Source File

SOURCE=.\AccessPath.h
# End Source File
# Begin Source File

SOURCE=.\AccessRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\AccessRoot.h
# End Source File
# Begin Source File

SOURCE=.\ConfigBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigBase.h
# End Source File
# Begin Source File

SOURCE=.\ConfigCheckBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigCheckBox.h
# End Source File
# Begin Source File

SOURCE=.\ConfigComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigComboBox.h
# End Source File
# Begin Source File

SOURCE=.\ConfigEditBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigEditBox.h
# End Source File
# Begin Source File

SOURCE=.\ConfigFreeColorBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigFreeColorBox.h
# End Source File
# Begin Source File

SOURCE=.\ConfigNumeric.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigNumeric.h
# End Source File
# Begin Source File

SOURCE=.\ConfigOverride.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigOverride.h
# End Source File
# Begin Source File

SOURCE=.\ConfigRadioGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigRadioGroup.h
# End Source File
# Begin Source File

SOURCE=.\ConfigRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigRoot.h
# End Source File
# Begin Source File

SOURCE=.\ConfigSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigSelection.h
# End Source File
# Begin Source File

SOURCE=.\ConfigSetColorBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigSetColorBox.h
# End Source File
# Begin Source File

SOURCE=.\KeySelection.cpp
# End Source File
# Begin Source File

SOURCE=.\KeySelection.h
# End Source File
# Begin Source File

SOURCE=.\KeyValue.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValue.h
# End Source File
# Begin Source File

SOURCE=.\KeyValueContext.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValueContext.h
# End Source File
# Begin Source File

SOURCE=.\PS2_in_VC.h
# End Source File
# Begin Source File

SOURCE=.\TupperAttribPch.cpp
# End Source File
# Begin Source File

SOURCE=.\TupperattribPch.h
# End Source File
# End Target
# End Project
