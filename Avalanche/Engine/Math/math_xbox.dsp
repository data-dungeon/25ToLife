# Microsoft Developer Studio Project File - Name="math_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=math_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "math_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "math_xbox.mak" CFG="math_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "math_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "math_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/math", WDDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "math_xbox - Xbox Release"

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
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "math_xbox - Xbox Debug"

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
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "math_xbox - Xbox Release"
# Name "math_xbox - Xbox Debug"
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32\Mat4x4.hpp
# End Source File
# Begin Source File

SOURCE=.\win32\Mat4x4WIN32.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\Math.hpp
# End Source File
# Begin Source File

SOURCE=.\win32\Matrix.hpp
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
