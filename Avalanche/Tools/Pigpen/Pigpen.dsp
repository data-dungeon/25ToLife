# Microsoft Developer Studio Project File - Name="Pigpen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Pigpen - Win32 OpenGL Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Pigpen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Pigpen.mak" CFG="Pigpen - Win32 OpenGL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Pigpen - Win32 OpenGL Release" (based on "Win32 (x86) Application")
!MESSAGE "Pigpen - Win32 OpenGL Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Juju/tools/Pigpen", UDCAAAAA"
# PROP Scc_LocalPath "."
CPP=cwcl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WGL_Release"
# PROP BASE Intermediate_Dir "WGL_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WGL_Release"
# PROP Intermediate_Dir "WGL_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "$(AVPATH)" /I "$(AVGAME)" /I "$(PLUGPATH)\Export" /I "$(PLUGPATH)\Common" /I "$(PLUGPATH)\ImportExport" /I "$(MGAPIDIST)\inc" /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "OPENGL" /D "PIGPEN" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib s3tc.lib /nologo /subsystem:windows /incremental:yes /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"msvcrtd.lib" /force /libpath:"$(AVPATH)\Display\WGL_Release" /libpath:"$(AVPATH)\Game\WGL_Release" /libpath:"$(AVPATH)\Layers\WGL_Release" /libpath:"$(AVPATH)\Processor\WGL_Release" /libpath:"$(AVTOOLS)\Tupperware\Debug"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WGL_Debug"
# PROP BASE Intermediate_Dir "WGL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WGL_Debug"
# PROP Intermediate_Dir "WGL_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(AVPATH)" /I "$(AVGAME)" /I "$(PLUGPATH)\Export" /I "$(PLUGPATH)\Common" /I "$(PLUGPATH)\ImportExport" /I "$(MGAPIDIST)\inc" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "OPENGL" /D "PIGPEN" /D MMX_OK=$(MMX_OK) /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib s3tc.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /force /pdbtype:sept /libpath:"$(AVPATH)\Display\WGL_Debug" /libpath:"$(AVPATH)\Game\WGL_Debug" /libpath:"$(AVPATH)\Layers\WGL_Debug" /libpath:"$(AVPATH)\Processor\WGL_Debug" /libpath:"$(AVTOOLS)\Tupperware\Debug"
# SUBTRACT LINK32 /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "Pigpen - Win32 OpenGL Release"
# Name "Pigpen - Win32 OpenGL Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Doc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Pigpen.ico
# End Source File
# Begin Source File

SOURCE=.\Pigpen.rc
# End Source File
# Begin Source File

SOURCE=.\res\Pigpen.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "MultiGen Plug-ins"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\BaseInfo.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\BaseInfoList.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\CommonUtility.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\CommonUtility.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\cropbmi.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\cropbmi.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\DBLSwap.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\DBLSwap.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\DBLUtil.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\DBLUtil.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\DBLWrite.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\DBLWrite.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\ExportVars.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\ExportVars.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\HashTable.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\HashTable.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\InfoFile.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\InfoFile.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\lookup2_c.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\N64Swizzle.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\N64Swizzle.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\PluginError.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\PluginError.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\Plugins.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\ReadImage.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Common\ReadImage.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\ImportExport\reggie.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\ImportExport\reggie.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\ImportExport\savestr.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\ImportExport\savestr.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\TextureChunk.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\TextureChunk.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\TextureInfo.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\TextureInfo.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\TextureList.cpp"

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\MultiGen Plug-ins\Export\TextureList.h"
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EffectDialog.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EffectDialog.h
# End Source File
# Begin Source File

SOURCE=.\EmitterDialog.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EmitterDialog.h
# End Source File
# Begin Source File

SOURCE=.\GraphicDialog.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GraphicDialog.h
# End Source File
# Begin Source File

SOURCE=.\ImportDialog.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImportDialog.h
# End Source File
# Begin Source File

SOURCE=.\PageCollide.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageCollide.h
# End Source File
# Begin Source File

SOURCE=.\PageColor.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageColor.h
# End Source File
# Begin Source File

SOURCE=.\PageDisplay.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageDisplay.h
# End Source File
# Begin Source File

SOURCE=.\PageEnvelope.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageEnvelope.h
# End Source File
# Begin Source File

SOURCE=.\PageLifetime.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageLifetime.h
# End Source File
# Begin Source File

SOURCE=.\PageMovement.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageMovement.h
# End Source File
# Begin Source File

SOURCE=.\PagePiggyback.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PagePiggyback.h
# End Source File
# Begin Source File

SOURCE=.\PageSize.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageSize.h
# End Source File
# Begin Source File

SOURCE=.\PageSwooshDisplay.cpp
# ADD CPP /Yu"PigpenPCH.h"
# End Source File
# Begin Source File

SOURCE=.\PageSwooshDisplay.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSheet.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ParticleSheet.h
# End Source File
# Begin Source File

SOURCE=.\PreviewDialog.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PreviewDialog.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDialog.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProgressDialog.h
# End Source File
# Begin Source File

SOURCE=.\SurfaceListDialog.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SurfaceListDialog.h
# End Source File
# Begin Source File

SOURCE=.\SwooshEmitterDialog.cpp
# ADD CPP /Yu"PigpenPCH.h"
# End Source File
# Begin Source File

SOURCE=.\SwooshEmitterDialog.h
# End Source File
# Begin Source File

SOURCE=.\SwooshSheet.cpp
# ADD CPP /Yu"PigpenPCH.h"
# End Source File
# Begin Source File

SOURCE=.\SwooshSheet.h
# End Source File
# End Group
# Begin Group "Data Classes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Effect.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Effect.h
# End Source File
# Begin Source File

SOURCE=.\Emitter.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Emitter.h
# End Source File
# Begin Source File

SOURCE=.\Envelope.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Envelope.h
# End Source File
# Begin Source File

SOURCE=.\Graphic.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Graphic.h
# End Source File
# Begin Source File

SOURCE=.\Image.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# Begin Source File

SOURCE=.\Particle.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Particle.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSwoosh.h
# End Source File
# Begin Source File

SOURCE=.\Spawn.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Spawn.h
# End Source File
# Begin Source File

SOURCE=.\Surface.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Surface.h
# End Source File
# Begin Source File

SOURCE=.\Swoosh.cpp
# ADD CPP /Yu"PigpenPCH.h"
# End Source File
# Begin Source File

SOURCE=.\Swoosh.h
# End Source File
# Begin Source File

SOURCE=.\SwooshEmitter.cpp
# ADD CPP /Yu"PigpenPCH.h"
# End Source File
# Begin Source File

SOURCE=.\SwooshEmitter.h
# End Source File
# End Group
# Begin Group "App"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\App.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\App.h
# End Source File
# Begin Source File

SOURCE=.\Doc.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Doc.h
# End Source File
# Begin Source File

SOURCE=.\Export.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Export.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\PigpenUtility.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PigpenUtility.h
# End Source File
# Begin Source File

SOURCE=.\Preview.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Preview.h
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\View.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yu"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yu"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\View.h
# End Source File
# End Group
# Begin Group "Avalanche"

# PROP Default_Filter ""
# Begin Group "Display"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\EngineNew\display\PS2Renderer\GSMemManager.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\EngineNew\Display\PS2Renderer\GSMemManager.h
# End Source File
# End Group
# Begin Group "GameHelper"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\GameLibNew\GameHelper\RandomNumber.cpp
# End Source File
# Begin Source File

SOURCE=..\..\GameLibNew\GameHelper\RandomNumber.h
# End Source File
# End Group
# End Group
# Begin Group "Stubs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PigpenStubs.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\PigpenPCH.cpp

!IF  "$(CFG)" == "Pigpen - Win32 OpenGL Release"

# ADD CPP /Yc"PigpenPCH.h"

!ELSEIF  "$(CFG)" == "Pigpen - Win32 OpenGL Debug"

# ADD CPP /Yc"PigpenPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PigpenPCH.h
# End Source File
# End Target
# End Project
