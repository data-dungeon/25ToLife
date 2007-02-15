# Microsoft Developer Studio Project File - Name="Tupper3d" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Tupper3d - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Tupper3d.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tupper3d.mak" CFG="Tupper3d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Tupper3d - Win32 OpenGL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Tupper3d - Win32 OpenGL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Tupper3d - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Tupper3d - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Tupper3d - Win32 Max Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE "Tupper3d - Win32 Maya Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Tupper3d - Win32 Maya Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Tupper3d - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Tupper3d - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Tupper3d", RJRAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Tupper3d - Win32 OpenGL Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"Tupper3dPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Tupper3d - Win32 OpenGL Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"Tupper3dPch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Tupper3d - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Tupper3d___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "Tupper3d___Win32_Max_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "WIN32_MAX_DEBUG" /Yu"Tupper3dPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Tupper3d - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Tupper3d___Win32_Max_Release"
# PROP BASE Intermediate_Dir "Tupper3d___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "WIN32_MAX_RELEASE" /Yu"Tupper3dPch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Tupper3d - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Tupper3d___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "Tupper3d___Win32_Max_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_LIB" /Yu"Tupper3dPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Tupper3d - Win32 Maya Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Tupper3d___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "Tupper3d___Win32_Maya_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_LIB" /Yu"Tupper3dPch.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_LIB" /D "WIN32_MAYA_DEBUG" /D "_DEBUG" /D "WIN32" /Yu"Tupper3dPch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Tupper3d - Win32 Maya Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Tupper3d___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "Tupper3d___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_LIB" /Yu"Tupper3dPch.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_LIB" /D "WIN32_MAYA_RELEASE" /D "WIN32" /Yu"Tupper3dPch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Tupper3d - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Tupper3d___Win32_Release"
# PROP BASE Intermediate_Dir "Tupper3d___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Tupper3d___Win32_Release"
# PROP Intermediate_Dir "Tupper3d___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_LIB" /D "WIN32_MAYA_RELEASE" /D "WIN32" /Yu"Tupper3dPch.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(AVTOOLS)" /D "NDEBUG" /D "_LIB" /D "WIN32_MAYA_RELEASE" /D "WIN32" /Yu"Tupper3dPch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Tupper3d - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Tupper3d___Win32_Debug0"
# PROP BASE Intermediate_Dir "Tupper3d___Win32_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Tupper3d___Win32_Debug0"
# PROP Intermediate_Dir "Tupper3d___Win32_Debug0"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_LIB" /D "WIN32_MAYA_DEBUG" /D "_DEBUG" /D "WIN32" /Yu"Tupper3dPch.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(AVTOOLS)" /D "_LIB" /D "WIN32_MAYA_DEBUG" /D "_DEBUG" /D "WIN32" /Yu"Tupper3dPch.h" /FD /GZ /c
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

# Name "Tupper3d - Win32 OpenGL Debug"
# Name "Tupper3d - Win32 OpenGL Release"
# Name "Tupper3d - Win32 Max Debug"
# Name "Tupper3d - Win32 Max Release"
# Name "Tupper3d - Win32 Max Hybrid"
# Name "Tupper3d - Win32 Maya Debug"
# Name "Tupper3d - Win32 Maya Release"
# Name "Tupper3d - Win32 Release"
# Name "Tupper3d - Win32 Debug"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tupper3dPch.cpp
# ADD CPP /Yc"Tupper3dPch.h"
# End Source File
# Begin Source File

SOURCE=.\Tupper3dPch.h
# End Source File
# End Group
# Begin Group "Scene"

# PROP Default_Filter ""
# Begin Group "Node"

# PROP Default_Filter ""
# Begin Group "Transform"

# PROP Default_Filter ""
# Begin Group "StaticMatrix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\staticmatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\staticmatrix.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\transform.cpp
# End Source File
# Begin Source File

SOURCE=.\transform.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\node.cpp
# End Source File
# Begin Source File

SOURCE=.\node.h
# End Source File
# Begin Source File

SOURCE=.\nodepool.cpp
# End Source File
# Begin Source File

SOURCE=.\nodepool.h
# End Source File
# End Group
# Begin Group "Object"

# PROP Default_Filter ""
# Begin Group "TriangleMesh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\triangleStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\triangleStrip.h
# End Source File
# Begin Source File

SOURCE=.\trimesh.cpp
# End Source File
# Begin Source File

SOURCE=.\trimesh.h
# End Source File
# Begin Source File

SOURCE=.\trimeshmap.cpp
# End Source File
# Begin Source File

SOURCE=.\trimeshmap.h
# End Source File
# End Group
# Begin Group "PolyMesh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\polymesh.cpp
# End Source File
# Begin Source File

SOURCE=.\polymesh.h
# End Source File
# Begin Source File

SOURCE=.\polymeshmap.cpp
# End Source File
# Begin Source File

SOURCE=.\polymeshmap.h
# End Source File
# End Group
# Begin Group "Light"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\light.cpp
# End Source File
# Begin Source File

SOURCE=.\light.h
# End Source File
# End Group
# Begin Group "Camera"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# End Group
# Begin Group "XRef"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xref.cpp
# End Source File
# Begin Source File

SOURCE=.\xref.h
# End Source File
# End Group
# Begin Group "Derived"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\derived.cpp
# End Source File
# Begin Source File

SOURCE=.\derived.h
# End Source File
# Begin Source File

SOURCE=.\derivedmodifier.cpp
# End Source File
# Begin Source File

SOURCE=.\derivedmodifier.h
# End Source File
# End Group
# Begin Group "Shape"

# PROP Default_Filter ""
# Begin Group "Bezier Shape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bezierShape.cpp
# End Source File
# Begin Source File

SOURCE=.\bezierShape.h
# End Source File
# Begin Source File

SOURCE=.\spline.cpp
# End Source File
# Begin Source File

SOURCE=.\spline.h
# End Source File
# End Group
# End Group
# Begin Group "Bone"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bone.cpp
# End Source File
# Begin Source File

SOURCE=.\Bone.h
# End Source File
# End Group
# Begin Group "Locator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\locator.cpp
# End Source File
# Begin Source File

SOURCE=.\locator.h
# End Source File
# End Group
# Begin Group "LookAt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lookat.cpp
# End Source File
# Begin Source File

SOURCE=.\lookat.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\object.cpp
# End Source File
# Begin Source File

SOURCE=.\object.h
# End Source File
# Begin Source File

SOURCE=.\objectpool.cpp
# End Source File
# Begin Source File

SOURCE=.\objectpool.h
# End Source File
# End Group
# Begin Group "Material"

# PROP Default_Filter ""
# Begin Group "Standard Material"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\standardmaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\standardmaterial.h
# End Source File
# Begin Source File

SOURCE=.\standardmaterialmap.cpp
# End Source File
# Begin Source File

SOURCE=.\standardmaterialmap.h
# End Source File
# End Group
# Begin Group "MultiSubObject Material"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\multisubobjectmaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\multisubobjectmaterial.h
# End Source File
# Begin Source File

SOURCE=.\multisubobjectmaterialsubmaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\multisubobjectmaterialsubmaterial.h
# End Source File
# End Group
# Begin Group "Blend Material"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\blendmaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\blendmaterial.h
# End Source File
# End Group
# Begin Group "Composite Material"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\compositematerial.cpp
# End Source File
# Begin Source File

SOURCE=.\compositematerial.h
# End Source File
# Begin Source File

SOURCE=.\compositematerialsubmaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\compositematerialsubmaterial.h
# End Source File
# End Group
# Begin Group "Double Sided Material"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\doublesidedmaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\doublesidedmaterial.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\material.cpp
# End Source File
# Begin Source File

SOURCE=.\material.h
# End Source File
# Begin Source File

SOURCE=.\materialpool.cpp
# End Source File
# Begin Source File

SOURCE=.\materialpool.h
# End Source File
# End Group
# Begin Group "Map"

# PROP Default_Filter ""
# Begin Group "Bitmap"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitmapmap.cpp
# End Source File
# Begin Source File

SOURCE=.\bitmapmap.h
# End Source File
# End Group
# Begin Group "Composite Map"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\compositemap.cpp
# End Source File
# Begin Source File

SOURCE=.\compositemap.h
# End Source File
# Begin Source File

SOURCE=.\compositemapsubmap.cpp
# End Source File
# Begin Source File

SOURCE=.\compositemapsubmap.h
# End Source File
# End Group
# Begin Group "Mix Map"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mixmap.cpp
# End Source File
# Begin Source File

SOURCE=.\mixmap.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\map.cpp
# End Source File
# Begin Source File

SOURCE=.\map.h
# End Source File
# Begin Source File

SOURCE=.\mappool.cpp
# End Source File
# Begin Source File

SOURCE=.\mappool.h
# End Source File
# End Group
# Begin Group "Image"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\image.cpp
# End Source File
# Begin Source File

SOURCE=.\image.h
# End Source File
# Begin Source File

SOURCE=.\imagepool.cpp
# End Source File
# Begin Source File

SOURCE=.\imagepool.h
# End Source File
# End Group
# Begin Group "Modifier"

# PROP Default_Filter ""
# Begin Group "Apply Vertex Colors Modifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\applyvcmodifier.cpp
# End Source File
# Begin Source File

SOURCE=.\applyvcmodifier.h
# End Source File
# End Group
# Begin Group "Vertex Paint Modifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vertexpaintmodifier.cpp
# End Source File
# Begin Source File

SOURCE=.\vertexpaintmodifier.h
# End Source File
# Begin Source File

SOURCE=.\vertexpaintmodifiercontext.cpp
# End Source File
# Begin Source File

SOURCE=.\vertexpaintmodifiercontext.h
# End Source File
# End Group
# Begin Group "Hold Vertex Colors Modifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\holdvcmodifier.cpp
# End Source File
# Begin Source File

SOURCE=.\holdvcmodifier.h
# End Source File
# End Group
# Begin Group "Face Attributes Modifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\faceattributesmodifier.cpp
# End Source File
# Begin Source File

SOURCE=.\faceattributesmodifier.h
# End Source File
# Begin Source File

SOURCE=.\faceattributesmodifiercontext.cpp
# End Source File
# Begin Source File

SOURCE=.\faceattributesmodifiercontext.h
# End Source File
# End Group
# Begin Group "Skin Modifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skinmodifier.cpp
# End Source File
# Begin Source File

SOURCE=.\skinmodifier.h
# End Source File
# Begin Source File

SOURCE=.\skinmodifierbone.cpp
# End Source File
# Begin Source File

SOURCE=.\skinmodifierbone.h
# End Source File
# Begin Source File

SOURCE=.\skinmodifiercontext.cpp
# End Source File
# Begin Source File

SOURCE=.\skinmodifiercontext.h
# End Source File
# Begin Source File

SOURCE=.\skinmodifiercontextvertex.cpp
# End Source File
# Begin Source File

SOURCE=.\skinmodifiercontextvertex.h
# End Source File
# Begin Source File

SOURCE=.\skinmodifiercontextvertexpool.cpp
# End Source File
# Begin Source File

SOURCE=.\skinmodifiercontextvertexpool.h
# End Source File
# End Group
# Begin Group "Cloth Modifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ClothModifier.cpp
# End Source File
# Begin Source File

SOURCE=.\ClothModifier.h
# End Source File
# Begin Source File

SOURCE=.\ClothModifierContext.cpp
# End Source File
# Begin Source File

SOURCE=.\ClothModifierContext.h
# End Source File
# End Group
# Begin Group "Unknown Modifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\unknownmodifier.cpp
# End Source File
# Begin Source File

SOURCE=.\unknownmodifier.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\modifier.cpp
# End Source File
# Begin Source File

SOURCE=.\modifier.h
# End Source File
# Begin Source File

SOURCE=.\modifierpool.cpp
# End Source File
# Begin Source File

SOURCE=.\modifierpool.h
# End Source File
# End Group
# Begin Group "Layer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\layer.cpp
# End Source File
# Begin Source File

SOURCE=.\layer.h
# End Source File
# Begin Source File

SOURCE=.\layerpool.cpp
# End Source File
# Begin Source File

SOURCE=.\layerpool.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\scene.cpp
# End Source File
# Begin Source File

SOURCE=.\scene.h
# End Source File
# End Group
# Begin Group "Animation"

# PROP Default_Filter ""
# Begin Group "Globals"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimGlobals.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimGlobals.h
# End Source File
# End Group
# Begin Group "Set"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimSet.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimSet.h
# End Source File
# Begin Source File

SOURCE=.\AnimSetPool.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimSetPool.h
# End Source File
# End Group
# Begin Group "Channel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimChannel.h
# End Source File
# Begin Source File

SOURCE=.\AnimChannelPool.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimChannelPool.h
# End Source File
# End Group
# Begin Group "Clip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimClipPool.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimClipPool.h
# End Source File
# Begin Source File

SOURCE=.\AnimInstancedClip.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimInstancedClip.h
# End Source File
# Begin Source File

SOURCE=.\AnimSourceClip.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimSourceClip.h
# End Source File
# End Group
# Begin Group "Clip Blend"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimClipBlend.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimClipBlend.h
# End Source File
# Begin Source File

SOURCE=.\AnimClipBlendPool.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimClipBlendPool.h
# End Source File
# End Group
# Begin Group "Curve"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimCurve.h
# End Source File
# Begin Source File

SOURCE=.\AnimCurvePool.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimCurvePool.h
# End Source File
# End Group
# End Group
# Begin Group "Connection"

# PROP Default_Filter ""
# Begin Group "Motion Path"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\connectionmotionpath.cpp
# End Source File
# Begin Source File

SOURCE=.\connectionmotionpath.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\connection.cpp
# End Source File
# Begin Source File

SOURCE=.\connection.h
# End Source File
# Begin Source File

SOURCE=.\connectionpool.cpp
# End Source File
# Begin Source File

SOURCE=.\connectionpool.h
# End Source File
# End Group
# Begin Group "Audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\audio.cpp
# End Source File
# Begin Source File

SOURCE=.\audio.h
# End Source File
# Begin Source File

SOURCE=.\audiopool.cpp
# End Source File
# Begin Source File

SOURCE=.\audiopool.h
# End Source File
# End Group
# Begin Group "ImageData"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ImageData.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageData.h
# End Source File
# Begin Source File

SOURCE=.\ImageDataFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageDataFrame.h
# End Source File
# Begin Source File

SOURCE=.\ImageDataMap.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageDataMap.h
# End Source File
# Begin Source File

SOURCE=.\ImageDataPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageDataPalette.h
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Group "Conversion"

# PROP Default_Filter ""
# Begin Group "Object Converters"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ObjectConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectConverter.h
# End Source File
# Begin Source File

SOURCE=.\PolyMeshToTriMeshConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyMeshToTriMeshConverter.h
# End Source File
# Begin Source File

SOURCE=.\TriMeshToPolyMeshConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\TriMeshToPolyMeshConverter.h
# End Source File
# End Group
# Begin Group "Coordinate System Converters"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CoordinateSystemConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\CoordinateSystemConverter.h
# End Source File
# Begin Source File

SOURCE=.\ZupToYupConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\ZupToYupConverter.h
# End Source File
# End Group
# Begin Group "Material Converters"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MultiSubObjectMaterialConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiSubObjectMaterialConverter.h
# End Source File
# End Group
# End Group
# Begin Group "Tri Mesh Connectivity"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TriMeshConnectivity.cpp
# End Source File
# Begin Source File

SOURCE=.\TriMeshConnectivity.h
# End Source File
# Begin Source File

SOURCE=.\TriMeshConnectivityParts.h
# End Source File
# Begin Source File

SOURCE=.\TriMeshConnectivityParts.hpp
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TupMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\TupMatrix.h
# End Source File
# Begin Source File

SOURCE=.\TupMatrix.hpp
# End Source File
# Begin Source File

SOURCE=.\TupVertex.cpp
# End Source File
# Begin Source File

SOURCE=.\TupVertex.h
# End Source File
# Begin Source File

SOURCE=.\TupVertex.hpp
# End Source File
# End Group
# Begin Group "Helpers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ObjectHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectHelper.h
# End Source File
# End Group
# Begin Group "Keeper"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\keeper.h
# End Source File
# Begin Source File

SOURCE=.\keeper.hpp
# End Source File
# End Group
# End Group
# End Target
# End Project
