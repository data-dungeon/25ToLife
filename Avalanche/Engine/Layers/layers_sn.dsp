# Microsoft Developer Studio Project File - Name="layers_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=layers_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "layers_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "layers_sn.mak" CFG="layers_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "layers_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "layers_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "layers_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "layers_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/layers", ODDAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "layers_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "layers_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "layers_sn___Win32_PS2_EE_Debug"
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

!ELSEIF  "$(CFG)" == "layers_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "layers_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "layers_sn___Win32_PS2_EE_Release"
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

!ELSEIF  "$(CFG)" == "layers_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "layers_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "layers_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "OPENGL" /D "SNOW_REMOVING_EA_OPENGL" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "A_COMPILER_SN" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "layers_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "layers_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "layers_sn___Win32_PS2_EE_PA"
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

# Name "layers_sn - Win32 PS2 EE Debug"
# Name "layers_sn - Win32 PS2 EE Release"
# Name "layers_sn - Win32 PS2 EE CDROM"
# Name "layers_sn - Win32 PS2 EE PA"
# Begin Group "assert"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Assert\assert.cpp
# End Source File
# Begin Source File

SOURCE=.\Assert\Assert.h
# End Source File
# End Group
# Begin Group "ps2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Context\contps2.s

!IF  "$(CFG)" == "layers_sn - Win32 PS2 EE Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2_EE_Debug
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%PS2_ROOT%\ee\gcc\ee\bin\ps2eeas.exe -sn -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "layers_sn - Win32 PS2 EE Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2_EE_Release
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%PS2_ROOT%\ee\gcc\ee\bin\ps2eeas.exe -sn -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "layers_sn - Win32 PS2 EE CDROM"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2_EE_CDROM
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%PS2_ROOT%\ee\gcc\ee\bin\ps2eeas.exe -sn -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ELSEIF  "$(CFG)" == "layers_sn - Win32 PS2 EE PA"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.\Context
IntDir=.\PS2_EE_PA
InputPath=.\Context\contps2.s
InputName=contps2

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%PS2_ROOT%\ee\gcc\ee\bin\ps2eeas.exe -sn -o $(IntDir)\$(InputName).obj $(InputDir)\$(InputName).s

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\context.cpp
# End Source File
# Begin Source File

SOURCE=.\Context.h
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=.\DLinkList.cpp
# End Source File
# Begin Source File

SOURCE=.\DLinkList.h
# End Source File
# Begin Source File

SOURCE=.\DMASom.h
# End Source File
# Begin Source File

SOURCE=.\Event.h
# End Source File
# Begin Source File

SOURCE=.\hog.cpp
# End Source File
# Begin Source File

SOURCE=.\hog.h
# End Source File
# Begin Source File

SOURCE=.\hogtoc.h
# End Source File
# Begin Source File

SOURCE=.\joystick.cpp
# End Source File
# Begin Source File

SOURCE=.\Joystick.h
# End Source File
# Begin Source File

SOURCE=.\LayersState.cpp
# End Source File
# Begin Source File

SOURCE=.\LayersState.h
# End Source File
# Begin Source File

SOURCE=.\LayersStruct.h
# End Source File
# Begin Source File

SOURCE=.\media.cpp
# End Source File
# Begin Source File

SOURCE=.\Media.h
# End Source File
# Begin Source File

SOURCE=.\medialow.cpp
# End Source File
# Begin Source File

SOURCE=.\medialow.h
# End Source File
# Begin Source File

SOURCE=.\memory.cpp
# End Source File
# Begin Source File

SOURCE=.\Memory.h
# End Source File
# Begin Source File

SOURCE=.\SLinkList.cpp
# End Source File
# Begin Source File

SOURCE=.\SLinkList.h
# End Source File
# Begin Source File

SOURCE=.\Som.h
# End Source File
# Begin Source File

SOURCE=.\stack.cpp
# End Source File
# Begin Source File

SOURCE=.\Stack.h
# End Source File
# Begin Source File

SOURCE=.\tasking.cpp
# End Source File
# Begin Source File

SOURCE=.\Tasking.h
# End Source File
# Begin Source File

SOURCE=.\Timing.cpp
# End Source File
# Begin Source File

SOURCE=.\Timing.h
# End Source File
# End Target
# End Project
