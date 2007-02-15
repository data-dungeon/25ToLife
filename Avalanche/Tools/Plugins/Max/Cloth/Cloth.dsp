# Microsoft Developer Studio Project File - Name="Cloth" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Cloth - Win32 Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cloth.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cloth.mak" CFG="Cloth - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cloth - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Cloth - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Cloth - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Max/Cloth", KBRBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cloth - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "$(MAXSDK)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"ClothPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib poly.lib mnmath.lib /nologo /base:"0x592a0000" /subsystem:windows /dll /machine:I386 /out:"$(MAXDIR)\plugins\Avalanche\Cloth.dlm" /libpath:"$(MAXSDK)\lib" /release

!ELSEIF  "$(CFG)" == "Cloth - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"ClothPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib poly.lib mnmath.lib /nologo /base:"0x592a0000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDEBUGDIR)\exe\plugins\Avalanche\Cloth.dlm" /pdbtype:sept /libpath:"$(MAXSDK)\lib"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Cloth___Win32_Hybrid"
# PROP BASE Intermediate_Dir "Cloth___Win32_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Cloth___Win32_Hybrid"
# PROP Intermediate_Dir "Cloth___Win32_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "$(MAXSDK)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"ClothPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib poly.lib mnmath.lib /nologo /base:"0x592a0000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\Avalanche\Cloth.dlm" /pdbtype:sept /libpath:"$(MAXSDK)\lib"

!ENDIF 

# Begin Target

# Name "Cloth - Win32 Release"
# Name "Cloth - Win32 Debug"
# Name "Cloth - Win32 Hybrid"
# Begin Group "Cloth Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Cloth.cpp

!IF  "$(CFG)" == "Cloth - Win32 Release"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Debug"

# ADD CPP /Yu"ClothPCH.h"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Hybrid"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Cloth.h
# End Source File
# Begin Source File

SOURCE=.\ClothModData.cpp

!IF  "$(CFG)" == "Cloth - Win32 Release"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Debug"

# ADD CPP /Yu"ClothPCH.h"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Hybrid"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ClothModData.h
# End Source File
# Begin Source File

SOURCE=.\DllEntry.cpp

!IF  "$(CFG)" == "Cloth - Win32 Release"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Debug"

# ADD CPP /Yu"ClothPCH.h"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Hybrid"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Registry.cpp

!IF  "$(CFG)" == "Cloth - Win32 Release"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Debug"

# ADD CPP /Yu"ClothPCH.h"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Hybrid"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\SelectMode.cpp

!IF  "$(CFG)" == "Cloth - Win32 Release"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Debug"

# ADD CPP /Yu"ClothPCH.h"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Hybrid"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SelectMode.h
# End Source File
# Begin Source File

SOURCE=.\SelectMouseProc.cpp

!IF  "$(CFG)" == "Cloth - Win32 Release"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Debug"

# ADD CPP /Yu"ClothPCH.h"

!ELSEIF  "$(CFG)" == "Cloth - Win32 Hybrid"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SelectMouseProc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Cloth.def
# End Source File
# Begin Source File

SOURCE=.\Cloth.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\sel_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\sel_tool.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ClothPCH.cpp
# ADD CPP /Yc"ClothPCH.h"
# End Source File
# Begin Source File

SOURCE=.\ClothPCH.h
# End Source File
# End Target
# End Project
