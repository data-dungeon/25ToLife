# Microsoft Developer Studio Generated NMAKE File, Based on HXT.dsp
!IF "$(CFG)" == ""
CFG=HXT - Win32 Debug
!MESSAGE No configuration specified. Defaulting to HXT - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HXT - Win32 Release" && "$(CFG)" != "HXT - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HXT.mak" CFG="HXT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HXT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HXT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HXT - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\PLY.hxt"


CLEAN :
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Export.obj"
	-@erase "$(INTDIR)\ExportActionsDialog.obj"
	-@erase "$(INTDIR)\ExportModelDialog.obj"
	-@erase "$(INTDIR)\ExportModelsDialog.obj"
	-@erase "$(INTDIR)\HXT.obj"
	-@erase "$(INTDIR)\HXT.pch"
	-@erase "$(INTDIR)\HXT.res"
	-@erase "$(INTDIR)\Import.obj"
	-@erase "$(INTDIR)\ImportModelDialog.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PLY.exp"
	-@erase "..\PLY.hxt"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\HXT.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HXT.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\lib\pc\Plugin.lib ..\..\..\lib\pc\Fileio.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\PLY.pdb" /machine:I386 /out:"..\PLY.hxt" /implib:"$(OUTDIR)\PLY.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Export.obj" \
	"$(INTDIR)\ExportActionsDialog.obj" \
	"$(INTDIR)\ExportModelDialog.obj" \
	"$(INTDIR)\ExportModelsDialog.obj" \
	"$(INTDIR)\HXT.obj" \
	"$(INTDIR)\Import.obj" \
	"$(INTDIR)\ImportModelDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\HXT.res"

"..\PLY.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\PLY.hxt"


CLEAN :
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Export.obj"
	-@erase "$(INTDIR)\ExportActionsDialog.obj"
	-@erase "$(INTDIR)\ExportModelDialog.obj"
	-@erase "$(INTDIR)\ExportModelsDialog.obj"
	-@erase "$(INTDIR)\HXT.obj"
	-@erase "$(INTDIR)\HXT.pch"
	-@erase "$(INTDIR)\HXT.res"
	-@erase "$(INTDIR)\Import.obj"
	-@erase "$(INTDIR)\ImportModelDialog.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PLY.exp"
	-@erase "$(OUTDIR)\PLY.pdb"
	-@erase "..\PLY.hxt"
	-@erase "..\PLY.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\HXT.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HXT.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\lib\pc\Plugin.lib ..\..\..\lib\pc\Fileio.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\PLY.pdb" /debug /machine:I386 /out:"..\PLY.hxt" /implib:"$(OUTDIR)\PLY.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Export.obj" \
	"$(INTDIR)\ExportActionsDialog.obj" \
	"$(INTDIR)\ExportModelDialog.obj" \
	"$(INTDIR)\ExportModelsDialog.obj" \
	"$(INTDIR)\HXT.obj" \
	"$(INTDIR)\Import.obj" \
	"$(INTDIR)\ImportModelDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\HXT.res"

"..\PLY.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("HXT.dep")
!INCLUDE "HXT.dep"
!ELSE 
!MESSAGE Warning: cannot find "HXT.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "HXT - Win32 Release" || "$(CFG)" == "HXT - Win32 Debug"
SOURCE=.\Entry.cpp

"$(INTDIR)\Entry.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\Export.cpp

"$(INTDIR)\Export.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\ExportActionsDialog.cpp

"$(INTDIR)\ExportActionsDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\ExportModelDialog.cpp

"$(INTDIR)\ExportModelDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\ExportModelsDialog.cpp

"$(INTDIR)\ExportModelsDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\HXT.cpp

"$(INTDIR)\HXT.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\HXT.rc

"$(INTDIR)\HXT.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Import.cpp

"$(INTDIR)\Import.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\ImportModelDialog.cpp

"$(INTDIR)\ImportModelDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\HXT.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\HXT.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

