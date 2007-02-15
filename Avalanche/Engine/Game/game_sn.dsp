# Microsoft Developer Studio Project File - Name="game_sn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=game_sn - Win32 PS2 EE PA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "game_sn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "game_sn.mak" CFG="game_sn - Win32 PS2 EE PA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "game_sn - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "game_sn - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE "game_sn - Win32 PS2 EE CDROM" (based on "Win32 (x86) Static Library")
!MESSAGE "game_sn - Win32 PS2 EE PA" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/Engine/game", LDDAAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "game_sn - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "game_sn___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "game_sn___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Debug"
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /Od /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "_DEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /debug -mvu0-use-vf0-vf15 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "game_sn - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "game_sn___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "game_sn___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_Release"
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -Winline -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "game_sn - Win32 PS2 EE CDROM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "game_sn___Win32_PS2_EE_CDROM"
# PROP BASE Intermediate_Dir "game_sn___Win32_PS2_EE_CDROM"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_CDROM"
# PROP Intermediate_Dir "PS2_EE_CDROM"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "$(EAOPENGL)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /D "OPENGL" /D "SNOW_REMOVING_EA_OPENGL" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "NDEBUG" /D "A_COMPILER_SN" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "game_sn - Win32 PS2 EE PA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "game_sn___Win32_PS2_EE_PA"
# PROP BASE Intermediate_Dir "game_sn___Win32_PS2_EE_PA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PS2_EE_PA"
# PROP Intermediate_Dir "PS2_EE_PA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C::\usr\local\sce\ee\include" /I "C::\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /I "$(EAOPENGL)" /D "NDEBUG" /D "SN_TARGET_PS2" /D "PS2" /FD -Wa,-sn /c
# ADD CPP /nologo /w /W0 /Zi /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /I "$(AVPATH)" /I "$(AVGAME)" /D "A_COMPILER_SN" /D "NDEBUG" /D "CDROM" /D "SN_TARGET_PS2" /D "PS2" /FD -mlong-calls -Wa,-sn -mvu0-use-vf0-vf15 -mno-check-zero-division -ffast-math -fno-rtti -fno-exceptions /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo /D:SN_TARGET_PS2
# ADD LIB32 /nologo /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "game_sn - Win32 PS2 EE Debug"
# Name "game_sn - Win32 PS2 EE Release"
# Name "game_sn - Win32 PS2 EE CDROM"
# Name "game_sn - Win32 PS2 EE PA"
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
# End Group
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
# Begin Group "misc"

# PROP Default_Filter ""
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
# End Group
# End Target
# End Project
