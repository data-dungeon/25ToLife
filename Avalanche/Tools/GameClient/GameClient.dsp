# Microsoft Developer Studio Project File - Name="GameClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GameClient - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GameClient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GameClient.mak" CFG="GameClient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GameClient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GameClient - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avalanche/tools/GameClient", NCYCAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GameClient - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "GameClient - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GameClient - Win32 Release"
# Name "GameClient - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Communicator.cpp
# End Source File
# Begin Source File

SOURCE=.\CommunicatorGameCube.cpp
# End Source File
# Begin Source File

SOURCE=.\CommunicatorPC.cpp
# End Source File
# Begin Source File

SOURCE=.\CommunicatorPS2.cpp
# End Source File
# Begin Source File

SOURCE=.\CommunicatorXBOX.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectingMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Connection.cpp
# End Source File
# Begin Source File

SOURCE=.\DirSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\FileServerConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\FileServerPane.cpp
# End Source File
# Begin Source File

SOURCE=.\GameClient.cpp
# End Source File
# Begin Source File

SOURCE=.\GameClient.odl
# End Source File
# Begin Source File

SOURCE=.\GameClient.rc
# End Source File
# Begin Source File

SOURCE=.\GameClientDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GameClientPane.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwarePane.cpp
# End Source File
# Begin Source File

SOURCE=.\OldFileServerConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputLogConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputLogPane.cpp
# End Source File
# Begin Source File

SOURCE=.\PS2TestPane.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Communicator.h
# End Source File
# Begin Source File

SOURCE=.\CommunicatorGameCube.h
# End Source File
# Begin Source File

SOURCE=.\CommunicatorPC.h
# End Source File
# Begin Source File

SOURCE=.\CommunicatorPS2.h
# End Source File
# Begin Source File

SOURCE=.\CommunicatorXBOX.h
# End Source File
# Begin Source File

SOURCE=.\ConnectingMessage.h
# End Source File
# Begin Source File

SOURCE=.\Connection.h
# End Source File
# Begin Source File

SOURCE=.\DirSelect.h
# End Source File
# Begin Source File

SOURCE=.\DlgProxy.h
# End Source File
# Begin Source File

SOURCE=.\FileServerConnection.h
# End Source File
# Begin Source File

SOURCE=.\FileServerPane.h
# End Source File
# Begin Source File

SOURCE=.\GameClient.h
# End Source File
# Begin Source File

SOURCE=.\GameClientDlg.h
# End Source File
# Begin Source File

SOURCE=.\GameClientPane.h
# End Source File
# Begin Source File

SOURCE=.\HardwarePane.h
# End Source File
# Begin Source File

SOURCE=.\OldFileServerConnection.h
# End Source File
# Begin Source File

SOURCE=.\OutputLogConnection.h
# End Source File
# Begin Source File

SOURCE=.\OutputLogPane.h
# End Source File
# Begin Source File

SOURCE=.\PS2TestPane.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\GameClient.ico
# End Source File
# Begin Source File

SOURCE=.\res\GameClient.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\GameClient.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
