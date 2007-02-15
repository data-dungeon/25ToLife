//------------------------------------------------------------------------------
// UpdaterCommandLine.h
//
// Command line parser for WinUpdate
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#pragma once
#include "afxcmn.h"

class UpdaterCommandLine : public CCommandLineInfo {
public:
	UpdaterCommandLine() : m_idSet(false) {}
	virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast);

	DWORD		m_parentProcessID;
	std::string m_versionHost;
	std::string m_versionPath;
	std::string m_dirToPatch;
	std::string m_gameToLaunch;
	std::string m_launchParameters;
	bool		m_idSet;
};
