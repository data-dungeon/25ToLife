# Microsoft Developer Studio Project File - Name="AttrControls" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AttrControls - Win32 Maya Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AttrControls.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AttrControls.mak" CFG="AttrControls - Win32 Maya Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AttrControls - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AttrControls - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AttrControls - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AttrControls - Win32 Max Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE "AttrControls - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AttrControls - Win32 Maya Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AttrControls - Win32 Maya Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Libraries/AttrControls", FJHCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AttrControls - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AttrControls - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AttrControls - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AttrControls___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "AttrControls___Win32_Max_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AttrControls - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AttrControls___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "AttrControls___Win32_Max_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AttrControls - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AttrControls___Win32_Max_Release"
# PROP BASE Intermediate_Dir "AttrControls___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "AttrControls___Win32_Max_Release"
# PROP Intermediate_Dir "AttrControls___Win32_Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /GX /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AttrControls - Win32 Maya Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AttrControls___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "AttrControls___Win32_Maya_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AttrControls - Win32 Maya Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AttrControls___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "AttrControls___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "AttrControls - Win32 Release"
# Name "AttrControls - Win32 Debug"
# Name "AttrControls - Win32 Max Debug"
# Name "AttrControls - Win32 Max Hybrid"
# Name "AttrControls - Win32 Max Release"
# Name "AttrControls - Win32 Maya Debug"
# Name "AttrControls - Win32 Maya Release"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DebugPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugPrint.h
# End Source File
# Begin Source File

SOURCE=.\WinUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\WinUtil.h
# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomControl.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomControl.h
# End Source File
# Begin Source File

SOURCE=.\CustomControls.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomControls.h
# End Source File
# Begin Source File

SOURCE=.\CustomEditBox.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomEditBox.h
# End Source File
# Begin Source File

SOURCE=.\CustomRollupPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomRollupPanel.h
# End Source File
# Begin Source File

SOURCE=.\CustomRollupWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomRollupWindow.h
# End Source File
# Begin Source File

SOURCE=.\CustomSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomSlider.h
# End Source File
# Begin Source File

SOURCE=.\CustomSpinner.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomSpinner.h
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ICustomControl.h
# End Source File
# Begin Source File

SOURCE=.\ICustomEditBox.h
# End Source File
# Begin Source File

SOURCE=.\ICustomRollupPanel.h
# End Source File
# Begin Source File

SOURCE=.\ICustomRollupWindow.h
# End Source File
# Begin Source File

SOURCE=.\ICustomSlider.h
# End Source File
# Begin Source File

SOURCE=.\ICustomSpinner.h
# End Source File
# End Group
# Begin Group "InterfaceImp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ICustomEditBoxImp.cpp
# End Source File
# Begin Source File

SOURCE=.\ICustomEditBoxImp.h
# End Source File
# Begin Source File

SOURCE=.\ICustomRollupWindowImp.cpp
# End Source File
# Begin Source File

SOURCE=.\ICustomRollupWindowImp.h
# End Source File
# Begin Source File

SOURCE=.\ICustomSliderImp.cpp
# End Source File
# Begin Source File

SOURCE=.\ICustomSliderImp.h
# End Source File
# Begin Source File

SOURCE=.\ICustomSpinnerImp.cpp
# End Source File
# Begin Source File

SOURCE=.\ICustomSpinnerImp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AttrControls.rc
# End Source File
# Begin Source File

SOURCE=.\panhand.cur
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\slider.cur
# End Source File
# Begin Source File

SOURCE=.\spinner.cur
# End Source File
# End Group
# End Target
# End Project
