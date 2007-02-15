# Microsoft Developer Studio Generated NMAKE File, Based on HXT.dsp
!IF "$(CFG)" == ""
CFG=HXT - Win32 Release
!MESSAGE No configuration specified. Defaulting to HXT - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "HXT - Win32 Release" && "$(CFG)" != "HXT - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HXT.mak" CFG="HXT - Win32 Release"
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

ALL : "..\Duplicator.hxt"


CLEAN :
	-@erase "$(INTDIR)\DupDialog.obj"
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Evaluate.obj"
	-@erase "$(INTDIR)\HXT.obj"
	-@erase "$(INTDIR)\HXT.pch"
	-@erase "$(INTDIR)\HXT.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Duplicator.exp"
	-@erase "$(OUTDIR)\Duplicator.lib"
	-@erase "..\Duplicator.hxt"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\HXT.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HXT.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\lib\pc\Plugin.lib ..\..\..\lib\pc\Fileio.lib ..\..\..\lib\pc\matrix.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Duplicator.pdb" /machine:I386 /out:"..\Duplicator.hxt" /implib:"$(OUTDIR)\Duplicator.lib" 
LINK32_OBJS= \
	"$(INTDIR)\DupDialog.obj" \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Evaluate.obj" \
	"$(INTDIR)\HXT.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\HXT.res"

"..\Duplicator.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\Duplicator.hxt"


CLEAN :
	-@erase "$(INTDIR)\DupDialog.obj"
	-@erase "$(INTDIR)\Entry.obj"
	-@erase "$(INTDIR)\Evaluate.obj"
	-@erase "$(INTDIR)\HXT.obj"
	-@erase "$(INTDIR)\HXT.pch"
	-@erase "$(INTDIR)\HXT.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Duplicator.exp"
	-@erase "$(OUTDIR)\Duplicator.lib"
	-@erase "$(OUTDIR)\Duplicator.pdb"
	-@erase "..\Duplicator.hxt"
	-@erase "..\Duplicator.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\HXT.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HXT.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\lib\pc\Plugin.lib ..\..\..\lib\pc\matrix.lib ..\..\..\lib\pc\Fileio.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Duplicator.pdb" /debug /machine:I386 /out:"..\Duplicator.hxt" /implib:"$(OUTDIR)\Duplicator.lib" 
LINK32_OBJS= \
	"$(INTDIR)\DupDialog.obj" \
	"$(INTDIR)\Entry.obj" \
	"$(INTDIR)\Evaluate.obj" \
	"$(INTDIR)\HXT.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\HXT.res"

"..\Duplicator.hxt" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
SOURCE=.\DupDialog.cpp

"$(INTDIR)\DupDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\Entry.cpp

"$(INTDIR)\Entry.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\Evaluate.cpp

"$(INTDIR)\Evaluate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\HXT.cpp

"$(INTDIR)\HXT.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HXT.pch"


SOURCE=.\HXT.rc

"$(INTDIR)\HXT.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "HXT - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\HXT.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "HXT - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\HXT.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\HXT.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

