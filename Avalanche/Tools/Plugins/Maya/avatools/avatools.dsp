# Microsoft Developer Studio Project File - Name="avatools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AVATOOLS - WIN32 MAYA DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "avatools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avatools.mak" CFG="AVATOOLS - WIN32 MAYA DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avatools - Win32 Maya Release" (based on "Win32 (x86) Application")
!MESSAGE "avatools - Win32 Maya Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Maya/avatools", SOQAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "avatools - Win32 Maya Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "avatools___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "avatools___Win32_Maya_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(MAYAPATH)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "$(MAYAPATH)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Foundation.lib OpenMaya.lib User32.lib Gdi32.lib OpenMayaAnim.lib OpenMayaRender.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /machine:I386 /out:"$(MAYAPATH)\bin\plug-ins\avatools.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 Foundation.lib OpenMaya.lib User32.lib Gdi32.lib OpenMayaAnim.lib OpenMayaRender.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Maya_Release\avatools.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Custom Build - Copying $(InputName) to Maya Directory
InputPath=.\Maya_Release\avatools.mll
InputName=avatools
SOURCE="$(InputPath)"

"$(MAYAPATH)\bin\plug-ins\$(InputName).mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) "$(MAYAPATH)\bin\plug-ins\$(InputName).mll"

# End Custom Build

!ELSEIF  "$(CFG)" == "avatools - Win32 Maya Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "avatools___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "avatools___Win32_Maya_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(MAYAPATH)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "." /I "$(MAYAPATH)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Foundation.lib OpenMaya.lib User32.lib Gdi32.lib OpenMayaAnim.lib OpenMayaRender.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /machine:I386 /out:"$(MAYAPATH)\bin\plug-ins\avatools.mll" /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 Foundation.lib OpenMaya.lib User32.lib Gdi32.lib OpenMayaAnim.lib OpenMayaRender.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386 /out:"Maya_Debug/avatools.mll" /pdbtype:sept /libpath:"$(MAYAPATH)\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Custom Build - Copying $(InputName) to Maya Directory
InputPath=.\Maya_Debug\avatools.mll
InputName=avatools
SOURCE="$(InputPath)"

"$(MAYAPATH)\bin\plug-ins\$(InputName).mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) "$(MAYAPATH)\bin\plug-ins\$(InputName).mll"

# End Custom Build

!ENDIF 

# Begin Target

# Name "avatools - Win32 Maya Release"
# Name "avatools - Win32 Maya Debug"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CompareMeshes.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareMeshes.h
# End Source File
# Begin Source File

SOURCE=.\CRC.cpp
# End Source File
# Begin Source File

SOURCE=.\CRC.h
# End Source File
# Begin Source File

SOURCE=.\hdoublearray.cpp
# End Source File
# Begin Source File

SOURCE=.\hdoublearray.h
# End Source File
# Begin Source File

SOURCE=.\hintarray.cpp
# End Source File
# Begin Source File

SOURCE=.\hintarray.h
# End Source File
# Begin Source File

SOURCE=.\macros.h
# End Source File
# Begin Source File

SOURCE=..\..\Libraries\MTools\MTools.cpp
# End Source File
# Begin Source File

SOURCE=.\MTools.h
# End Source File
# Begin Source File

SOURCE=.\neighbortree.h
# End Source File
# Begin Source File

SOURCE=.\neighbortree.hpp
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# Begin Source File

SOURCE=.\vector.hpp
# End Source File
# Begin Source File

SOURCE=.\vector2.h
# End Source File
# Begin Source File

SOURCE=.\vector2.hpp
# End Source File
# Begin Source File

SOURCE=.\vectormap.h
# End Source File
# Begin Source File

SOURCE=.\vectormap.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\avatools.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Shelves"

# PROP Default_Filter ""
# Begin Group "AVA_ANIMATION"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bakeAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\bakeAnimation.h
# End Source File
# Begin Source File

SOURCE=.\BakeBoneScaleIntoTranslation.cpp
# End Source File
# Begin Source File

SOURCE=.\BakeBoneScaleIntoTranslation.h
# End Source File
# Begin Source File

SOURCE=.\CopyKeys.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyKeys.h
# End Source File
# Begin Source File

SOURCE=.\PasteKeys.cpp
# End Source File
# Begin Source File

SOURCE=.\PasteKeys.h
# End Source File
# End Group
# Begin Group "AVA_ENGINE"

# PROP Default_Filter ""
# End Group
# Begin Group "AVA_LIGHTING"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\attenuateLight.cpp
# End Source File
# Begin Source File

SOURCE=.\attenuateLight.h
# End Source File
# Begin Source File

SOURCE=.\vertexLight.cpp
# End Source File
# Begin Source File

SOURCE=.\vertexLight.h
# End Source File
# End Group
# Begin Group "AVA_MODELING"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\complimentSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\complimentSelect.h
# End Source File
# Begin Source File

SOURCE=.\extractpolygons.cpp
# End Source File
# Begin Source File

SOURCE=.\extractpolygons.h
# End Source File
# Begin Source File

SOURCE=.\invertSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\invertSelect.h
# End Source File
# Begin Source File

SOURCE=.\mergeEdge.cpp
# End Source File
# Begin Source File

SOURCE=.\mergeEdge.h
# End Source File
# Begin Source File

SOURCE=.\selectManipHandleView.cpp
# End Source File
# Begin Source File

SOURCE=.\selectManipHandleView.h
# End Source File
# Begin Source File

SOURCE=.\selectManipHandleX.cpp
# End Source File
# Begin Source File

SOURCE=.\selectManipHandleX.h
# End Source File
# Begin Source File

SOURCE=.\selectManipHandleY.cpp
# End Source File
# Begin Source File

SOURCE=.\selectManipHandleY.h
# End Source File
# Begin Source File

SOURCE=.\selectManipHandleZ.cpp
# End Source File
# Begin Source File

SOURCE=.\selectManipHandleZ.h
# End Source File
# Begin Source File

SOURCE=.\smoothNormals.cpp
# End Source File
# Begin Source File

SOURCE=.\smoothNormals.h
# End Source File
# End Group
# Begin Group "AVA_MODELING2"

# PROP Default_Filter ""
# End Group
# Begin Group "AVA_POPULATION"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\addObjectsToLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\addObjectsToLayer.h
# End Source File
# Begin Source File

SOURCE=.\copyAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\copyAttributes.h
# End Source File
# Begin Source File

SOURCE=.\copyVertexColors.cpp
# End Source File
# Begin Source File

SOURCE=.\copyVertexColors.h
# End Source File
# Begin Source File

SOURCE=.\createReference.cpp
# End Source File
# Begin Source File

SOURCE=.\createReference.h
# End Source File
# Begin Source File

SOURCE=.\duplicateObject.cpp
# End Source File
# Begin Source File

SOURCE=.\duplicateObject.h
# End Source File
# Begin Source File

SOURCE=.\instanceObject.cpp
# End Source File
# Begin Source File

SOURCE=.\instanceObject.h
# End Source File
# Begin Source File

SOURCE=.\makeObjectsNamesDifferent.cpp
# End Source File
# Begin Source File

SOURCE=.\makeObjectsNamesDifferent.h
# End Source File
# Begin Source File

SOURCE=.\makeObjectsNamesSimilar.cpp
# End Source File
# Begin Source File

SOURCE=.\makeObjectsNamesSimilar.h
# End Source File
# Begin Source File

SOURCE=.\removeReference.cpp
# End Source File
# Begin Source File

SOURCE=.\removeReference.h
# End Source File
# Begin Source File

SOURCE=.\replaceInstances.cpp
# End Source File
# Begin Source File

SOURCE=.\replaceInstances.h
# End Source File
# Begin Source File

SOURCE=.\selectAllByType.cpp
# End Source File
# Begin Source File

SOURCE=.\selectAllByType.h
# End Source File
# Begin Source File

SOURCE=.\selectInstances.cpp
# End Source File
# Begin Source File

SOURCE=.\selectInstances.h
# End Source File
# Begin Source File

SOURCE=.\updateInstances.cpp
# End Source File
# Begin Source File

SOURCE=.\updateInstances.h
# End Source File
# Begin Source File

SOURCE=.\updateReference.cpp
# End Source File
# Begin Source File

SOURCE=.\updateReference.h
# End Source File
# End Group
# Begin Group "AVA_TEXTURES"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AverageUVs.cpp
# End Source File
# Begin Source File

SOURCE=.\AverageUVs.h
# End Source File
# Begin Source File

SOURCE=.\consolidateShaders.cpp
# End Source File
# Begin Source File

SOURCE=.\consolidateShaders.h
# End Source File
# Begin Source File

SOURCE=.\flipUVs.cpp
# End Source File
# Begin Source File

SOURCE=.\flipUVs.h
# End Source File
# Begin Source File

SOURCE=.\highlightShader.cpp
# End Source File
# Begin Source File

SOURCE=.\highlightShader.h
# End Source File
# Begin Source File

SOURCE=.\multilisterToBoxes.cpp
# End Source File
# Begin Source File

SOURCE=.\multilisterToBoxes.h
# End Source File
# Begin Source File

SOURCE=.\rotateUVs.cpp
# End Source File
# Begin Source File

SOURCE=.\rotateUVs.h
# End Source File
# Begin Source File

SOURCE=.\scaleUVs.cpp
# End Source File
# Begin Source File

SOURCE=.\scaleUVs.h
# End Source File
# Begin Source File

SOURCE=.\tileUVs.cpp
# End Source File
# Begin Source File

SOURCE=.\tileUVs.h
# End Source File
# End Group
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\addKeyValueToNotes.cpp
# End Source File
# Begin Source File

SOURCE=.\addKeyValueToNotes.h
# End Source File
# Begin Source File

SOURCE=.\addObjectsToRefGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\addObjectsToRefGroup.h
# End Source File
# Begin Source File

SOURCE=.\applyCvColorShader.cpp
# End Source File
# Begin Source File

SOURCE=.\applyCvColorShader.h
# End Source File
# Begin Source File

SOURCE=.\bakeTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\bakeTransform.h
# End Source File
# Begin Source File

SOURCE=.\delKeyValueFromNotes.cpp
# End Source File
# Begin Source File

SOURCE=.\delKeyValueFromNotes.h
# End Source File
# Begin Source File

SOURCE=.\getKeyValueFromNotes.cpp
# End Source File
# Begin Source File

SOURCE=.\getKeyValueFromNotes.h
# End Source File
# Begin Source File

SOURCE=.\listKeysFromNotes.cpp
# End Source File
# Begin Source File

SOURCE=.\listKeysFromNotes.h
# End Source File
# Begin Source File

SOURCE=.\makeRefGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\makeRefGroup.h
# End Source File
# Begin Source File

SOURCE=.\makeRefLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\makeRefLayer.h
# End Source File
# Begin Source File

SOURCE=.\replaceObjectsWithLayerRef.cpp
# End Source File
# Begin Source File

SOURCE=.\replaceObjectsWithLayerRef.h
# End Source File
# Begin Source File

SOURCE=.\setBoneOrientToChild.cpp
# End Source File
# Begin Source File

SOURCE=.\setBoneOrientToChild.h
# End Source File
# Begin Source File

SOURCE=.\setCameraToView.cpp
# End Source File
# Begin Source File

SOURCE=.\setCameraToView.h
# End Source File
# Begin Source File

SOURCE=.\splitEdge.cpp
# End Source File
# Begin Source File

SOURCE=.\splitEdge.h
# End Source File
# Begin Source File

SOURCE=.\test.cpp
# End Source File
# Begin Source File

SOURCE=.\test.h
# End Source File
# Begin Source File

SOURCE=.\toggleVisibility.cpp
# End Source File
# Begin Source File

SOURCE=.\toggleVisibility.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\copyCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\copyCharacter.h
# End Source File
# Begin Source File

SOURCE=.\copyClips.cpp
# End Source File
# Begin Source File

SOURCE=.\copyClips.h
# End Source File
# Begin Source File

SOURCE=.\CopyTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyTransform.h
# End Source File
# Begin Source File

SOURCE=.\GoToBindPose.cpp
# End Source File
# Begin Source File

SOURCE=.\GoToBindPose.h
# End Source File
# End Target
# End Project
