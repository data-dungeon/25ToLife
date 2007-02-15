# Microsoft Developer Studio Project File - Name="ImageAttributes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ImageAttributes - Win32 Max Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ImageAttributes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ImageAttributes.mak" CFG="ImageAttributes - Win32 Max Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ImageAttributes - Win32 Max Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ImageAttributes - Win32 Max Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ImageAttributes - Win32 Max Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Max/ImageAttributes", EPSBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ImageAttributes - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ImageAttributes___Win32_Max_Debug0"
# PROP BASE Intermediate_Dir "ImageAttributes___Win32_Max_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "$(MAXSDK)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR- /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"ImageAttributesPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x73860000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\Avalanche\ImageAttributes.dlu" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x73860000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDEBUGDIR)\exe\plugins\Avalanche\ImageAttributes.dlu" /pdbtype:sept /libpath:"$(MAXSDK)\lib"

!ELSEIF  "$(CFG)" == "ImageAttributes - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ImageAttributes___Win32_Max_Release"
# PROP BASE Intermediate_Dir "ImageAttributes___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /O2 /I "$(MAXSDK)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /GR- /GX /O2 /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"ImageAttributesPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x73860000" /subsystem:windows /dll /machine:I386 /out:"$(MAXDIR)\plugins\Avalanche\ImageAttributes.dlu" /libpath:"$(MAXSDK)\lib" /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x73860000" /subsystem:windows /dll /machine:I386 /out:"$(MAXDIR)\plugins\Avalanche\ImageAttributes.dlu" /libpath:"$(MAXSDK)\lib" /release

!ELSEIF  "$(CFG)" == "ImageAttributes - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ImageAttributes___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "ImageAttributes___Win32_Max_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "$(MAXSDK)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GR- /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"ImageAttributesPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x73860000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\Avalanche\ImageAttributes.dlu" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib /nologo /base:"0x73860000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\Avalanche\ImageAttributes.dlu" /pdbtype:sept /libpath:"$(MAXSDK)\lib"

!ENDIF 

# Begin Target

# Name "ImageAttributes - Win32 Max Debug"
# Name "ImageAttributes - Win32 Max Release"
# Name "ImageAttributes - Win32 Max Hybrid"
# Begin Group "Main files"

# PROP Default_Filter "cpp,h"
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

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageAttributes.h
# End Source File
# Begin Source File

SOURCE=.\ImageAttributesPCH.cpp
# ADD CPP /Yc"ImageAttributesPCH.h"
# End Source File
# Begin Source File

SOURCE=.\ImageAttributesPCH.h
# End Source File
# Begin Source File

SOURCE=.\ImageList.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageList.h
# End Source File
# Begin Source File

SOURCE=.\ImgAttrController.cpp
# End Source File
# Begin Source File

SOURCE=.\ImgAttrController.h
# End Source File
# Begin Source File

SOURCE=.\KeyValCommunicator.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyValCommunicator.h
# End Source File
# Begin Source File

SOURCE=.\PickMode.cpp
# End Source File
# Begin Source File

SOURCE=.\PickMode.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter "bmp,rc"
# Begin Source File

SOURCE=.\ImageAttributes.rc

!IF  "$(CFG)" == "ImageAttributes - Win32 Max Debug"

# ADD BASE RSC /l 0x409 /i "..\KeyVal"
# ADD RSC /l 0x409 /i "..\KeyVal" /i "../KeyVal"

!ELSEIF  "$(CFG)" == "ImageAttributes - Win32 Max Release"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "../KeyVal"

!ELSEIF  "$(CFG)" == "ImageAttributes - Win32 Max Hybrid"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "../KeyVal"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MainToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\MainToolbarMask.bmp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ImageAttributes.def
# End Source File
# End Target
# End Project
