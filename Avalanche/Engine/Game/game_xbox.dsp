# Microsoft Developer Studio Project File - Name="game_xbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=game_xbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "game_xbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "game_xbox.mak" CFG="game_xbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "game_xbox - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "game_xbox - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/game", LDDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "game_xbox - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "$(AVGAME)" /I "$(AVPATH)" /D "NDEBUG" /D "DIRECTX" /D "WIN32" /D "_XBOX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "game_xbox - Xbox Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(AVGAME)" /I "$(AVPATH)" /D "_DEBUG" /D "DIRECTX" /D "WIN32" /D "_XBOX" /D MMX_OK=$(MMX_OK) /YX /FD /G6 /Zvc6 /c
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "game_xbox - Xbox Release"
# Name "game_xbox - Xbox Debug"
# Begin Group "database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Database\anm_load.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\ChunkLoaders.h
# End Source File
# Begin Source File

SOURCE=.\Database\DataHelpers.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\DataHelpers.h
# End Source File
# Begin Source File

SOURCE=.\Database\DataMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\DataMgr.h
# End Source File
# Begin Source File

SOURCE=.\Database\DataTypes.h
# End Source File
# Begin Source File

SOURCE=.\Database\DBFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\DBFile.h
# End Source File
# Begin Source File

SOURCE=.\Database\dma_load.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\DynamicObject.h
# End Source File
# Begin Source File

SOURCE=.\Database\EnviroInstance.h
# End Source File
# Begin Source File

SOURCE=.\Database\Instance.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\Instance.h
# End Source File
# Begin Source File

SOURCE=.\Database\LOD.h
# End Source File
# Begin Source File

SOURCE=.\Database\mot_load.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\SceneNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\SceneNode.h
# End Source File
# Begin Source File

SOURCE=.\Database\som_load.cpp
# End Source File
# Begin Source File

SOURCE=.\Database\World.h
# End Source File
# Begin Source File

SOURCE=.\Database\world_load.cpp
# End Source File
# End Group
# Begin Group "managers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Managers\AuraMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\AuraMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\DistortionMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\DistortionMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\LightMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\LightMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\MiscPools.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\MiscPools.h
# End Source File
# Begin Source File

SOURCE=.\Managers\ParticleDB.h
# End Source File
# Begin Source File

SOURCE=.\Managers\ParticleMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\ParticleMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\ReflectionMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\ReflectionMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\RenderMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\RenderMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\SceneMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\SceneMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\ShadowMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\ShadowMgr.h
# End Source File
# Begin Source File

SOURCE=.\Managers\SpriteMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Managers\SpriteMgr.h
# End Source File
# End Group
# Begin Group "animation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Animation\Animproc.cpp
# End Source File
# Begin Source File

SOURCE=.\Animation\Animproc.h
# End Source File
# Begin Source File

SOURCE=.\Animation\ApplyKFM.cpp
# End Source File
# Begin Source File

SOURCE=.\Animation\ApplyKFM.h
# End Source File
# Begin Source File

SOURCE=.\Animation\bone.cpp
# End Source File
# Begin Source File

SOURCE=.\Animation\Bone.h
# End Source File
# Begin Source File

SOURCE=.\Animation\bonegroup.cpp
# End Source File
# Begin Source File

SOURCE=.\Animation\BoneGroup.h
# End Source File
# Begin Source File

SOURCE=.\Animation\MotCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Animation\motion.cpp
# End Source File
# Begin Source File

SOURCE=.\Animation\Motion.h
# End Source File
# Begin Source File

SOURCE=.\Animation\motionchannel.cpp
# End Source File
# Begin Source File

SOURCE=.\Animation\MotionChannel.h
# End Source File
# Begin Source File

SOURCE=.\Animation\MotionDB.h
# End Source File
# Begin Source File

SOURCE=.\Animation\ScriptDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\Animation\ScriptDebug.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Misc\Actor.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc\Actor.h
# End Source File
# Begin Source File

SOURCE=.\Misc\Bounds.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc\Bounds.h
# End Source File
# Begin Source File

SOURCE=.\Misc\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Misc\FastVars.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc\FastVars.h
# End Source File
# Begin Source File

SOURCE=.\Misc\fontinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc\FontInfo.h
# End Source File
# Begin Source File

SOURCE=.\Misc\Frustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc\Frustum.h
# End Source File
# Begin Source File

SOURCE=.\Misc\GameEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc\GameEngine.h
# End Source File
# End Target
# End Project
