# Microsoft Developer Studio Project File - Name="math_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=math_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "math_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "math_sn.mak" CFG="math_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "math_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "math_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "math_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "math_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/math", WDDAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "math_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "math_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "math_sn___Win32_PS2_EE_Debug"
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

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "math_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "math_sn___Win32_PS2_EE_Release"
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

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "math_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "math_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "A_COMPILER_SN" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "math_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "math_sn___Win32_PS2_EE_PA"
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

# Name "math_sn - Win32 PS2 EE Debug"
# Name "math_sn - Win32 PS2 EE Release"
# Name "math_sn - Win32 PS2 EE CDROM"
# Name "math_sn - Win32 PS2 EE PA"
# Begin Group "ps2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PS2\Mat4x4.hpp
# End Source File
# Begin Source File

SOURCE=.\PS2\Mat4x4PS2.cpp

!IF  "$(CFG)" == "math_sn - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE Release"

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE CDROM"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE PA"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Math.hpp
# End Source File
# Begin Source File

SOURCE=.\PS2\MathInit.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2\MathMicro.vcl
# End Source File
# Begin Source File

SOURCE=.\PS2\MathMicro.vsm

!IF  "$(CFG)" == "math_sn - Win32 PS2 EE Debug"

# Begin Custom Build
InputDir=.\PS2
OutDir=.\PS2_EE_Debug
InputPath=.\PS2\MathMicro.vsm
InputName=MathMicro

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE Release"

# Begin Custom Build
InputDir=.\PS2
OutDir=.\PS2_EE_Release
InputPath=.\PS2\MathMicro.vsm
InputName=MathMicro

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE CDROM"

# Begin Custom Build
InputDir=.\PS2
OutDir=.\PS2_EE_CDROM
InputPath=.\PS2\MathMicro.vsm
InputName=MathMicro

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ELSEIF  "$(CFG)" == "math_sn - Win32 PS2 EE PA"

# Begin Custom Build
InputDir=.\PS2
OutDir=.\PS2_EE_PA
InputPath=.\PS2\MathMicro.vsm
InputName=MathMicro

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ee-dvp-as -I$(InputDir) -o $(OutDir)\$(InputName).obj $(InputDir)\$(InputName).vsm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PS2\Vect3.hpp
# End Source File
# Begin Source File

SOURCE=.\PS2\Vect4.hpp
# End Source File
# End Group
# Begin Group "generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generic\BuildView.cpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Det.hpp
# End Source File
# Begin Source File

SOURCE=.\Generic\DistFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Mat3x3.cpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Mat3x3.hpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Mat4x4.cpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Mat4x4.hpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Math.cpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Math.hpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Matrix.hpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Quatern.cpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Vect2.hpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Vect3.hpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Vect4.hpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Vector.cpp
# End Source File
# Begin Source File

SOURCE=.\Generic\Vector.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\BuildView.h
# End Source File
# Begin Source File

SOURCE=.\Det.h
# End Source File
# Begin Source File

SOURCE=.\DistFunc.h
# End Source File
# Begin Source File

SOURCE=.\Macros.h
# End Source File
# Begin Source File

SOURCE=.\Mat3x3.h
# End Source File
# Begin Source File

SOURCE=.\Mat4x4.h
# End Source File
# Begin Source File

SOURCE=.\MathClass.h
# End Source File
# Begin Source File

SOURCE=.\MathStats.h
# End Source File
# Begin Source File

SOURCE=.\MathStruct.h
# End Source File
# Begin Source File

SOURCE=.\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Quatern.h
# End Source File
# Begin Source File

SOURCE=.\Vect2.h
# End Source File
# Begin Source File

SOURCE=.\Vect3.h
# End Source File
# Begin Source File

SOURCE=.\Vect4.h
# End Source File
# Begin Source File

SOURCE=.\Vector.h
# End Source File
# End Target
# End Project
