#pragma once

#include <Afxtempl.h>
#include <stdarg.h>

#include "resource.h"

class WMD
{
public:
	WMD(const char *xlFile, const char *outputDir);
	~WMD();

	// Did it work (0 means cool)
	int ExitCode()
		{ return m_errorExit; }

private:
	// Our current error status
	int	m_errorExit;

	// Open the file (check for errors!)
	bool Open();

	// Write the output
	bool Convert();

	// Our intput parameters
	CString			m_xlFile;
	CString			m_outputDir;

	// We need a list of sheet names
	CStringArray	m_sheetNames;
	bool BuildSheetNames();

	// We have to build a translation table for each sheet
	static const char *m_varMapSheet;
	class VarMap
	{
	public:
			VarMap() { m_onOutputFile = 0; }
			~VarMap() {}

			CString			m_varFile;
			CStringArray	m_sheetName;
			int				m_onOutputFile;
	};
	VarMap				*m_varMap;
	int					m_varMaps;
	bool BuildVarMap();
	bool GetVarFile(const CString &sheetName, int onRow, int rows, CString &varFile);
	
	// The almighty write
	bool WriteVar(const CString &sheetName, const CString &varFile, const CStringArray &headerRow, const CStringArray &dataRow);
	
	// Is this CString an int	
	bool IsInt(const CString &value, int &i);

	// Print a warning
	void DebugLog(const char *format, ...);
	void Log(const char *format, ...);
	void Warning(const char *format, ...);

	// Always returns false
	bool Error(const char *format, ...);
};
