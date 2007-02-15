# Microsoft Developer Studio Project File - Name="TupExport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TupExport - Win32 Max Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TupExport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TupExport.mak" CFG="TupExport - Win32 Max Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TupExport - Win32 Max Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TupExport - Win32 Max Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TupExport - Win32 Max Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Max/TupExport", VFTAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TupExport - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Max Debug"
# PROP BASE Intermediate_Dir "Max Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "E:\3dsmax4\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /FR /Yu"TupExportPch.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x90b0000" /subsystem:windows /dll /debug /machine:I386 /out:"E:\3dsmax4\plugins\TupExport.dle" /pdbtype:sept /libpath:"E:\3dsmax4\maxsdk\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x90b0000" /subsystem:windows /dll /debug /machine:I386 /out:"Max_Debug\TupExport.dle" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# Begin Custom Build - Copying $(InputName) to Max Debug Directory
InputPath=.\Max_Debug\TupExport.dle
InputName=TupExport
SOURCE="$(InputPath)"

"$(MAXDEBUGDIR)\exe\plugins\avalanche\$(InputName).dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(MAXDEBUGDIR)\exe\plugins\avalanche\$(InputName).dle

# End Custom Build

!ELSEIF  "$(CFG)" == "TupExport - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Max Release"
# PROP BASE Intermediate_Dir "Max Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /O2 /I "E:\3dsmax4\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"TupExportPch.h" /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x90b0000" /subsystem:windows /dll /machine:I386 /out:"E:\3dsmax4\plugins\TupExport.dle" /libpath:"E:\3dsmax4\maxsdk\lib" /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x90b0000" /subsystem:windows /dll /machine:I386 /out:"Max_Release\TupExport.dle" /libpath:"$(MAXSDK)\lib" /release
# Begin Custom Build - Copying $(InputName) to Max Directory
InputPath=.\Max_Release\TupExport.dle
InputName=TupExport
SOURCE="$(InputPath)"

"$(MAXDIR)\plugins\avalanche\$(InputName).dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(MAXDIR)\plugins\avalanche\$(InputName).dle

# End Custom Build

!ELSEIF  "$(CFG)" == "TupExport - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Max Hybrid"
# PROP BASE Intermediate_Dir "Max Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "E:\3dsmax4\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "..\..\..\\" /I "$(AVPATH)" /I "$(AVGAME)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"TupExportPch.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x90b0000" /subsystem:windows /dll /debug /machine:I386 /out:"E:\3dsmax4\plugins\TupExport.dle" /pdbtype:sept /libpath:"E:\3dsmax4\maxsdk\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x90b0000" /subsystem:windows /dll /debug /machine:I386 /out:"Max_Hybrid\TupExport.dle" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# Begin Custom Build - Copying $(InputName) to Max Directory
InputPath=.\Max_Hybrid\TupExport.dle
InputName=TupExport
SOURCE="$(InputPath)"

"$(MAXDIR)\plugins\avalanche\$(InputName).dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(MAXDIR)\plugins\avalanche\$(InputName).dle

# End Custom Build

!ENDIF 

# Begin Target

# Name "TupExport - Win32 Max Debug"
# Name "TupExport - Win32 Max Release"
# Name "TupExport - Win32 Max Hybrid"
# Begin Group "Export"

# PROP Default_Filter ""
# Begin Group "max headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\applyvc.h
# End Source File
# Begin Source File

SOURCE=.\bonemoddata.h
# End Source File
# Begin Source File

SOURCE=.\bones.h
# End Source File
# Begin Source File

SOURCE=.\holdvc.h
# End Source File
# Begin Source File

SOURCE=.\uvgen.h
# End Source File
# Begin Source File

SOURCE=.\vertexpaint.h
# End Source File
# End Group
# Begin Group "FaceAttributes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\UIDisplay\CurrAttribBase.h
# End Source File
# Begin Source File

SOURCE=..\perFaceDataMod\modData.h
# End Source File
# End Group
# Begin Group "Cloth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ClothModData.cpp
# End Source File
# Begin Source File

SOURCE=..\Cloth\ClothModData.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ExportImage.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ExportMap.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ExportMaterial.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ExportModifier.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ExportNode.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ExportObject.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ExportScene.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ExportTransform.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\texmaps.h
# End Source File
# Begin Source File

SOURCE=.\TupExport.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\TupExport.h
# End Source File
# End Group
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TupExport.def
# End Source File
# Begin Source File

SOURCE=.\TupExport.rc
# End Source File
# Begin Source File

SOURCE=.\TupExportPch.cpp
# ADD CPP /Yc"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\TupExportPch.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\avaLogo_256.bmp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\tuppic.bmp
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ClassDesc.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\ClassDesc.h
# End Source File
# Begin Source File

SOURCE=.\codednames.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\codednames.h
# End Source File
# Begin Source File

SOURCE=.\DllEntry.cpp
# ADD CPP /Yu"TupExportPch.h"
# End Source File
# Begin Source File

SOURCE=.\filepath.cpp

!IF  "$(CFG)" == "TupExport - Win32 Max Debug"

!ELSEIF  "$(CFG)" == "TupExport - Win32 Max Release"

!ELSEIF  "$(CFG)" == "TupExport - Win32 Max Hybrid"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\filepath.h
# End Source File
# End Group
# End Target
# End Project
