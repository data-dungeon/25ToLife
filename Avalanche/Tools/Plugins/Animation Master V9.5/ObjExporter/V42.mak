# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Power Macintosh Dynamic-Link Library" 0x0402
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=HXT - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to HXT - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HXT - Win32 Release" && "$(CFG)" != "HXT - Win32 Debug" &&\
 "$(CFG)" != "HXT - Power Macintosh Release" && "$(CFG)" !=\
 "HXT - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "V42.mak" CFG="HXT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HXT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HXT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HXT - Power Macintosh Release" (based on\
 "Power Macintosh Dynamic-Link Library")
!MESSAGE "HXT - Power Macintosh Debug" (based on\
 "Power Macintosh Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "HXT - Win32 Debug"

!IF  "$(CFG)" == "HXT - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\AV2.hxt"

CLEAN : 
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Export.obj"
	-@erase "$(INTDIR)\ExportActionsDialog.obj"
	-@erase "$(INTDIR)\ExportModelDialog.obj"
	-@erase "$(INTDIR)\ExportModelsDialog.obj"
	-@erase "$(INTDIR)\Hxt.obj"
	-@erase "$(INTDIR)\HXT.res"
	-@erase "$(INTDIR)\Import.obj"
	-@erase "$(INTDIR)\ImportModelDialog.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\V42.pch"
	-@erase "$(OUTDIR)\AV2.exp"
	-@erase "$(OUTDIR)\AV2.lib"
	-@erase "..\AV2.hxt"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/V42.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/HXT.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/V42.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\..\..\lib\pc\Fileio.lib ..\..\..\lib\pc\Plugin.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\AV2.hxt"
LINK32_FLAGS=..\..\..\lib\pc\Fileio.lib ..\..\..\lib\pc\Plugin.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/AV2.pdb" /machine:I386\
 /out:"..\AV2.hxt" /implib:"$(OUTDIR)/AV2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Export.obj" \
	"$(INTDIR)\ExportActionsDialog.obj" \
	"$(INTDIR)\ExportModelDialog.obj" \
	"$(INTDIR)\ExportModelsDialog.obj" \
	"$(INTDIR)\Hxt.obj" \
	"$(INTDIR)\HXT.res" \
	"$(INTDIR)\Import.obj" \
	"$(INTDIR)\ImportModelDialog.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\AV2.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\AV2.hxt"

CLEAN : 
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Export.obj"
	-@erase "$(INTDIR)\ExportActionsDialog.obj"
	-@erase "$(INTDIR)\ExportModelDialog.obj"
	-@erase "$(INTDIR)\ExportModelsDialog.obj"
	-@erase "$(INTDIR)\Hxt.obj"
	-@erase "$(INTDIR)\HXT.res"
	-@erase "$(INTDIR)\Import.obj"
	-@erase "$(INTDIR)\ImportModelDialog.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\V42.pch"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\AV2.exp"
	-@erase "$(OUTDIR)\AV2.lib"
	-@erase "$(OUTDIR)\AV2.pdb"
	-@erase "..\AV2.hxt"
	-@erase "..\AV2.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/V42.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/HXT.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/V42.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\..\..\lib\pc\Fileio.lib ..\..\..\lib\pc\Plugin.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\AV2.hxt"
LINK32_FLAGS=..\..\..\lib\pc\Fileio.lib ..\..\..\lib\pc\Plugin.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/AV2.pdb" /debug\
 /machine:I386 /out:"..\AV2.hxt" /implib:"$(OUTDIR)/AV2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Export.obj" \
	"$(INTDIR)\ExportActionsDialog.obj" \
	"$(INTDIR)\ExportModelDialog.obj" \
	"$(INTDIR)\ExportModelsDialog.obj" \
	"$(INTDIR)\Hxt.obj" \
	"$(INTDIR)\HXT.res" \
	"$(INTDIR)\Import.obj" \
	"$(INTDIR)\ImportModelDialog.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\AV2.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PMacRel"
# PROP BASE Intermediate_Dir "PMacRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PMacRel"
# PROP Intermediate_Dir "PMacRel"
# PROP Target_Dir ""
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\AV2.hxt" "$(OUTDIR)\AV2.trg"

CLEAN : 
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Export.obj"
	-@erase "$(INTDIR)\ExportActionsDialog.obj"
	-@erase "$(INTDIR)\ExportModelDialog.obj"
	-@erase "$(INTDIR)\ExportModelsDialog.obj"
	-@erase "$(INTDIR)\Hxt.obj"
	-@erase "$(INTDIR)\HXT.rsc"
	-@erase "$(INTDIR)\Import.obj"
	-@erase "$(INTDIR)\ImportModelDialog.obj"
	-@erase "$(INTDIR)\Mac.rsc"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\V42.pch"
	-@erase "$(OUTDIR)\AV2.exp"
	-@erase "$(OUTDIR)\AV2.hxt"
	-@erase "$(OUTDIR)\AV2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /ppc
# ADD MTL /nologo /D "NDEBUG" /ppc
MTL_PROJ=/nologo /D "NDEBUG" /ppc 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /D "_USRDLL"\
 /Fp"$(INTDIR)/V42.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/HXT.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/V42.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:nobundle /dll /machine:MPPC
# ADD LINK32 ..\..\..\lib\mac\Fileio.lib ..\..\..\lib\mac\Plugin.lib /nologo /mac:nobundle /mac:creator="IE  " /dll /machine:MPPC /out:"pmacrel\AV2.hxt"
LINK32_FLAGS=..\..\..\lib\mac\Fileio.lib ..\..\..\lib\mac\Plugin.lib /nologo\
 /mac:nobundle /mac:type="shlb" /mac:creator="IE  "\
 /mac:init="WlmConnectionInit" /mac:term="WlmConnectionTerm" /dll\
 /incremental:no /pdb:"$(OUTDIR)/AV2.pdb" /machine:MPPC /out:"$(OUTDIR)/AV2.hxt"\
 /implib:"$(OUTDIR)/AV2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Export.obj" \
	"$(INTDIR)\ExportActionsDialog.obj" \
	"$(INTDIR)\ExportModelDialog.obj" \
	"$(INTDIR)\ExportModelsDialog.obj" \
	"$(INTDIR)\Hxt.obj" \
	"$(INTDIR)\HXT.rsc" \
	"$(INTDIR)\Import.obj" \
	"$(INTDIR)\ImportModelDialog.obj" \
	"$(INTDIR)\Mac.rsc" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\AV2.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/AV2.hxt"

"$(OUTDIR)\AV2.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\AV2.hxt\
 "$(MFILE32_DEST):HXT:AV2.hxt">"$(OUTDIR)\AV2.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\AV2.hxt\
 "$(MFILE32_DEST):HXT:AV2.hxt">"$(OUTDIR)\AV2.trg"
	

!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PMacDbg"
# PROP BASE Intermediate_Dir "PMacDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PMacDbg"
# PROP Intermediate_Dir "PMacDbg"
# PROP Target_Dir ""
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\AV2.hxt"

CLEAN : 
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Export.obj"
	-@erase "$(INTDIR)\ExportActionsDialog.obj"
	-@erase "$(INTDIR)\ExportModelDialog.obj"
	-@erase "$(INTDIR)\ExportModelsDialog.obj"
	-@erase "$(INTDIR)\Hxt.obj"
	-@erase "$(INTDIR)\HXT.rsc"
	-@erase "$(INTDIR)\Import.obj"
	-@erase "$(INTDIR)\ImportModelDialog.obj"
	-@erase "$(INTDIR)\Mac.rsc"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\V42.pch"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\AV2.exp"
	-@erase "$(OUTDIR)\AV2.hxt"
	-@erase "$(OUTDIR)\AV2.ilk"
	-@erase "$(OUTDIR)\AV2.lib"
	-@erase "$(OUTDIR)\AV2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /ppc
# ADD MTL /nologo /D "_DEBUG" /ppc
MTL_PROJ=/nologo /D "_DEBUG" /ppc 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /QPm /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\include" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /QPm /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\include" /D "_MAC" /D\
 "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /D\
 "_USRDLL" /Fp"$(INTDIR)/V42.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QPm /c 
CPP_OBJS=.\PMacDbg/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/HXT.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/V42.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:nobundle /dll /debug /machine:MPPC
# ADD LINK32 ..\..\..\lib\mac\Fileio.lib ..\..\..\lib\mac\Plugin.lib /nologo /mac:nobundle /mac:creator="IE  " /dll /debug /machine:MPPC /out:"pmacdbg\AV2.hxt"
LINK32_FLAGS=..\..\..\lib\mac\Fileio.lib ..\..\..\lib\mac\Plugin.lib /nologo\
 /mac:nobundle /mac:type="shlb" /mac:creator="IE  "\
 /mac:init="WlmConnectionInit" /mac:term="WlmConnectionTerm" /dll\
 /incremental:yes /pdb:"$(OUTDIR)/AV2.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/AV2.hxt" /implib:"$(OUTDIR)/AV2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Export.obj" \
	"$(INTDIR)\ExportActionsDialog.obj" \
	"$(INTDIR)\ExportModelDialog.obj" \
	"$(INTDIR)\ExportModelsDialog.obj" \
	"$(INTDIR)\Hxt.obj" \
	"$(INTDIR)\HXT.rsc" \
	"$(INTDIR)\Import.obj" \
	"$(INTDIR)\ImportModelDialog.obj" \
	"$(INTDIR)\Mac.rsc" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\AV2.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	

!ENDIF 

################################################################################
# Begin Target

# Name "HXT - Win32 Release"
# Name "HXT - Win32 Debug"
# Name "HXT - Power Macintosh Release"
# Name "HXT - Power Macintosh Debug"

!IF  "$(CFG)" == "HXT - Win32 Release"

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/V42.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\V42.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/V42.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\V42.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "_MAC" /D "_MPPC_" /D\
 "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /D "_USRDLL"\
 /Fp"$(INTDIR)/V42.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\V42.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\include" /D "_MAC" /D\
 "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /D\
 "_USRDLL" /Fp"$(INTDIR)/V42.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /QPm /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\V42.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Export.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_EXPOR=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashFile.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HAction.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\sdk\HChor.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HProgres.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportActionsDialog.h"\
	".\ExportModelDialog.h"\
	".\ExportModelsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPOR=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Export.obj" : $(SOURCE) $(DEP_CPP_EXPOR) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_EXPOR=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashFile.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HAction.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\sdk\HChor.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HProgres.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportActionsDialog.h"\
	".\ExportModelDialog.h"\
	".\ExportModelsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPOR=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Export.obj" : $(SOURCE) $(DEP_CPP_EXPOR) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_EXPOR=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashFile.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HAction.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\sdk\HChor.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HProgres.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportActionsDialog.h"\
	".\ExportModelDialog.h"\
	".\ExportModelsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPOR=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Export.obj" : $(SOURCE) $(DEP_CPP_EXPOR) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_EXPOR=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashFile.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HAction.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\sdk\HChor.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HProgres.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportActionsDialog.h"\
	".\ExportModelDialog.h"\
	".\ExportModelsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPOR=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Export.obj" : $(SOURCE) $(DEP_CPP_EXPOR) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ExportActionsDialog.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_EXPORT=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HAction.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportActionsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORT=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportActionsDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORT) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_EXPORT=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HAction.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportActionsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORT=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportActionsDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORT) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_EXPORT=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HAction.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportActionsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORT=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportActionsDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORT) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_EXPORT=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HAction.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportActionsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORT=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportActionsDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORT) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ExportModelDialog.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_EXPORTM=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportModelDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORTM=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportModelDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORTM) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_EXPORTM=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportModelDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORTM=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportModelDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORTM) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_EXPORTM=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportModelDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORTM=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportModelDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORTM) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_EXPORTM=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportModelDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORTM=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportModelDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORTM) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ExportModelsDialog.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_EXPORTMO=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportModelsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORTMO=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportModelsDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORTMO) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_EXPORTMO=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportModelsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORTMO=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportModelsDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORTMO) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_EXPORTMO=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportModelsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORTMO=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportModelsDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORTMO) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_EXPORTMO=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\SDK\HProject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\ExportModelsDialog.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_EXPORTMO=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ExportModelsDialog.obj" : $(SOURCE) $(DEP_CPP_EXPORTMO) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Import.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_IMPOR=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashFile.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\ImportModelDialog.h"\
	".\StdAfx.h"\
	
NODEP_CPP_IMPOR=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Import.obj" : $(SOURCE) $(DEP_CPP_IMPOR) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_IMPOR=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashFile.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\ImportModelDialog.h"\
	".\StdAfx.h"\
	
NODEP_CPP_IMPOR=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Import.obj" : $(SOURCE) $(DEP_CPP_IMPOR) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_IMPOR=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashFile.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\ImportModelDialog.h"\
	".\StdAfx.h"\
	
NODEP_CPP_IMPOR=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Import.obj" : $(SOURCE) $(DEP_CPP_IMPOR) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_IMPOR=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashFile.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\sdk\HBone.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SDK\HModel.h"\
	"..\..\..\include\sdk\HObject.h"\
	"..\..\..\include\sdk\HSegment.h"\
	"..\..\..\include\sdk\HPropert.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\ImportModelDialog.h"\
	".\StdAfx.h"\
	
NODEP_CPP_IMPOR=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Import.obj" : $(SOURCE) $(DEP_CPP_IMPOR) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ImportModelDialog.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_IMPORT=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\ImportModelDialog.h"\
	".\StdAfx.h"\
	
NODEP_CPP_IMPORT=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ImportModelDialog.obj" : $(SOURCE) $(DEP_CPP_IMPORT) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_IMPORT=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\ImportModelDialog.h"\
	".\StdAfx.h"\
	
NODEP_CPP_IMPORT=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ImportModelDialog.obj" : $(SOURCE) $(DEP_CPP_IMPORT) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_IMPORT=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\ImportModelDialog.h"\
	".\StdAfx.h"\
	
NODEP_CPP_IMPORT=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ImportModelDialog.obj" : $(SOURCE) $(DEP_CPP_IMPORT) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_IMPORT=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\F_Protos.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\SDK\Misc.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\ImportModelDialog.h"\
	".\StdAfx.h"\
	
NODEP_CPP_IMPORT=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\ImportModelDialog.obj" : $(SOURCE) $(DEP_CPP_IMPORT) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Entry.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_ENTRY=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\SDK\Entry.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_ENTRY=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Entry.obj" : $(SOURCE) $(DEP_CPP_ENTRY) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_ENTRY=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\SDK\Entry.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_ENTRY=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Entry.obj" : $(SOURCE) $(DEP_CPP_ENTRY) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_ENTRY=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\SDK\Entry.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_ENTRY=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Entry.obj" : $(SOURCE) $(DEP_CPP_ENTRY) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_ENTRY=\
	"..\..\..\include\Allocate.h"\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\DataType.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\FileCons.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\HashTime.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Macros.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\include\ObjType.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\PatchID.h"\
	"..\..\..\include\Quaterni.h"\
	"..\..\..\include\Rotate.h"\
	"..\..\..\include\SDK\Entry.h"\
	"..\..\..\include\SDK\HHashObj.h"\
	"..\..\..\include\SingleLo.h"\
	"..\..\..\include\StrClass.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_ENTRY=\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Entry.obj" : $(SOURCE) $(DEP_CPP_ENTRY) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mac.r

!IF  "$(CFG)" == "HXT - Win32 Release"

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"


"$(INTDIR)\Mac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Mac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"


"$(INTDIR)\Mac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Mac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Hxt.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_CPP_HXT_C=\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Localize.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_HXT_C=\
	"..\..\..\Include\MacOS\Files.h"\
	"..\..\..\Include\MacOS\ToolUtil.h"\
	"..\..\..\Include\MacOS\Types.h"\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Hxt.obj" : $(SOURCE) $(DEP_CPP_HXT_C) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_CPP_HXT_C=\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Localize.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_HXT_C=\
	"..\..\..\Include\MacOS\Files.h"\
	"..\..\..\Include\MacOS\ToolUtil.h"\
	"..\..\..\Include\MacOS\Types.h"\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Hxt.obj" : $(SOURCE) $(DEP_CPP_HXT_C) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_CPP_HXT_C=\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Localize.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_HXT_C=\
	"..\..\..\Include\MacOS\Files.h"\
	"..\..\..\Include\MacOS\ToolUtil.h"\
	"..\..\..\Include\MacOS\Types.h"\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Hxt.obj" : $(SOURCE) $(DEP_CPP_HXT_C) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_CPP_HXT_C=\
	"..\..\..\include\ColorVec.h"\
	"..\..\..\include\Exports.h"\
	"..\..\..\include\Fastftol.h"\
	"..\..\..\include\HashArra.h"\
	"..\..\..\include\IEModel.h"\
	"..\..\..\include\Localize.h"\
	"..\..\..\include\MapLimit.h"\
	"..\..\..\Include\OldAttr.h"\
	"..\..\..\include\Vector.h"\
	"..\..\..\include\Vector2.h"\
	".\Hxt.h"\
	".\StdAfx.h"\
	
NODEP_CPP_HXT_C=\
	"..\..\..\Include\MacOS\Files.h"\
	"..\..\..\Include\MacOS\ToolUtil.h"\
	"..\..\..\Include\MacOS\Types.h"\
	"..\..\..\Include\smlmath.h"\
	

"$(INTDIR)\Hxt.obj" : $(SOURCE) $(DEP_CPP_HXT_C) "$(INTDIR)"\
 "$(INTDIR)\V42.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HXT.rc

!IF  "$(CFG)" == "HXT - Win32 Release"

DEP_RSC_HXT_R=\
	".\res\HXT.rc2"\
	

"$(INTDIR)\HXT.res" : $(SOURCE) $(DEP_RSC_HXT_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

DEP_RSC_HXT_R=\
	".\res\HXT.rc2"\
	

"$(INTDIR)\HXT.res" : $(SOURCE) $(DEP_RSC_HXT_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Release"

DEP_RSC_HXT_R=\
	".\res\HXT.rc2"\
	

"$(INTDIR)\HXT.rsc" : $(SOURCE) $(DEP_RSC_HXT_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HXT - Power Macintosh Debug"

DEP_RSC_HXT_R=\
	".\res\HXT.rc2"\
	

"$(INTDIR)\HXT.rsc" : $(SOURCE) $(DEP_RSC_HXT_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
