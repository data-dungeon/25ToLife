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

!IF  "$(CFG)" == "HXT - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\$(HASHPATH)\HXT\AvalancheExporter.hxt"

!ELSE 

ALL : "CollectData - Win32 Release" "HashTools - Win32 Release" ".\$(HASHPATH)\HXT\AvalancheExporter.hxt"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"HashTools - Win32 ReleaseCLEAN" "CollectData - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Export.obj"
	-@erase "$(INTDIR)\ExportActionsDialog.obj"
	-@erase "$(INTDIR)\ExportModelDialog.obj"
	-@erase "$(INTDIR)\ExportModelsDialog.obj"
	-@erase "$(INTDIR)\Hxt.obj"
	-@erase "$(INTDIR)\HXT.pch"
	-@erase "$(INTDIR)\Hxt.res"
	-@erase "$(INTDIR)\Property.obj"
	-@erase "$(INTDIR)\PropertyTree.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AvalancheExporter.exp"
	-@erase ".\$(HASHPATH)\HXT\AvalancheExporter.hxt"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(HASHSDKPATH)\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Hxt.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HXT.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=$(HASHSDKPATH)\lib\pc\Plugin.lib $(HASHSDKPATH)\lib\pc\matrix.lib $(HASHSDKPATH)\lib\pc\Fileio.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\AvalancheExporter.pdb" /machine:I386 /out:"$(HASHPATH)\HXT\AvalancheExporter.hxt" /implib:"$(OUTDIR)\AvalancheExporter.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Export.obj" \
	"$(INTDIR)\ExportActionsDialog.obj" \
	"$(INTDIR)\ExportModelDialog.obj" \
	"$(INTDIR)\ExportModelsDialog.obj" \
	"$(INTDIR)\Hxt.obj" \
	"$(INTDIR)\Property.obj" \
	"$(INTDIR)\PropertyTree.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Hxt.res" \
	"..\AV4CollectData\Release\CollectData.lib"

".\$(HASHPATH)\HXT\AvalancheExporter.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\$(HASHPATH)\HXT\AvalancheExporter.hxt"

!ELSE 

ALL : "CollectData - Win32 Debug" "HashTools - Win32 Debug" ".\$(HASHPATH)\HXT\AvalancheExporter.hxt"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"HashTools - Win32 DebugCLEAN" "CollectData - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Export.obj"
	-@erase "$(INTDIR)\ExportActionsDialog.obj"
	-@erase "$(INTDIR)\ExportModelDialog.obj"
	-@erase "$(INTDIR)\ExportModelsDialog.obj"
	-@erase "$(INTDIR)\Hxt.obj"
	-@erase "$(INTDIR)\HXT.pch"
	-@erase "$(INTDIR)\Hxt.res"
	-@erase "$(INTDIR)\Property.obj"
	-@erase "$(INTDIR)\PropertyTree.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AvalancheExporter.exp"
	-@erase "$(OUTDIR)\AvalancheExporter.pdb"
	-@erase ".\$(HASHPATH)\HXT\AvalancheExporter.hxt"
	-@erase ".\$(HASHPATH)\HXT\AvalancheExporter.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(HASHSDKPATH)\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Hxt.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HXT.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=$(HASHSDKPATH)\lib\pc\Plugin.lib $(HASHSDKPATH)\lib\pc\matrix.lib $(HASHSDKPATH)\lib\pc\Fileio.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\AvalancheExporter.pdb" /debug /machine:I386 /out:"$(HASHPATH)\HXT\AvalancheExporter.hxt" /implib:"$(OUTDIR)\AvalancheExporter.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Export.obj" \
	"$(INTDIR)\ExportActionsDialog.obj" \
	"$(INTDIR)\ExportModelDialog.obj" \
	"$(INTDIR)\ExportModelsDialog.obj" \
	"$(INTDIR)\Hxt.obj" \
	"$(INTDIR)\Property.obj" \
	"$(INTDIR)\PropertyTree.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Hxt.res" \
	"..\AV4CollectData\Debug\CollectData.lib"

".\$(HASHPATH)\HXT\AvalancheExporter.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


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


SOURCE=.\Hxt.cpp

"$(INTDIR)\Hxt.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\Hxt.rc

"$(INTDIR)\Hxt.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Property.cpp

"$(INTDIR)\Property.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\PropertyTree.cpp

"$(INTDIR)\PropertyTree.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(HASHSDKPATH)\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\HXT.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(HASHSDKPATH)\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\HXT.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

!IF  "$(CFG)" == "HXT - Win32 Release"

"HashTools - Win32 Release" : 
   cd "\dev\Avalanche\tools\Plugins\Animation Master V9.5\HashTools"
   $(MAKE) /$(MAKEFLAGS) /F ".\HashTools.mak" CFG="HashTools - Win32 Release" 
   cd "..\AvalancheExporter"

"HashTools - Win32 ReleaseCLEAN" : 
   cd "\dev\Avalanche\tools\Plugins\Animation Master V9.5\HashTools"
   $(MAKE) /$(MAKEFLAGS) /F ".\HashTools.mak" CFG="HashTools - Win32 Release" RECURSE=1 CLEAN 
   cd "..\AvalancheExporter"

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

"HashTools - Win32 Debug" : 
   cd "\dev\Avalanche\tools\Plugins\Animation Master V9.5\HashTools"
   $(MAKE) /$(MAKEFLAGS) /F ".\HashTools.mak" CFG="HashTools - Win32 Debug" 
   cd "..\AvalancheExporter"

"HashTools - Win32 DebugCLEAN" : 
   cd "\dev\Avalanche\tools\Plugins\Animation Master V9.5\HashTools"
   $(MAKE) /$(MAKEFLAGS) /F ".\HashTools.mak" CFG="HashTools - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\AvalancheExporter"

!ENDIF 

!IF  "$(CFG)" == "HXT - Win32 Release"

"CollectData - Win32 Release" : 
   cd "\dev\Avalanche\tools\Plugins\Animation Master V9.5\AV4CollectData"
   $(MAKE) /$(MAKEFLAGS) /F ".\CollectData.mak" CFG="CollectData - Win32 Release" 
   cd "..\AvalancheExporter"

"CollectData - Win32 ReleaseCLEAN" : 
   cd "\dev\Avalanche\tools\Plugins\Animation Master V9.5\AV4CollectData"
   $(MAKE) /$(MAKEFLAGS) /F ".\CollectData.mak" CFG="CollectData - Win32 Release" RECURSE=1 CLEAN 
   cd "..\AvalancheExporter"

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

"CollectData - Win32 Debug" : 
   cd "\dev\Avalanche\tools\Plugins\Animation Master V9.5\AV4CollectData"
   $(MAKE) /$(MAKEFLAGS) /F ".\CollectData.mak" CFG="CollectData - Win32 Debug" 
   cd "..\AvalancheExporter"

"CollectData - Win32 DebugCLEAN" : 
   cd "\dev\Avalanche\tools\Plugins\Animation Master V9.5\AV4CollectData"
   $(MAKE) /$(MAKEFLAGS) /F ".\CollectData.mak" CFG="CollectData - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\AvalancheExporter"

!ENDIF 


!ENDIF 

