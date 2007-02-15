# Microsoft Developer Studio Project File - Name="display_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=display_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "display_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "display_sn.mak" CFG="display_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "display_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "display_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "display_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "display_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/Display", MDDAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display_sn___Win32_PS2_EE_Debug0"
# PROP BASE Intermediate_Dir "display_sn___Win32_PS2_EE_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "AVALANCHE" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /debug -mvu0-use-vf0-vf15 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "display_sn___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "AVALANCHE" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "display_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "$(EAOPENGL)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "A_COMPILER_SN" /D "PS2" /D "OPENGL" /D "SNOW_REMOVING_EA_OPENGL" /D "AVALANCHE" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "AVALANCHE" /D "NDEBUG" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "display_sn___Win32_PS2_EE_PA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_PA"
# PROP Intermediate_Dir "PS2_EE_PA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "$(EAOPENGL)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "A_COMPILER_SN" /D "PS2" /D "AVALANCHE" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "AVALANCHE" /D "NDEBUG" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -mlong-calls -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
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

# Name "display_sn - Win32 PS2 EE Debug"
# Name "display_sn - Win32 PS2 EE Release"
# Name "display_sn - Win32 PS2 EE CDROM"
# Name "display_sn - Win32 PS2 EE PA"
# Begin Group "ps2renderer"

# PROP Default_Filter ""
# Begin Group "StripBufferVUCode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\VUStripBuffer1x2CD.vsm
InputName=VUStripBuffer1x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\VUStripBuffer2x2CD.vsm
InputName=VUStripBuffer2x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\VUStripBuffer3x2CD.vsm
InputName=VUStripBuffer3x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\VUStripBuffer4x2CD.vsm
InputName=VUStripBuffer4x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\VUStripBuffer5x2CD.vsm
InputName=VUStripBuffer5x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\VUStripBuffer6x2CD.vsm
InputName=VUStripBuffer6x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\VUStripBuffer7x2CD.vsm
InputName=VUStripBuffer7x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\VUStripBuffer8x2CD.vsm
InputName=VUStripBuffer8x2CD

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "SpriteParticleVUCode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\newparticle.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\newparticle.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\newparticle.vsm
InputName=newparticle

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VUCode\Sprites.vsm

!IF  "$(CFG)" == "display_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Debug
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_Release
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "display_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2Renderer\VUCode
OutDir=.\PS2_EE_PA
InputPath=.\PS2Renderer\VUCode\Sprites.vsm
InputName=Sprites

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	m4 -I$(InputDir) $(InputDir)\$(InputName).vsm > $(InputDir)\$(InputName).m4 
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).m4 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\PS2Renderer\buffcomm.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\DDMAObject.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GIFTag.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSMemManager.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSMemManager.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSState.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\GSState.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\MasterList.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\MatrixStack.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\Platform.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\PRIMReg.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\PS2.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\PS2Renderer.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\PSDRenderState.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\RenderState.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\StripBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\TextureBind.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\TextureState.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VIFInline.h
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VtxBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2Renderer\VtxBuffer.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Common\BuffComm.h
# End Source File
# Begin Source File

SOURCE=.\Common\CRenderState.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CTextureState.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\DisplayPlatform.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\DisplayPlatform.h
# End Source File
# Begin Source File

SOURCE=.\Common\DisplayState.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\DisplayState.h
# End Source File
# Begin Source File

SOURCE=.\Common\Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\Draw.h
# End Source File
# Begin Source File

SOURCE=.\Common\DrawArray.h
# End Source File
# Begin Source File

SOURCE=.\Common\GfxFunc.h
# End Source File
# Begin Source File

SOURCE=.\Common\MatrixStack.h
# End Source File
# Begin Source File

SOURCE=.\Common\PatchDisp.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\PatchDisp.h
# End Source File
# Begin Source File

SOURCE=.\Common\RenderMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\RenderMisc.h
# End Source File
# Begin Source File

SOURCE=.\Common\RenderState.h
# End Source File
# Begin Source File

SOURCE=.\Common\SceneView.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\SceneView.h
# End Source File
# Begin Source File

SOURCE=.\Common\SplitPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\SplitPatch.h
# End Source File
# Begin Source File

SOURCE=.\Common\StripBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Common\TextureAnimator.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\TextureAnimator.h
# End Source File
# Begin Source File

SOURCE=.\Common\TextureMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\TextureMgr.h
# End Source File
# Begin Source File

SOURCE=.\Common\TextureState.h
# End Source File
# Begin Source File

SOURCE=.\Common\VtxBuffer.h
# End Source File
# End Target
# End Project
