# Microsoft Developer Studio Project File - Name="display_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=display_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "display_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "display_xbox.mak" CFG="display_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "display_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "display_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/display", MDDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "display_xbox - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "XBX_Release"
# PROP Intermediate_Dir "XBX_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "WIN32" /D "_XBOX" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "display_xbox - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "XBX_Debug"
# PROP Intermediate_Dir "XBX_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "WIN32" /D "_XBOX" /D "DIRECTX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "display_xbox - Xbox Release"
# Name "display_xbox - Xbox Debug"
# Begin Group "DirectX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DirectX\buffcomm.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\DirectX.h
# End Source File
# Begin Source File

SOURCE=.\DirectX\DrawArray.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\DrawList.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\MatrixStack.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\Platform.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\RenderState.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\StripBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\TextureBind.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\TextureState.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\VtxBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectX\VtxBuffer.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Common\BuffComm.h
# End Source File
# Begin Source File

SOURCE=.\Common\CRenderState.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CTextureState.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\DisplayPlatform.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\DisplayPlatform.h
# End Source File
# Begin Source File

SOURCE=.\Common\DisplayState.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\DisplayState.h
# End Source File
# Begin Source File

SOURCE=.\Common\Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\Draw.h
# End Source File
# Begin Source File

SOURCE=.\Common\DrawArray.h
# End Source File
# Begin Source File

SOURCE=.\Common\GfxFunc.h
# End Source File
# Begin Source File

SOURCE=.\Common\MatrixStack.h
# End Source File
# Begin Source File

SOURCE=.\Common\PatchDisp.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\PatchDisp.h
# End Source File
# Begin Source File

SOURCE=.\Common\RenderMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\RenderMisc.h
# End Source File
# Begin Source File

SOURCE=.\Common\RenderState.h
# End Source File
# Begin Source File

SOURCE=.\Common\SceneView.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\SceneView.h
# End Source File
# Begin Source File

SOURCE=.\Common\SplitPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\SplitPatch.h
# End Source File
# Begin Source File

SOURCE=.\Common\StripBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Common\TextureAnimator.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\TextureAnimator.h
# End Source File
# Begin Source File

SOURCE=.\Common\TextureMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\TextureMgr.h
# End Source File
# Begin Source File

SOURCE=.\Common\TextureState.h
# End Source File
# Begin Source File

SOURCE=.\Common\VtxBuffer.h
# End Source File
# End Target
# End Project
