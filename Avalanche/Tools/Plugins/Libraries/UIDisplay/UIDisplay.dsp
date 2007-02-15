# Microsoft Developer Studio Project File - Name="UIDisplay" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=UIDisplay - Win32 Maya Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UIDisplay.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UIDisplay.mak" CFG="UIDisplay - Win32 Maya Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UIDisplay - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "UIDisplay - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "UIDisplay - Win32 Max Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE "UIDisplay - Win32 Maya Release" (based on "Win32 (x86) Static Library")
!MESSAGE "UIDisplay - Win32 Maya Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Max/UIDisplay", ZGVAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UIDisplay - Win32 Max Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"UIDisplayPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "UIDisplay - Win32 Max Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"UIDisplayPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "UIDisplay - Win32 Max Hybrid"

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
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"UIDisplayPCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "UIDisplay - Win32 Maya Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UIDisplay___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "UIDisplay___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"UIDisplayPCH.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"UIDisplayPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "UIDisplay - Win32 Maya Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UIDisplay___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "UIDisplay___Win32_Maya_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"UIDisplayPCH.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"UIDisplayPCH.h" /FD /GZ /c
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

# Name "UIDisplay - Win32 Max Debug"
# Name "UIDisplay - Win32 Max Release"
# Name "UIDisplay - Win32 Max Hybrid"
# Name "UIDisplay - Win32 Maya Release"
# Name "UIDisplay - Win32 Maya Debug"
# Begin Group "Config Files"

# PROP Default_Filter ""
# Begin Group "Override Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConfigOverride.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigOverride.h
# End Source File
# End Group
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

SOURCE=.\ConfigCheckList.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigCheckList.h
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
# End Group
# Begin Group "CurrAttribData Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CurrAttribBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CurrAttribBase.h
# End Source File
# End Group
# Begin Group "ExclusiveController Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExclusiveController.cpp
# End Source File
# Begin Source File

SOURCE=.\ExclusiveController.h
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# End Group
# Begin Group "UIDisplay Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FuncPtrController.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncPtrController.h
# End Source File
# Begin Source File

SOURCE=.\UIController.cpp
# End Source File
# Begin Source File

SOURCE=.\UIController.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplayBase.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplayBase.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplayCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplayCheck.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplayCheckList.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplayCheckList.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplayCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplayCombo.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplayEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplayEdit.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplayFreeColor.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplayFreeColor.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplayNumeric.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplayNumeric.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplayRadioGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplayRadioGroup.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplaySetColor.cpp
# End Source File
# Begin Source File

SOURCE=.\UIDisplaySetColor.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CheckListState.bmp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\UIDisplay.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\UIDisplayPCH.cpp
# ADD CPP /Yc"UIDisplayPCH.h"
# End Source File
# Begin Source File

SOURCE=.\UIDisplayPCH.h
# End Source File
# End Target
# End Project
