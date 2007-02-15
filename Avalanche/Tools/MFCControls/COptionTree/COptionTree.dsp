# Microsoft Developer Studio Project File - Name="COptionTree" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=COptionTree - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "COptionTree.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "COptionTree.mak" CFG="COptionTree - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "COptionTree - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "COptionTree - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "COptionTree - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "COptionTree - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "COptionTree - Win32 Release"
# Name "COptionTree - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\OptionTree.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeCheckButton.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeColorPopUp.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSel.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSelColorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSelFontCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSelGlobals.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSelSizeCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeImagePopUp.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeIPAddressEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItem.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemCheckBox.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemColor.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemDate.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemFile.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemFont.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemHyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemImage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemIPAddress.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemRadio.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemSpinner.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeList.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeSpinnerButton.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionTreeSpinnerEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\OptionTree.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeCheckButton.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeColorPopUp.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeDef.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSel.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSelColorButton.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSelFontCombo.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSelGlobals.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeFontSelSizeCombo.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeImagePopUp.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeInfo.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeIPAddressEdit.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItem.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemCheckBox.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemColor.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemComboBox.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemDate.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemEdit.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemFile.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemFont.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemHyperLink.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemImage.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemIPAddress.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemRadio.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemSpinner.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeItemStatic.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeList.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeSpinnerButton.h
# End Source File
# Begin Source File

SOURCE=.\OptionTreeSpinnerEdit.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
