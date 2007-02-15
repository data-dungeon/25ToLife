# Microsoft Developer Studio Project File - Name="mdllib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mdllib - Win32 Max Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mdllib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mdllib.mak" CFG="mdllib - Win32 Max Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mdllib - Win32 Max Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE "mdllib - Win32 Maya Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mdllib - Win32 Max Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mdllib - Win32 Maya Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mdllib - Win32 Max Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/Plugins/Libraries/mdllib", XCSAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mdllib - Win32 Max Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mdllib___Win32_Max_Hybrid"
# PROP BASE Intermediate_Dir "mdllib___Win32_Max_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Hybrid"
# PROP Intermediate_Dir "Max_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mdllib - Win32 Maya Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mdllib___Win32_Maya_Debug"
# PROP BASE Intermediate_Dir "mdllib___Win32_Maya_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Maya_Debug"
# PROP Intermediate_Dir "Maya_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /I "$(MAYAPATH)\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\.." /I "$(MAYAPATH)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mdllib - Win32 Max Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mdllib___Win32_Max_Debug"
# PROP BASE Intermediate_Dir "mdllib___Win32_Max_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Max_Debug"
# PROP Intermediate_Dir "Max_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /I "$(MAYAPATH)\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "$(MAYAPATH)\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mdllib - Win32 Maya Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mdllib___Win32_Maya_Release"
# PROP BASE Intermediate_Dir "mdllib___Win32_Maya_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Maya_Release"
# PROP Intermediate_Dir "Maya_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mdllib - Win32 Max Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mdllib___Win32_Max_Release"
# PROP BASE Intermediate_Dir "mdllib___Win32_Max_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Max_Release"
# PROP Intermediate_Dir "Max_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "mdllib - Win32 Max Hybrid"
# Name "mdllib - Win32 Maya Debug"
# Name "mdllib - Win32 Max Debug"
# Name "mdllib - Win32 Maya Release"
# Name "mdllib - Win32 Max Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\harg.cpp
# End Source File
# Begin Source File

SOURCE=.\hattr.cpp
# End Source File
# Begin Source File

SOURCE=.\hbias.cpp
# End Source File
# Begin Source File

SOURCE=.\hbone.cpp
# End Source File
# Begin Source File

SOURCE=.\hbonearray.cpp
# End Source File
# Begin Source File

SOURCE=.\hbonechain.cpp
# End Source File
# Begin Source File

SOURCE=.\hbonechainarray.cpp
# End Source File
# Begin Source File

SOURCE=.\hcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\hcp.cpp
# End Source File
# Begin Source File

SOURCE=.\hdecal.cpp
# End Source File
# Begin Source File

SOURCE=.\hedge.cpp
# End Source File
# Begin Source File

SOURCE=.\hfileio.cpp
# End Source File
# Begin Source File

SOURCE=.\hfiletools.cpp
# End Source File
# Begin Source File

SOURCE=.\hfiletools2.cpp
# End Source File
# Begin Source File

SOURCE=.\hgroup.cpp
# End Source File
# Begin Source File

SOURCE=.\hhook.cpp
# End Source File
# Begin Source File

SOURCE=.\himage.cpp
# End Source File
# Begin Source File

SOURCE=.\hintarray.cpp
# End Source File
# Begin Source File

SOURCE=.\hintmap.cpp
# End Source File
# Begin Source File

SOURCE=.\hmap.cpp
# End Source File
# Begin Source File

SOURCE=.\hmatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\hneighbor.cpp
# End Source File
# Begin Source File

SOURCE=.\hpatch.cpp
# End Source File
# Begin Source File

SOURCE=.\hpatcharray.cpp
# End Source File
# Begin Source File

SOURCE=.\hpatchvectors.cpp
# End Source File
# Begin Source File

SOURCE=.\hpatchvectors2d.cpp
# End Source File
# Begin Source File

SOURCE=.\hpoly.cpp
# End Source File
# Begin Source File

SOURCE=.\hpolyarray.cpp
# End Source File
# Begin Source File

SOURCE=.\hshader.cpp
# End Source File
# Begin Source File

SOURCE=.\hspline.cpp
# End Source File
# Begin Source File

SOURCE=.\hsplinearray.cpp
# End Source File
# Begin Source File

SOURCE=.\hstamp.cpp
# End Source File
# Begin Source File

SOURCE=.\hstatus.cpp
# End Source File
# Begin Source File

SOURCE=.\htesscorner.cpp
# End Source File
# Begin Source File

SOURCE=.\htessedge.cpp
# End Source File
# Begin Source File

SOURCE=.\htjunction.cpp
# End Source File
# Begin Source File

SOURCE=.\hvectormap.cpp
# End Source File
# Begin Source File

SOURCE=.\logfile.cpp
# End Source File
# Begin Source File

SOURCE=.\mdlobject.cpp
# End Source File
# Begin Source File

SOURCE=.\mdlobjectarray.cpp
# End Source File
# Begin Source File

SOURCE=.\uberstring.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\harg.h
# End Source File
# Begin Source File

SOURCE=.\hattr.h
# End Source File
# Begin Source File

SOURCE=.\hbias.h
# End Source File
# Begin Source File

SOURCE=.\hbone.h
# End Source File
# Begin Source File

SOURCE=.\hbonearray.h
# End Source File
# Begin Source File

SOURCE=.\hbonechain.h
# End Source File
# Begin Source File

SOURCE=.\hbonechainarray.h
# End Source File
# Begin Source File

SOURCE=.\hcolor.h
# End Source File
# Begin Source File

SOURCE=.\hcp.h
# End Source File
# Begin Source File

SOURCE=.\hdecal.h
# End Source File
# Begin Source File

SOURCE=.\hdoublearray.h
# End Source File
# Begin Source File

SOURCE=.\hdoublearray.hpp
# End Source File
# Begin Source File

SOURCE=.\hdynamicproperties.h
# End Source File
# Begin Source File

SOURCE=.\hedge.h
# End Source File
# Begin Source File

SOURCE=.\hfiletools.h
# End Source File
# Begin Source File

SOURCE=.\hfiletools2.h
# End Source File
# Begin Source File

SOURCE=.\hgroup.h
# End Source File
# Begin Source File

SOURCE=.\hhook.h
# End Source File
# Begin Source File

SOURCE=.\himage.h
# End Source File
# Begin Source File

SOURCE=.\hintarray.h
# End Source File
# Begin Source File

SOURCE=.\hintarray.hpp
# End Source File
# Begin Source File

SOURCE=.\hintmap.h
# End Source File
# Begin Source File

SOURCE=.\hintmap.hpp
# End Source File
# Begin Source File

SOURCE=.\hmacros.h
# End Source File
# Begin Source File

SOURCE=.\hmap.h
# End Source File
# Begin Source File

SOURCE=.\hmatrix.h
# End Source File
# Begin Source File

SOURCE=.\hmatrix44.h
# End Source File
# Begin Source File

SOURCE=.\hmatrix44.hpp
# End Source File
# Begin Source File

SOURCE=.\hneighbor.h
# End Source File
# Begin Source File

SOURCE=.\hpatch.h
# End Source File
# Begin Source File

SOURCE=.\hpatch.hpp
# End Source File
# Begin Source File

SOURCE=.\hpatcharray.h
# End Source File
# Begin Source File

SOURCE=.\hpatchvectors.h
# End Source File
# Begin Source File

SOURCE=.\hpatchvectors2d.h
# End Source File
# Begin Source File

SOURCE=.\hpoly.h
# End Source File
# Begin Source File

SOURCE=.\hpolyarray.h
# End Source File
# Begin Source File

SOURCE=.\hquaternion.h
# End Source File
# Begin Source File

SOURCE=.\hquaternion.hpp
# End Source File
# Begin Source File

SOURCE=.\hshader.h
# End Source File
# Begin Source File

SOURCE=.\hspline.h
# End Source File
# Begin Source File

SOURCE=.\hspline.hpp
# End Source File
# Begin Source File

SOURCE=.\hsplinearray.h
# End Source File
# Begin Source File

SOURCE=.\hsplinearray.hpp
# End Source File
# Begin Source File

SOURCE=.\hsplinesegement.hpp
# End Source File
# Begin Source File

SOURCE=.\hsplinesegment.h
# End Source File
# Begin Source File

SOURCE=.\hstamp.h
# End Source File
# Begin Source File

SOURCE=.\hstatus.h
# End Source File
# Begin Source File

SOURCE=.\htesscorner.h
# End Source File
# Begin Source File

SOURCE=.\htessedge.h
# End Source File
# Begin Source File

SOURCE=.\htjunction.h
# End Source File
# Begin Source File

SOURCE=.\huserproperties.h
# End Source File
# Begin Source File

SOURCE=.\hvector.h
# End Source File
# Begin Source File

SOURCE=.\hvector.hpp
# End Source File
# Begin Source File

SOURCE=.\hvector2d.h
# End Source File
# Begin Source File

SOURCE=.\hvector2d.hpp
# End Source File
# Begin Source File

SOURCE=.\hvectormap.h
# End Source File
# Begin Source File

SOURCE=.\logfile.h
# End Source File
# Begin Source File

SOURCE=.\mdlobject.h
# End Source File
# Begin Source File

SOURCE=.\mdlobjectarray.h
# End Source File
# Begin Source File

SOURCE=.\uberstring.h
# End Source File
# Begin Source File

SOURCE=.\uiflags.h
# End Source File
# End Group
# End Target
# End Project
