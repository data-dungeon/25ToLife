# Microsoft Developer Studio Project File - Name="ObjectAttributes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ObjectAttributes - Win32 Max Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ObjectAttributes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ObjectAttributes.mak" CFG="ObjectAttributes - Win32 Max Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ObjectAttributes - Win32 Max Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ObjectAttributes - Win32 Max Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ObjectAttributes - Win32 Max Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Max/ObjectAttributes", PWSAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ObjectAttributes - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ObjectAttributes___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "ObjectAttributes___Win32_Max_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /I "..\UIDisplay" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"ObjectAttributesPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x2e6f0000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDEBUGDIR)\exe\plugins\avalanche\ObjectAttributes.dlu" /pdbtype:sept /libpath:"$(MAXDIR)\maxsdk\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x2e6f0000" /subsystem:windows /dll /debug /machine:I386 /out:"Max_Debug\ObjectAttributes.dlu" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# Begin Custom Build - Copying $(InputName) to Dependent Max Plugin Directory
InputPath=.\Max_Debug\ObjectAttributes.dlu
InputName=ObjectAttributes
SOURCE="$(InputPath)"

"$(MAXDEBUGDIR)\exe\plugins\avalanche\$(InputName).dlu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(MAXDEBUGDIR)\exe\plugins\avalanche\$(InputName).dlu

# End Custom Build

!ELSEIF  "$(CFG)" == "ObjectAttributes - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ObjectAttributes___Win32_Max_Release"
# PROP BASE Intermediate_Dir "ObjectAttributes___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"ObjectAttributesPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x2e6f0000" /subsystem:windows /dll /machine:I386 /out:"D:\Apps\3dsmax4\plugins\ObjectAttributes.dlu" /libpath:"D:\Apps\3dsmax4\maxsdk\lib" /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x2e6f0000" /subsystem:windows /dll /machine:I386 /out:"Max_Release\ObjectAttributes.dlu" /libpath:"$(MAXSDK)\lib" /release
# Begin Custom Build - Copying $(InputName) to Dependent Max Plugin Directory
InputPath=.\Max_Release\ObjectAttributes.dlu
InputName=ObjectAttributes
SOURCE="$(InputPath)"

"$(MAXDIR)\plugins\avalanche\$(InputName).dlu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(MAXDIR)\plugins\avalanche\$(InputName).dlu

# End Custom Build

!ELSEIF  "$(CFG)" == "ObjectAttributes - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ObjectAttributes___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "ObjectAttributes___Win32_Max_Hybrid"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"ObjectAttributesPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x2e6f0000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\Avalanche\ObjectAttributes.dlu" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x2e6f0000" /subsystem:windows /dll /debug /machine:I386 /out:"Max_Hybrid\ObjectAttributes.dlu" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# Begin Custom Build - Copying $(InputName) to Dependent Max Plugin Directory
InputPath=.\Max_Hybrid\ObjectAttributes.dlu
InputName=ObjectAttributes
SOURCE="$(InputPath)"

"$(MAXDIR)\plugins\avalanche\$(InputName).dlu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(MAXDIR)\plugins\avalanche\$(InputName).dlu

# End Custom Build

!ENDIF 

# Begin Target

# Name "ObjectAttributes - Win32 Max Debug"
# Name "ObjectAttributes - Win32 Max Release"
# Name "ObjectAttributes - Win32 Max Hybrid"
# Begin Group "Main Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectAttributes.def
# End Source File
# Begin Source File

SOURCE=.\ObjectAttributes.h
# End Source File
# Begin Source File

SOURCE=.\ObjectAttributesPCH.cpp
# ADD CPP /Yc"ObjectAttributesPCH.h"
# End Source File
# Begin Source File

SOURCE=.\ObjectAttributesPCH.h
# End Source File
# Begin Source File

SOURCE=.\SetupCustomControls.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupCustomControls.h
# End Source File
# End Group
# Begin Group "Attributes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CurrAttribController.cpp
# End Source File
# Begin Source File

SOURCE=.\CurrAttribController.h
# End Source File
# Begin Source File

SOURCE=.\CurrAttribData.cpp
# End Source File
# Begin Source File

SOURCE=.\CurrAttribData.h
# End Source File
# Begin Source File

SOURCE=.\ObjAttrController.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjAttrController.h
# End Source File
# Begin Source File

SOURCE=.\PickMode.cpp
# End Source File
# Begin Source File

SOURCE=.\PickMode.h
# End Source File
# End Group
# Begin Group "KeyVal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KeyValCommunicator.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValCommunicator.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ObjectAttributes.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "../../libraries/KeyVal" /i "../../libraries/UIDisplay" /i "../../libraries/AttrControls"
# End Source File
# Begin Source File

SOURCE=.\onetomany.ico
# End Source File
# Begin Source File

SOURCE=.\onetomanydisabled.ico
# End Source File
# Begin Source File

SOURCE=.\replace.ico
# End Source File
# Begin Source File

SOURCE=.\replacedisabled.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# End Target
# End Project
