# Microsoft Developer Studio Project File - Name="Mdlimport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Mdlimport - Win32 Max Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mdlimport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mdlimport.mak" CFG="Mdlimport - Win32 Max Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mdlimport - Win32 Max Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Mdlimport - Win32 Max Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Mdlimport - Win32 Max Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Max/Mdlimport", KKSAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mdlimport - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Mdlimport___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "Mdlimport___Win32_Max_Hybrid"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "..\..\libraries\mdllib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "..\..\libraries\mdllib" /I "..\..\libraries\cholib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x4ae30000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\Mdlimport.dli" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# SUBTRACT BASE LINK32 /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x4ae30000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\avalanche\Mdlimport.dli" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# SUBTRACT LINK32 /map /force

!ELSEIF  "$(CFG)" == "Mdlimport - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Mdlimport___Win32_Max_Release"
# PROP BASE Intermediate_Dir "Mdlimport___Win32_Max_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /I "..\..\libraries\mdllib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /I "..\..\libraries\mdllib" /I "..\..\libraries\cholib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x4ae30000" /subsystem:windows /dll /machine:I386 /out:"$(MAXDIR)\plugins\Mdlimport.dli" /libpath:"$(MAXSDK)\lib" /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x4ae30000" /subsystem:windows /dll /machine:I386 /out:"$(MAXDIR)\plugins\avalanche\Mdlimport.dli" /libpath:"$(MAXSDK)\lib" /release

!ELSEIF  "$(CFG)" == "Mdlimport - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Mdlimport___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "Mdlimport___Win32_Max_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "..\..\libraries\mdllib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "..\..\libraries\mdllib" /I "..\..\libraries\cholib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /FR /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x4ae30000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\Mdlimport.dli" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x4ae30000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDEBUGDIR)\exe\plugins\avalanche\Mdlimport.dli" /pdbtype:sept /libpath:"$(MAXSDK)\lib"

!ENDIF 

# Begin Target

# Name "Mdlimport - Win32 Max Hybrid"
# Name "Mdlimport - Win32 Max Release"
# Name "Mdlimport - Win32 Max Debug"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\max_macros.h
# End Source File
# Begin Source File

SOURCE=.\maxModelInfo.h
# End Source File
# Begin Source File

SOURCE=.\MaxTools.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxTools.h
# End Source File
# Begin Source File

SOURCE=.\Mdlimport.cpp
# End Source File
# Begin Source File

SOURCE=.\Mdlimport.h
# End Source File
# End Group
# Begin Group "FilePath"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\filepath.cpp
# End Source File
# Begin Source File

SOURCE=.\filepath.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\avaLogo_256.bmp
# End Source File
# Begin Source File

SOURCE=.\Mdlimport.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "DLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\Mdlimport.def
# End Source File
# End Group
# End Target
# End Project
