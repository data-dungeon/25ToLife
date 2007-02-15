# Microsoft Developer Studio Project File - Name="KeyVal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=KeyVal - Win32 Maya Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KeyVal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KeyVal.mak" CFG="KeyVal - Win32 Maya Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KeyVal - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "KeyVal - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "KeyVal - Win32 Max Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE "KeyVal - Win32 Maya Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "KeyVal - Win32 Maya Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Libraries/KeyVal", EJPBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KeyVal - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Max Debug"
# PROP BASE Intermediate_Dir "Max Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"KeyValPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "KeyVal - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Max Release"
# PROP BASE Intermediate_Dir "Max Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"KeyValPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "KeyVal - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Max Hybrid"
# PROP BASE Intermediate_Dir "Max Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"KeyValPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "KeyVal - Win32 Maya Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "KeyVal___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "KeyVal___Win32_Maya_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"KeyValPCH.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"KeyValPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "KeyVal - Win32 Maya Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KeyVal___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "KeyVal___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"KeyValPCH.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"KeyValPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "KeyVal - Win32 Max Debug"
# Name "KeyVal - Win32 Max Release"
# Name "KeyVal - Win32 Max Hybrid"
# Name "KeyVal - Win32 Maya Debug"
# Name "KeyVal - Win32 Maya Release"
# Begin Group "Main Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KeyValCommunicatorBase.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValCommunicatorBase.h
# End Source File
# Begin Source File

SOURCE=.\KeyValPanelController.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValPanelController.h
# End Source File
# Begin Source File

SOURCE=.\KeyValRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValRegistry.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\apply.ico
# End Source File
# Begin Source File

SOURCE=.\clear.ico
# End Source File
# Begin Source File

SOURCE=.\KeyVal.rc
# End Source File
# Begin Source File

SOURCE=.\purge.ico
# End Source File
# Begin Source File

SOURCE=.\remap.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\select.ico
# End Source File
# Begin Source File

SOURCE=.\TreeViewNormal.bmp
# End Source File
# Begin Source File

SOURCE=.\TreeViewState.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\KeyValPCH.cpp
# ADD CPP /Yc"KeyValPCH.h"
# End Source File
# Begin Source File

SOURCE=.\KeyValPCH.h
# End Source File
# End Target
# End Project
