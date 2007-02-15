# Microsoft Developer Studio Generated NMAKE File, Based on readtest.dsp
!IF "$(CFG)" == ""
CFG=readtest - Win32 Debug
!MESSAGE No configuration specified. Defaulting to readtest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "readtest - Win32 Release" && "$(CFG)" !=\
 "readtest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "readtest.mak" CFG="readtest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "readtest - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "readtest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "readtest - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\readtest.exe"

!ELSE 

ALL : "TextReadLIB - Win32 Release" "$(OUTDIR)\readtest.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"TextReadLIB - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\readtest.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\readtest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_MBCS" /Fp"$(INTDIR)\readtest.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\readtest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\readtest.pdb" /machine:I386 /out:"$(OUTDIR)\readtest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\readtest.obj"

"$(OUTDIR)\readtest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "readtest - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\readtest.exe"

!ELSE 

ALL : "TextReadLIB - Win32 Debug" "$(OUTDIR)\readtest.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"TextReadLIB - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\readtest.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\readtest.exe"
	-@erase "$(OUTDIR)\readtest.ilk"
	-@erase "$(OUTDIR)\readtest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "\tools\include" /D "WIN32" /D\
 "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\readtest.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\readtest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\readtest.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\readtest.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\readtest.obj"

"$(OUTDIR)\readtest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "readtest - Win32 Release" || "$(CFG)" ==\
 "readtest - Win32 Debug"

!IF  "$(CFG)" == "readtest - Win32 Release"

"TextReadLIB - Win32 Release" : 
   cd "\tools\textread.lib"
   $(MAKE) /$(MAKEFLAGS) /F .\TextReadLIB.mak CFG="TextReadLIB - Win32 Release"\
 
   cd "..\readtest"

"TextReadLIB - Win32 ReleaseCLEAN" : 
   cd "\tools\textread.lib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\TextReadLIB.mak\
 CFG="TextReadLIB - Win32 Release" RECURSE=1 
   cd "..\readtest"

!ELSEIF  "$(CFG)" == "readtest - Win32 Debug"

"TextReadLIB - Win32 Debug" : 
   cd "\tools\textread.lib"
   $(MAKE) /$(MAKEFLAGS) /F .\TextReadLIB.mak CFG="TextReadLIB - Win32 Debug" 
   cd "..\readtest"

"TextReadLIB - Win32 DebugCLEAN" : 
   cd "\tools\textread.lib"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\TextReadLIB.mak\
 CFG="TextReadLIB - Win32 Debug" RECURSE=1 
   cd "..\readtest"

!ENDIF 

SOURCE=.\readtest.cpp

!IF  "$(CFG)" == "readtest - Win32 Release"


"$(INTDIR)\readtest.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "readtest - Win32 Debug"

DEP_CPP_READT=\
	"..\include\read_tga.h"\
	"..\include\readbase.h"\
	"..\include\str_wrap.h"\
	"..\include\t_struct.h"\
	"..\include\t_types.h"\
	

"$(INTDIR)\readtest.obj" : $(SOURCE) $(DEP_CPP_READT) "$(INTDIR)"


!ENDIF 


!ENDIF 

