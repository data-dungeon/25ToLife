# Microsoft Developer Studio Project File - Name="mapAttribWin32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mapAttribWin32 - Win32 Maya Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mapAttribWin32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mapAttribWin32.mak" CFG="mapAttribWin32 - Win32 Maya Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mapAttribWin32 - Win32 Maya Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mapAttribWin32 - Win32 Maya Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Maya/mapAttrib/mapAttribWin32", PJGCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mapAttribWin32 - Win32 Maya Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mapAttribWin32___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "mapAttribWin32___Win32_Maya_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(MAYA_INCL)" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NT_PLUGIN" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(MAYAPATH)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_WINDLL" /D "_USR_DLL" /D "NT_PLUGIN" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 openmayaui.lib foundation.lib openmaya.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(MAYA_LIB)"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 openmayaui.lib foundation.lib openmaya.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(MAYAPATH)\lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Copying $(InputName) to Maya Directory
InputPath=.\Maya_Debug\mapAttribWin32.dll
InputName=mapAttribWin32
SOURCE="$(InputPath)"

"$(MAYAPATH)\bin\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) "$(MAYAPATH)\bin\$(InputName).dll"

# End Custom Build

!ELSEIF  "$(CFG)" == "mapAttribWin32 - Win32 Maya Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mapAttribWin32___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "mapAttribWin32___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(MAYAPATH)\include" /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_WINDLL" /D "_USR_DLL" /D "NT_PLUGIN" /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 openmayaui.lib foundation.lib openmaya.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(MAYAPATH)\lib"
# Begin Custom Build - Copying $(InputName) to Maya Directory
InputPath=.\Maya_Release\mapAttribWin32.dll
InputName=mapAttribWin32
SOURCE="$(InputPath)"

"$(MAYAPATH)\bin\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) "$(MAYAPATH)\bin\$(InputName).dll"

# End Custom Build

!ENDIF 

# Begin Target

# Name "mapAttribWin32 - Win32 Maya Debug"
# Name "mapAttribWin32 - Win32 Maya Release"
# Begin Group "Main Files"

# PROP Default_Filter ""
# Begin Group "Custom Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SetupCustomControls.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupCustomControls.h
# End Source File
# End Group
# Begin Group "Attributes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CurrAttribController.cpp
# End Source File
# Begin Source File

SOURCE=.\CurrAttribController.h
# End Source File
# Begin Source File

SOURCE=.\CurrAttribData.cpp
# End Source File
# Begin Source File

SOURCE=.\CurrAttribData.h
# End Source File
# Begin Source File

SOURCE=.\MapAttrController.cpp
# End Source File
# Begin Source File

SOURCE=.\MapAttrController.h
# End Source File
# Begin Source File

SOURCE=.\MapAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\MapAttributes.h
# End Source File
# End Group
# Begin Group "nfo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\filepath.cpp
# End Source File
# Begin Source File

SOURCE=.\filepath.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\mapAttribContainerWnd.cpp

!IF  "$(CFG)" == "mapAttribWin32 - Win32 Maya Debug"

# ADD BASE CPP /Yu"StdAfx.h"
# ADD CPP /Yu"StdAfx.h"

!ELSEIF  "$(CFG)" == "mapAttribWin32 - Win32 Maya Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mapAttribContainerWnd.h
# End Source File
# Begin Source File

SOURCE=.\mapAttribWin32.cpp

!IF  "$(CFG)" == "mapAttribWin32 - Win32 Maya Debug"

# ADD BASE CPP /Yu"StdAfx.h"
# ADD CPP /Yu"StdAfx.h"

!ELSEIF  "$(CFG)" == "mapAttribWin32 - Win32 Maya Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mapAttribWin32.def
# End Source File
# Begin Source File

SOURCE=.\mapAttribWin32.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"StdAfx.h"
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "KeyVal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KeyValCommunicator.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValCommunicator.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\copy.ico
# End Source File
# Begin Source File

SOURCE=.\copydisabled.ico
# End Source File
# Begin Source File

SOURCE=.\mapAttribWin32.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "../../../libraries/UIDisplay" /i "../../../libraries/AttrControls" /i "../../../libraries/KeyVal"
# End Source File
# Begin Source File

SOURCE=.\res\mapAttribWin32.rc2
# End Source File
# Begin Source File

SOURCE=.\onetomany.ico
# End Source File
# Begin Source File

SOURCE=.\onetomanydisabled.ico
# End Source File
# Begin Source File

SOURCE=.\replace.ico
# End Source File
# Begin Source File

SOURCE=.\replacedisabled.ico
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# End Target
# End Project
