# Microsoft Developer Studio Project File - Name="KFMTranslator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=KFMTranslator - Win32 Maya Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KFMTranslator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KFMTranslator.mak" CFG="KFMTranslator - Win32 Maya Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KFMTranslator - Win32 Maya Release" (based on "Win32 (x86) Application")
!MESSAGE "KFMTranslator - Win32 Maya Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Maya/KFMTranslator", LTYCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KFMTranslator - Win32 Maya Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KFMTranslator___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "KFMTranslator___Win32_Maya_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(MAYAPATH)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(MAYAPATH)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaRender.lib /nologo /subsystem:windows /dll /machine:I386 /out:"$(MAYAPATH)\bin\plug-ins\KFMTranslator.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaRender.lib OpenMayaUI.lib user32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"$(MAYAPATH)\bin\plug-ins\KFMTranslator.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin

!ELSEIF  "$(CFG)" == "KFMTranslator - Win32 Maya Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "KFMTranslator___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "KFMTranslator___Win32_Maya_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "$(MAYAPATH)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "$(MAYAPATH)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaRender.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"$(MAYAPATH)\bin\plug-ins\KFMTranslator.mll" /pdbtype:sept /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaRender.lib OpenMayaUI.lib user32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"$(MAYAPATH)\bin\plug-ins\KFMTranslator.mll" /pdbtype:sept /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin

!ENDIF 

# Begin Target

# Name "KFMTranslator - Win32 Maya Release"
# Name "KFMTranslator - Win32 Maya Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\KFM.cpp
# End Source File
# Begin Source File

SOURCE=.\KFMTranslator.cpp
# End Source File
# Begin Source File

SOURCE=.\pluginMain.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\HashTime.h
# End Source File
# Begin Source File

SOURCE=.\KFM.H
# End Source File
# Begin Source File

SOURCE=.\KFMTranslator.h
# End Source File
# Begin Source File

SOURCE=.\Matrix33.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# Begin Source File

SOURCE=.\vector2.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
