//------------------------------------------------------------------------------
// UpdaterCommandLine.cpp
//
// Command line parser for WinUpdate
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "UpdaterCommandLine.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Our command line comes in the form:
//
// WinUpdate parentProcessID versionHost versionPath patchDir gameToLaunch launchParameters
//
// where:
//
// parentProcessID	-- The process ID of the launching process, which needs to be active
// versionHost		-- The host name from which to get the version info
// versionPath		-- The path of the version description file on the host
// patchDir			-- The directory in which we are to apply the patch.
// gameToLaunch		-- The game that should be launched on success
// launchParameters	-- Command line parameters for game to launch
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// We set the parameters in the order above with no checking. Since this command
// line is only permitted from tested calls to CreateProcess, we don't need to
// handle user error. 
void UpdaterCommandLine::ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast)
{
	if (!m_idSet) {
		m_parentProcessID = (DWORD)atoi(pszParam);
		m_idSet	= true;
	}
	else if (m_versionHost.size() == 0)
		m_versionHost = pszParam;
	else if (m_versionPath.size() == 0)
		m_versionPath = pszParam;
	else if (m_dirToPatch.size() == 0)
		m_dirToPatch = pszParam;
	else if (m_gameToLaunch.size() == 0)
		m_gameToLaunch = pszParam;
	else if (m_launchParameters.size() == 0)
		m_launchParameters = pszParam;
	else
		m_launchParameters = m_launchParameters + " " + pszParam;
}
