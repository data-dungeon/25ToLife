# Microsoft Developer Studio Generated NMAKE File, Based on AMViewer.dsp
!IF "$(CFG)" == ""
CFG=AMViewer - Win32 Release
!MESSAGE No configuration specified. Defaulting to AMViewer - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "AMViewer - Win32 Release" && "$(CFG)" != "AMViewer - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AMViewer.mak" CFG="AMViewer - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AMViewer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AMViewer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "AMViewer - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\lib\pc\AMViewer.exe" "$(OUTDIR)\AMViewer.bsc"


CLEAN :
	-@erase "$(INTDIR)\Action.obj"
	-@erase "$(INTDIR)\Action.sbr"
	-@erase "$(INTDIR)\AMViewer.obj"
	-@erase "$(INTDIR)\AMViewer.pch"
	-@erase "$(INTDIR)\AMViewer.res"
	-@erase "$(INTDIR)\AMViewer.sbr"
	-@erase "$(INTDIR)\AMViewerDoc.obj"
	-@erase "$(INTDIR)\AMViewerDoc.sbr"
	-@erase "$(INTDIR)\AMViewerSplitter.obj"
	-@erase "$(INTDIR)\AMViewerSplitter.sbr"
	-@erase "$(INTDIR)\AMViewerView.obj"
	-@erase "$(INTDIR)\AMViewerView.sbr"
	-@erase "$(INTDIR)\Archive.obj"
	-@erase "$(INTDIR)\Archive.sbr"
	-@erase "$(INTDIR)\AV2.obj"
	-@erase "$(INTDIR)\AV2.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\Model.obj"
	-@erase "$(INTDIR)\Model.sbr"
	-@erase "$(INTDIR)\Mot.obj"
	-@erase "$(INTDIR)\Mot.sbr"
	-@erase "$(INTDIR)\Move.obj"
	-@erase "$(INTDIR)\Move.sbr"
	-@erase "$(INTDIR)\NoAccelEdit.obj"
	-@erase "$(INTDIR)\NoAccelEdit.sbr"
	-@erase "$(INTDIR)\Options.obj"
	-@erase "$(INTDIR)\Options.sbr"
	-@erase "$(INTDIR)\PatchModel.obj"
	-@erase "$(INTDIR)\PatchModel.sbr"
	-@erase "$(INTDIR)\Ply.obj"
	-@erase "$(INTDIR)\Ply.sbr"
	-@erase "$(INTDIR)\PolyModel.obj"
	-@erase "$(INTDIR)\PolyModel.sbr"
	-@erase "$(INTDIR)\Render.obj"
	-@erase "$(INTDIR)\Render.sbr"
	-@erase "$(INTDIR)\ScriptView.obj"
	-@erase "$(INTDIR)\ScriptView.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Time.obj"
	-@erase "$(INTDIR)\Time.sbr"
	-@erase "$(INTDIR)\Transform.obj"
	-@erase "$(INTDIR)\Transform.sbr"
	-@erase "$(INTDIR)\Turn.obj"
	-@erase "$(INTDIR)\Turn.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Zoom.obj"
	-@erase "$(INTDIR)\Zoom.sbr"
	-@erase "$(OUTDIR)\AMViewer.bsc"
	-@erase "..\lib\pc\AMViewer.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GR /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AMViewer.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AMViewer.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AMViewer.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Action.sbr" \
	"$(INTDIR)\AMViewer.sbr" \
	"$(INTDIR)\AMViewerDoc.sbr" \
	"$(INTDIR)\AMViewerSplitter.sbr" \
	"$(INTDIR)\AMViewerView.sbr" \
	"$(INTDIR)\Archive.sbr" \
	"$(INTDIR)\AV2.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\Model.sbr" \
	"$(INTDIR)\Mot.sbr" \
	"$(INTDIR)\Move.sbr" \
	"$(INTDIR)\NoAccelEdit.sbr" \
	"$(INTDIR)\Options.sbr" \
	"$(INTDIR)\PatchModel.sbr" \
	"$(INTDIR)\Ply.sbr" \
	"$(INTDIR)\PolyModel.sbr" \
	"$(INTDIR)\Render.sbr" \
	"$(INTDIR)\ScriptView.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\Time.sbr" \
	"$(INTDIR)\Transform.sbr" \
	"$(INTDIR)\Turn.sbr" \
	"$(INTDIR)\Zoom.sbr"

"$(OUTDIR)\AMViewer.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\AMViewer.pdb" /machine:I386 /out:"..\lib\pc\AMViewer.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Action.obj" \
	"$(INTDIR)\AMViewer.obj" \
	"$(INTDIR)\AMViewerDoc.obj" \
	"$(INTDIR)\AMViewerSplitter.obj" \
	"$(INTDIR)\AMViewerView.obj" \
	"$(INTDIR)\Archive.obj" \
	"$(INTDIR)\AV2.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\Model.obj" \
	"$(INTDIR)\Mot.obj" \
	"$(INTDIR)\Move.obj" \
	"$(INTDIR)\NoAccelEdit.obj" \
	"$(INTDIR)\Options.obj" \
	"$(INTDIR)\PatchModel.obj" \
	"$(INTDIR)\Ply.obj" \
	"$(INTDIR)\PolyModel.obj" \
	"$(INTDIR)\Render.obj" \
	"$(INTDIR)\ScriptView.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Time.obj" \
	"$(INTDIR)\Transform.obj" \
	"$(INTDIR)\Turn.obj" \
	"$(INTDIR)\Zoom.obj" \
	"$(INTDIR)\AMViewer.res" \
	"..\Lib\Pc\Bitmap.lib" \
	"..\Lib\Pc\FileIO.lib" \
	"..\Lib\Pc\Matrix.lib" \
	"..\Lib\Pc\Patch.lib" \
	"..\..\Program Files\Microsoft Visual Studio\Vc98\Lib\Opengl32.lib" \
	"..\..\Program Files\Microsoft Visual Studio\Vc98\Lib\Winmm.lib"

"..\lib\pc\AMViewer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AMViewer - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\lib\pc\AMViewer.exe" "$(OUTDIR)\AMViewer.bsc"


CLEAN :
	-@erase "$(INTDIR)\Action.obj"
	-@erase "$(INTDIR)\Action.sbr"
	-@erase "$(INTDIR)\AMViewer.obj"
	-@erase "$(INTDIR)\AMViewer.pch"
	-@erase "$(INTDIR)\AMViewer.res"
	-@erase "$(INTDIR)\AMViewer.sbr"
	-@erase "$(INTDIR)\AMViewerDoc.obj"
	-@erase "$(INTDIR)\AMViewerDoc.sbr"
	-@erase "$(INTDIR)\AMViewerSplitter.obj"
	-@erase "$(INTDIR)\AMViewerSplitter.sbr"
	-@erase "$(INTDIR)\AMViewerView.obj"
	-@erase "$(INTDIR)\AMViewerView.sbr"
	-@erase "$(INTDIR)\Archive.obj"
	-@erase "$(INTDIR)\Archive.sbr"
	-@erase "$(INTDIR)\AV2.obj"
	-@erase "$(INTDIR)\AV2.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\Model.obj"
	-@erase "$(INTDIR)\Model.sbr"
	-@erase "$(INTDIR)\Mot.obj"
	-@erase "$(INTDIR)\Mot.sbr"
	-@erase "$(INTDIR)\Move.obj"
	-@erase "$(INTDIR)\Move.sbr"
	-@erase "$(INTDIR)\NoAccelEdit.obj"
	-@erase "$(INTDIR)\NoAccelEdit.sbr"
	-@erase "$(INTDIR)\Options.obj"
	-@erase "$(INTDIR)\Options.sbr"
	-@erase "$(INTDIR)\PatchModel.obj"
	-@erase "$(INTDIR)\PatchModel.sbr"
	-@erase "$(INTDIR)\Ply.obj"
	-@erase "$(INTDIR)\Ply.sbr"
	-@erase "$(INTDIR)\PolyModel.obj"
	-@erase "$(INTDIR)\PolyModel.sbr"
	-@erase "$(INTDIR)\Render.obj"
	-@erase "$(INTDIR)\Render.sbr"
	-@erase "$(INTDIR)\ScriptView.obj"
	-@erase "$(INTDIR)\ScriptView.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Time.obj"
	-@erase "$(INTDIR)\Time.sbr"
	-@erase "$(INTDIR)\Transform.obj"
	-@erase "$(INTDIR)\Transform.sbr"
	-@erase "$(INTDIR)\Turn.obj"
	-@erase "$(INTDIR)\Turn.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Zoom.obj"
	-@erase "$(INTDIR)\Zoom.sbr"
	-@erase "$(OUTDIR)\AMViewer.bsc"
	-@erase "$(OUTDIR)\AMViewer.pdb"
	-@erase "..\lib\pc\AMViewer.exe"
	-@erase "..\lib\pc\AMViewer.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AMViewer.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AMViewer.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AMViewer.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Action.sbr" \
	"$(INTDIR)\AMViewer.sbr" \
	"$(INTDIR)\AMViewerDoc.sbr" \
	"$(INTDIR)\AMViewerSplitter.sbr" \
	"$(INTDIR)\AMViewerView.sbr" \
	"$(INTDIR)\Archive.sbr" \
	"$(INTDIR)\AV2.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\Model.sbr" \
	"$(INTDIR)\Mot.sbr" \
	"$(INTDIR)\Move.sbr" \
	"$(INTDIR)\NoAccelEdit.sbr" \
	"$(INTDIR)\Options.sbr" \
	"$(INTDIR)\PatchModel.sbr" \
	"$(INTDIR)\Ply.sbr" \
	"$(INTDIR)\PolyModel.sbr" \
	"$(INTDIR)\Render.sbr" \
	"$(INTDIR)\ScriptView.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\Time.sbr" \
	"$(INTDIR)\Transform.sbr" \
	"$(INTDIR)\Turn.sbr" \
	"$(INTDIR)\Zoom.sbr"

"$(OUTDIR)\AMViewer.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\AMViewer.pdb" /debug /machine:I386 /out:"..\lib\pc\AMViewer.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Action.obj" \
	"$(INTDIR)\AMViewer.obj" \
	"$(INTDIR)\AMViewerDoc.obj" \
	"$(INTDIR)\AMViewerSplitter.obj" \
	"$(INTDIR)\AMViewerView.obj" \
	"$(INTDIR)\Archive.obj" \
	"$(INTDIR)\AV2.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\Model.obj" \
	"$(INTDIR)\Mot.obj" \
	"$(INTDIR)\Move.obj" \
	"$(INTDIR)\NoAccelEdit.obj" \
	"$(INTDIR)\Options.obj" \
	"$(INTDIR)\PatchModel.obj" \
	"$(INTDIR)\Ply.obj" \
	"$(INTDIR)\PolyModel.obj" \
	"$(INTDIR)\Render.obj" \
	"$(INTDIR)\ScriptView.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Time.obj" \
	"$(INTDIR)\Transform.obj" \
	"$(INTDIR)\Turn.obj" \
	"$(INTDIR)\Zoom.obj" \
	"$(INTDIR)\AMViewer.res" \
	"..\Lib\Pc\Bitmap.lib" \
	"..\Lib\Pc\FileIO.lib" \
	"..\Lib\Pc\Matrix.lib" \
	"..\Lib\Pc\Patch.lib" \
	"..\..\Program Files\Microsoft Visual Studio\Vc98\Lib\Opengl32.lib" \
	"..\..\Program Files\Microsoft Visual Studio\Vc98\Lib\Winmm.lib"

"..\lib\pc\AMViewer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AMViewer.dep")
!INCLUDE "AMViewer.dep"
!ELSE 
!MESSAGE Warning: cannot find "AMViewer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AMViewer - Win32 Release" || "$(CFG)" == "AMViewer - Win32 Debug"
SOURCE=.\Action.cpp

"$(INTDIR)\Action.obj"	"$(INTDIR)\Action.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\AMViewer.cpp

"$(INTDIR)\AMViewer.obj"	"$(INTDIR)\AMViewer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\AMViewer.rc

"$(INTDIR)\AMViewer.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\AMViewerDoc.cpp

"$(INTDIR)\AMViewerDoc.obj"	"$(INTDIR)\AMViewerDoc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\AMViewerSplitter.cpp

"$(INTDIR)\AMViewerSplitter.obj"	"$(INTDIR)\AMViewerSplitter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\AMViewerView.cpp

"$(INTDIR)\AMViewerView.obj"	"$(INTDIR)\AMViewerView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Archive.cpp

"$(INTDIR)\Archive.obj"	"$(INTDIR)\Archive.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\AV2.cpp

"$(INTDIR)\AV2.obj"	"$(INTDIR)\AV2.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\MainFrm.cpp

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\map.cpp

"$(INTDIR)\map.obj"	"$(INTDIR)\map.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Model.cpp

"$(INTDIR)\Model.obj"	"$(INTDIR)\Model.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Mot.cpp

"$(INTDIR)\Mot.obj"	"$(INTDIR)\Mot.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Move.cpp

"$(INTDIR)\Move.obj"	"$(INTDIR)\Move.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\NoAccelEdit.cpp

"$(INTDIR)\NoAccelEdit.obj"	"$(INTDIR)\NoAccelEdit.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Options.cpp

"$(INTDIR)\Options.obj"	"$(INTDIR)\Options.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\PatchModel.cpp

"$(INTDIR)\PatchModel.obj"	"$(INTDIR)\PatchModel.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Ply.cpp

"$(INTDIR)\Ply.obj"	"$(INTDIR)\Ply.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\PolyModel.cpp

"$(INTDIR)\PolyModel.obj"	"$(INTDIR)\PolyModel.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Render.cpp

"$(INTDIR)\Render.obj"	"$(INTDIR)\Render.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\ScriptView.cpp

"$(INTDIR)\ScriptView.obj"	"$(INTDIR)\ScriptView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "AMViewer - Win32 Release"

CPP_SWITCHES=/nologo /G5 /Zp1 /MD /W3 /GR /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AMViewer.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\AMViewer.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AMViewer - Win32 Debug"

CPP_SWITCHES=/nologo /Zp1 /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AMViewer.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\AMViewer.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Time.cpp

"$(INTDIR)\Time.obj"	"$(INTDIR)\Time.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Transform.cpp

"$(INTDIR)\Transform.obj"	"$(INTDIR)\Transform.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Turn.cpp

"$(INTDIR)\Turn.obj"	"$(INTDIR)\Turn.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"


SOURCE=.\Zoom.cpp

"$(INTDIR)\Zoom.obj"	"$(INTDIR)\Zoom.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AMViewer.pch"



!ENDIF 

