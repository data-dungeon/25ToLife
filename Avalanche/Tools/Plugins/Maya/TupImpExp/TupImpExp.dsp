# Microsoft Developer Studio Project File - Name="TupImpExp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TupImpExp - Win32 Maya Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TupImpExp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TupImpExp.mak" CFG="TupImpExp - Win32 Maya Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TupImpExp - Win32 Maya Debug" (based on "Win32 (x86) Application")
!MESSAGE "TupImpExp - Win32 Maya Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Maya/TupImpExp", XWFCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TupImpExp - Win32 Maya Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TupImpExp___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "TupImpExp___Win32_Maya_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "." /I "$(MAYAPATH)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /Yu"TupImpExpPch.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "." /I "$(MAYAPATH)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /D "WIN32" /D "WIN32_MAYA_DEBUG" /Yu"TupImpExpPch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Foundation.lib OpenMaya.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Max_Hybrid\TupImpExp.mll" /pdbtype:sept /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Maya_Debug\TupImpExp.mll" /pdbtype:sept /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Custom Build - Copying $(InputName) to Maya Directory
InputPath=.\Maya_Debug\TupImpExp.mll
InputName=TupImpExp
SOURCE="$(InputPath)"

"$(MAYAPATH)\bin\plug-ins\$(InputName).mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) "$(MAYAPATH)\bin\plug-ins\$(InputName).mll"

# End Custom Build

!ELSEIF  "$(CFG)" == "TupImpExp - Win32 Maya Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TupImpExp___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "TupImpExp___Win32_Maya_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(MAYAPATH)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /Yu"TupImpExpPch.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(MAYAPATH)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /D "WIN32" /D "WIN32_MAYA_RELEASE" /Yu"TupImpExpPch.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Foundation.lib OpenMaya.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Max_Release\TupImpExp.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Maya_Release\TupImpExp.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Custom Build - Copying $(InputName) to Maya Directory
InputPath=.\Maya_Release\TupImpExp.mll
InputName=TupImpExp
SOURCE="$(InputPath)"

"$(MAYAPATH)\bin\plug-ins\$(InputName).mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) "$(MAYAPATH)\bin\plug-ins\$(InputName).mll"

# End Custom Build

!ENDIF 

# Begin Target

# Name "TupImpExp - Win32 Maya Debug"
# Name "TupImpExp - Win32 Maya Release"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pluginMain.cpp
# End Source File
# Begin Source File

SOURCE=.\TupImpExpPch.cpp
# ADD BASE CPP /Yc"TupImpExpPch.h"
# ADD CPP /Yc"TupImpExpPch.h"
# End Source File
# Begin Source File

SOURCE=.\TupImpExpPch.h
# End Source File
# End Group
# Begin Group "Import/Export"

# PROP Default_Filter ""
# Begin Group "Export"

# PROP Default_Filter ""
# Begin Group "Scene Export"

# PROP Default_Filter ""
# Begin Group "Mesh Manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MeshManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshManager.h
# End Source File
# End Group
# Begin Group "Skin Manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SkinManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinManager.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ExportImage.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportLayers.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportMap.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportModifier.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportScene.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\UnknownObject.cpp
# End Source File
# Begin Source File

SOURCE=.\UnknownObject.h
# End Source File
# End Group
# Begin Group "Animation Export"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportAnimCurves.cpp
# End Source File
# End Group
# Begin Group "Connections Export"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportConnections.cpp
# End Source File
# End Group
# Begin Group "Audio Export"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportAudio.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\TupExport.cpp
# End Source File
# Begin Source File

SOURCE=.\TupExport.h
# End Source File
# End Group
# Begin Group "Import"

# PROP Default_Filter ""
# Begin Group "Scene Import"

# PROP Default_Filter ""
# Begin Group "Shader Manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ShaderManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ShaderManager.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ImportLayers.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportMap.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportScene.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportTransform.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\TupImport.cpp
# End Source File
# Begin Source File

SOURCE=.\TupImport.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TupImpExp.cpp
# End Source File
# Begin Source File

SOURCE=.\TupImpExp.h
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CompareTupObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareTupObjects.h
# End Source File
# Begin Source File

SOURCE=.\CRC.cpp
# End Source File
# Begin Source File

SOURCE=.\CRC.h
# End Source File
# Begin Source File

SOURCE=.\filepath.cpp
# End Source File
# Begin Source File

SOURCE=.\filepath.h
# End Source File
# Begin Source File

SOURCE=.\intmap.h
# End Source File
# Begin Source File

SOURCE=.\intmap.hpp
# End Source File
# Begin Source File

SOURCE=.\MayaHelper.h
# End Source File
# Begin Source File

SOURCE=.\MayaHelper.hpp
# End Source File
# End Group
# Begin Group "Mel Scripts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TupTranslatorOptions.mel
# End Source File
# End Group
# End Target
# End Project
