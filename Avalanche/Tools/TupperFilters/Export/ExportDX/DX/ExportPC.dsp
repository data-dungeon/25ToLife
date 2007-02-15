# Microsoft Developer Studio Project File - Name="ExportPC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ExportPC - Win32 Max Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ExportPC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ExportPC.mak" CFG="ExportPC - Win32 Max Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ExportPC - Win32 Max Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ExportPC - Win32 Max Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/Avalanche/tools/TupperFilters/ExportPC"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ExportPC - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Max_Release"
# PROP BASE Intermediate_Dir "Max_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EXPORT_PC" /Yu"ExportPch.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "$(AVTOOLS)" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "_USRDLL" /D "_WINDOWS" /D "EXPORT_PC" /D "WIN32" /D "WIN32_MAX_RELEASE" /Yu"ExportPch.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"$(TFC_FILTERS)/ExportPC.twf"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Max_Release/ExportPC.twf"
# Begin Custom Build - Copying $(InputName) to TWFilers Directory
InputPath=.\Max_Release\ExportPC.twf
InputName=ExportPC
SOURCE="$(InputPath)"

"$(TFC_FILTERS)\$(InputName).twf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(TFC_FILTERS)\$(InputName).twf

# End Custom Build

!ELSEIF  "$(CFG)" == "ExportPC - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Max_Debug"
# PROP BASE Intermediate_Dir "Max_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EXPORT_PC" /Yu"ExportPch.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /I "$(AVPATH)" /I "$(AVGAME)" /D "_USRDLL" /D "_WINDOWS" /D "EXPORT_PC" /D "_DEBUG" /D "WIN32" /D "WIN32_MAX_DEBUG" /FR /Yu"ExportPch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"$(TFC_FILTERS)/ExportPC.twf" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Max_Debug\ExportPC.twf" /pdbtype:sept
# Begin Custom Build - Copying $(InputName) to TWFilersDebug Directory
InputPath=.\Max_Debug\ExportPC.twf
InputName=ExportPC
SOURCE="$(InputPath)"

"$(TFC_FILTERS)Debug\$(InputName).twf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(TFC_FILTERS)Debug\$(InputName).twf

# End Custom Build

!ENDIF 

# Begin Target

# Name "ExportPC - Win32 Max Release"
# Name "ExportPC - Win32 Max Debug"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportPC.def
# End Source File
# Begin Source File

SOURCE=.\ExportPch.cpp
# ADD BASE CPP /Yc"ExportPch.h"
# ADD CPP /Yc"ExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ExportPch.h
# End Source File
# Begin Source File

SOURCE=.\PluginDesc.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginDesc.h
# End Source File
# End Group
# Begin Group "Filter"

# PROP Default_Filter ""
# Begin Group "DBL"

# PROP Default_Filter ""
# Begin Group "Surface Types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportSurfaceTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportSurfaceTypes.h
# End Source File
# End Group
# Begin Group "Paths"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportPath.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportPath.h
# End Source File
# End Group
# Begin Group "Texture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportTexture.h
# End Source File
# Begin Source File

SOURCE=.\ExportTextureContext.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportTextureContext.h
# End Source File
# End Group
# Begin Group "Mesh"

# PROP Default_Filter ""
# Begin Group "Bones"

# PROP Default_Filter ""
# Begin Group "BoneGroupTable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BoneGroupTable.cpp
# End Source File
# Begin Source File

SOURCE=.\BoneGroupTable.h
# End Source File
# Begin Source File

SOURCE=.\ExportBoneGroupTable.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportBoneGroupTable.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ExportBones.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportBones.h
# End Source File
# End Group
# Begin Group "Cloth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportClothSOM.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportClothSOM.h
# End Source File
# End Group
# Begin Group "FVF"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FVFVBBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\FVFVBBuilder.h
# End Source File
# Begin Source File

SOURCE=.\FVFVertex.h
# End Source File
# Begin Source File

SOURCE=.\FVFVertexBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\FVFVertexBuilder.h
# End Source File
# Begin Source File

SOURCE=.\FVFVertexStream.h
# End Source File
# Begin Source File

SOURCE=.\VertexDescriptor.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ExportSOM.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportSOM.h
# End Source File
# End Group
# Begin Group "Instance"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportInstance.h
# End Source File
# End Group
# Begin Group "Named Point"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportNamedPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportNamedPoint.h
# End Source File
# End Group
# Begin Group "World Emitter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportWorldEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportWorldEmitter.h
# End Source File
# End Group
# Begin Group "Scene Tree"

# PROP Default_Filter ""
# Begin Group "WalkSceneNode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WalkSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=.\WalkSceneNode.h
# End Source File
# End Group
# Begin Group "SceneNodeParts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SceneNodeParts.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ExportSceneTree.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportSceneTree.h
# End Source File
# End Group
# Begin Group "Visibility Region"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportVisibilityRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportVisibilityRegion.h
# End Source File
# End Group
# Begin Group "Instance Group"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportInstanceGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportInstanceGroup.h
# End Source File
# End Group
# Begin Group "Lighting"

# PROP Default_Filter "*.cpp,*.h"
# Begin Source File

SOURCE=.\ExportLighting.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportLighting.h
# End Source File
# End Group
# Begin Group "Instance Attributes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportInstanceAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportInstanceAttributes.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportSound.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportSound.h
# End Source File
# End Group
# Begin Group "Water"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExportWater.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportWater.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\eachnode.cpp
# End Source File
# Begin Source File

SOURCE=.\eachnode.h
# End Source File
# Begin Source File

SOURCE=.\ExportDBL.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportDBL.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ExportPC.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportPC.h
# End Source File
# End Group
# End Target
# End Project
