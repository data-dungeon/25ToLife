# Microsoft Developer Studio Project File - Name="perFaceDataMod" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=perFaceDataMod - Win32 Max Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "perFaceDataMod.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "perFaceDataMod.mak" CFG="perFaceDataMod - Win32 Max Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "perFaceDataMod - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "perFaceDataMod - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "perFaceDataMod - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "perFaceDataMod - Win32 Max Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "perFaceDataMod - Win32 Max Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "perFaceDataMod - Win32 Max Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Max/perFaceDataMod", WSTAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "perFaceDataMod - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "_Release"
# PROP Intermediate_Dir "_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I "C:\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib /nologo /base:"0x39140000" /subsystem:windows /dll /machine:I386 /out:"$(MAXDIR).\plugins\avalanche\perFaceDataMod.dlm" /libpath:"C:\maxsdk\lib" /release

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "_Debug"
# PROP Intermediate_Dir "_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "$(MAXSDK)\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo".\Debug/" /Fd".\Debug/" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib /nologo /base:"0x39140000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDEBUGDIR)\exe\plugins\avalanche\perFaceDataMod.dlm" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "perFaceDataMod___Win32_Hybrid"
# PROP BASE Intermediate_Dir "perFaceDataMod___Win32_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "_Hybrid"
# PROP Intermediate_Dir "_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GR /GX /ZI /Od /I "$(MAXSDK)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib /nologo /base:"0x39140000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR).\plugins\avalanche\perFaceDataMod.dlm" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# SUBTRACT LINK32 /force

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "perFaceDataMod___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "perFaceDataMod___Win32_Max_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "$(MAXSDK)\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo".\Debug/" /Fd".\Debug/" /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /I "$(AVTOOLS)\Plugins\Max\UIDisplay" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FR /Yu"pfdModPCH.h" /Fo".\Debug/" /Fd".\Debug/" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib /nologo /base:"0x39140000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDEBUGDIR)\exe\plugins\avalanche\perFaceDataMod.dlm" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib MNMath.lib poly.lib /nologo /base:"0x39140000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDEBUGDIR)\exe\plugins\avalanche\perFaceDataMod.dlm" /pdbtype:sept /libpath:"$(MAXSDK)\lib" /libpath:"$(AVTOOLS)" /libpath:"$(AVTOOLS)\Max\utilities\UIDisplay" /libpath:"$(AVTOOLS)\Tupperware"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "perFaceDataMod___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "perFaceDataMod___Win32_Max_Hybrid"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GR /GX /ZI /Od /I "$(MAXSDK)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GR /GX /ZI /Od /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /I "$(AVTOOLS)\Plugins\Max\UIDisplay" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /Fp"" /Yu"pfdModPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib /nologo /base:"0x39140000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR).\plugins\avalanche\perFaceDataMod.dlm" /pdbtype:sept /libpath:"$(MAXSDK)\lib"
# SUBTRACT BASE LINK32 /force
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib /nologo /base:"0x39140000" /subsystem:windows /dll /debug /machine:I386 /out:"$(MAXDIR)\plugins\avalanche\perFaceDataMod.dlm" /pdbtype:sept /libpath:"$(MAXSDK)\lib" /libpath:"$(AVTOOLS)" /libpath:"$(AVTOOLS)\Max\utilities\UIDisplay" /libpath:"$(AVTOOLS)\Tupperware"
# SUBTRACT LINK32 /force

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "perFaceDataMod___Win32_Max_Release"
# PROP BASE Intermediate_Dir "perFaceDataMod___Win32_Max_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I "C:\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I "$(MAXSDK)\include" /I "$(AVTOOLS)" /I "$(AVTOOLS)\Plugins\Max\UIDisplay" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"pfdModPCH.h" /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib /nologo /base:"0x39140000" /subsystem:windows /dll /machine:I386 /out:"$(MAXDIR).\plugins\avalanche\perFaceDataMod.dlm" /libpath:"C:\maxsdk\lib" /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib manipsys.lib paramblk2.lib /nologo /base:"0x39140000" /subsystem:windows /dll /machine:I386 /out:"$(MAXDIR)\plugins\avalanche\perFaceDataMod.dlm" /libpath:"$(MAXSDK)\lib" /libpath:"$(AVTOOLS)" /libpath:"$(AVTOOLS)\Max\utilities\UIDisplay" /libpath:"$(AVTOOLS)\Tupperware" /release

!ENDIF 

# Begin Target

# Name "perFaceDataMod - Win32 Release"
# Name "perFaceDataMod - Win32 Debug"
# Name "perFaceDataMod - Win32 Hybrid"
# Name "perFaceDataMod - Win32 Max Debug"
# Name "perFaceDataMod - Win32 Max Hybrid"
# Name "perFaceDataMod - Win32 Max Release"
# Begin Group "PerFaceData Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\define.h
# End Source File
# Begin Source File

SOURCE=.\dllmain.cpp

!IF  "$(CFG)" == "perFaceDataMod - Win32 Release"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Release"

# ADD CPP /Yu"pfdModPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FaceBase.cpp
# End Source File
# Begin Source File

SOURCE=.\FaceBase.h
# End Source File
# Begin Source File

SOURCE=.\ModData.cpp

!IF  "$(CFG)" == "perFaceDataMod - Win32 Release"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Release"

# ADD CPP /Yu"pfdModPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ModData.h
# End Source File
# Begin Source File

SOURCE=.\PaintMode.cpp

!IF  "$(CFG)" == "perFaceDataMod - Win32 Release"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Release"

# ADD CPP /Yu"pfdModPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PaintMode.h
# End Source File
# Begin Source File

SOURCE=.\PaintMouseProc.cpp

!IF  "$(CFG)" == "perFaceDataMod - Win32 Release"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Release"

# ADD CPP /Yu"pfdModPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PaintMouseProc.h
# End Source File
# Begin Source File

SOURCE=.\PerFaceDataMod.cpp

!IF  "$(CFG)" == "perFaceDataMod - Win32 Release"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Release"

# ADD CPP /Yu"pfdModPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PerFaceDataMod.h
# End Source File
# Begin Source File

SOURCE=.\restore.cpp

!IF  "$(CFG)" == "perFaceDataMod - Win32 Release"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Debug"

# ADD CPP /Yu"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Release"

# ADD CPP /Yu"pfdModPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\restore.h
# End Source File
# Begin Source File

SOURCE=.\veroverrides.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\avaLogo_256.bmp
# End Source File
# Begin Source File

SOURCE=.\perFaceDataMod.def
# End Source File
# Begin Source File

SOURCE=.\perFaceDataMod.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\selmask.bmp
# End Source File
# Begin Source File

SOURCE=.\selTypes.bmp
# End Source File
# End Group
# Begin Group "Config Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FaceAttr.cfg
# End Source File
# End Group
# Begin Source File

SOURCE=.\pfdModPCH.cpp

!IF  "$(CFG)" == "perFaceDataMod - Win32 Release"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Debug"

# ADD CPP /I "$(AVTOOLS)\Tupperware" /I "$(AVTOOLS)\Max\utilities\UIDisplay" /Yc"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Debug"

# ADD CPP /Yc"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Hybrid"

# ADD CPP /Yc"pfdModPCH.h"

!ELSEIF  "$(CFG)" == "perFaceDataMod - Win32 Max Release"

# ADD CPP /Yc"pfdModPCH.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pfdModPCH.h
# End Source File
# End Target
# End Project
