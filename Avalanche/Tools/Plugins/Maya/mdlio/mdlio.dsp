# Microsoft Developer Studio Project File - Name="mdlio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=mdlio - Win32 Maya Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mdlio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mdlio.mak" CFG="mdlio - Win32 Maya Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mdlio - Win32 Maya Release" (based on "Win32 (x86) Application")
!MESSAGE "mdlio - Win32 Maya Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Maya/mdlio", YENAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mdlio - Win32 Maya Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mdlio___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "mdlio___Win32_Maya_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(MAYAPATH)\include" /I "..\..\libraries\mdllib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "$(MAYAPATH)\include" /I "..\..\libraries\mdllib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /out:"$(MAYAPATH)\bin\plug-ins\mdlio.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 OpenMayaRender.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaUI.lib user32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"$(MAYAPATH)\bin\plug-ins\mdlio.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "mdlio - Win32 Maya Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mdlio___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "mdlio___Win32_Maya_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(MAYAPATH)\include" /I "..\..\libraries\mdllib" /I "..\.." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "NT_PLUGIN" /FD /GZ /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(MAYAPATH)\include" /I "..\..\libraries\mdllib" /I "..\mdllib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "NT_PLUGIN" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"MSVCRTD" /out:"$(MAYAPATH)\bin\plug-ins\mdlio.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 OpenMayaRender.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib user32.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"MSVCRTD" /out:"$(MAYAPATH)\bin\plug-ins\mdlio.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "mdlio - Win32 Maya Release"
# Name "mdlio - Win32 Maya Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\mayatools.cpp
# End Source File
# Begin Source File

SOURCE=.\mdlio.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\mayatools.h
# End Source File
# Begin Source File

SOURCE=.\mdlio.h
# End Source File
# Begin Source File

SOURCE=.\mdlmacros.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
