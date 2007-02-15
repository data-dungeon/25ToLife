# Microsoft Developer Studio Project File - Name="AMViewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AMViewer - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AMViewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AMViewer.mak" CFG="AMViewer - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AMViewer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AMViewer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/AMViewer", UUVAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AMViewer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MD /W3 /GR /GX /O2 /I "$(HASHSDKPATH)\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 OpenGL32.lib WinMM.lib $(HASHSDKPATH)\lib\pc\fileio.lib $(HASHSDKPATH)\lib\pc\bitmap.lib $(HASHSDKPATH)\lib\pc\Matrix.lib $(HASHSDKPATH)\lib\pc\patch.lib /nologo /subsystem:windows /machine:I386 /out:"$(HASHPATH)\AMViewer.exe"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "AMViewer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(HASHSDKPATH)\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 OpenGL32.lib WinMM.lib $(HASHSDKPATH)\lib\pc\fileio.lib $(HASHSDKPATH)\lib\pc\bitmap.lib $(HASHSDKPATH)\lib\pc\Matrix.lib $(HASHSDKPATH)\lib\pc\patch.lib /nologo /subsystem:windows /debug /machine:I386 /out:"$(HASHPATH)\AMViewer.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "AMViewer - Win32 Release"
# Name "AMViewer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Action.cpp
# End Source File
# Begin Source File

SOURCE=.\AMViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\AMViewer.rc
# End Source File
# Begin Source File

SOURCE=.\AMViewerDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\AMViewerMac.r
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\AMViewerSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\AMViewerView.cpp
# End Source File
# Begin Source File

SOURCE=.\Archive.cpp
# End Source File
# Begin Source File

SOURCE=.\AV2.cpp
# End Source File
# Begin Source File

SOURCE=.\AV3.cpp
# End Source File
# Begin Source File

SOURCE=.\AV4.cpp
# End Source File
# Begin Source File

SOURCE=.\Bone.cpp
# End Source File
# Begin Source File

SOURCE=.\Channel.cpp
# End Source File
# Begin Source File

SOURCE=.\KFM.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\map.cpp
# End Source File
# Begin Source File

SOURCE=.\Model.cpp
# End Source File
# Begin Source File

SOURCE=.\Mot.cpp
# End Source File
# Begin Source File

SOURCE=.\Move.cpp
# End Source File
# Begin Source File

SOURCE=.\NoAccelEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Ply.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Time.cpp
# End Source File
# Begin Source File

SOURCE=.\Transform.cpp
# End Source File
# Begin Source File

SOURCE=.\Turn.cpp
# End Source File
# Begin Source File

SOURCE=.\Zoom.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Action.h
# End Source File
# Begin Source File

SOURCE=..\AvalancheExporter\AGPShader.h
# End Source File
# Begin Source File

SOURCE=".\AM-GL.h"
# End Source File
# Begin Source File

SOURCE=.\AMViewer.h
# End Source File
# Begin Source File

SOURCE=.\AMViewerDoc.h
# End Source File
# Begin Source File

SOURCE=.\AMViewerSplitter.h
# End Source File
# Begin Source File

SOURCE=.\AMViewerView.h
# End Source File
# Begin Source File

SOURCE=.\Archive.h
# End Source File
# Begin Source File

SOURCE=.\AV2.h
# End Source File
# Begin Source File

SOURCE=.\AV3.h
# End Source File
# Begin Source File

SOURCE=.\AV4.h
# End Source File
# Begin Source File

SOURCE=.\Channel.h
# End Source File
# Begin Source File

SOURCE=.\KFM.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Model.h
# End Source File
# Begin Source File

SOURCE=.\Mot.h
# End Source File
# Begin Source File

SOURCE=.\NoAccelEdit.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=.\Ply.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ScriptView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AMViewer.ico
# End Source File
# Begin Source File

SOURCE=.\res\AMViewer.rc2
# End Source File
# Begin Source File

SOURCE=.\res\AMViewerDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\move.cur
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\turn.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoom.cur
# End Source File
# End Group
# End Target
# End Project
