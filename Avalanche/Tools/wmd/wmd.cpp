///////////////////////////////////////////////////////////////////////////////
// Weapon eXcel converter
///////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdio.h>
#include <afxdao.h>

#include "stdafx.h"
#include "wmd.h"
#include "CSpreadSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char *WMD::m_varMapSheet = "varmap";

///////////////////////////////////////////////////////////////////////////////
// The one and only application object
///////////////////////////////////////////////////////////////////////////////
CWinApp theApp;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Weee
///////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	printf("WMD 1.1, by Food\n");

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		return 1;
	}

	// Ooops
	if (argc < 2)
	{
 		printf("Usage: WMD <weapon.xls> <output dir>\n\n");
		exit(2);
	}

	// Setup the parameters & do it!
	WMD wmd(argv[1], argv[2]);
	return wmd.ExitCode();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WMD::WMD(
const char *xlFile,
const char *outputDir)
{
	// No errors yet
	m_errorExit = 0;

	// Just save it off for now
	m_xlFile = xlFile;
	m_outputDir = outputDir;
	if (m_outputDir[m_outputDir.GetLength() - 1] != '\\')
		m_outputDir += "\\";

	m_varMap = NULL;
	m_varMaps = 0;

	// Open everything
	if (Open())
		Convert();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WMD::~WMD()
{
	// Clear the var map
	delete [] m_varMap;

	// Why do I have to call this?
	AfxDaoTerm();
}

///////////////////////////////////////////////////////////////////////////////
// Open the file (check for errors!)
///////////////////////////////////////////////////////////////////////////////
bool WMD::Open()
{
	// Wee
	if (!BuildSheetNames())
		return Error("Unable to build list of sheet names.");

	// Build the output file names
	if (!BuildVarMap())
		return Error("Unable to build category names.");

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Write the output
///////////////////////////////////////////////////////////////////////////////
bool WMD::Convert()
{
	// For each sheet
	int sheets = m_sheetNames.GetCount();
	for (int sheet = 0; sheet < sheets; sheet++)
	{
		// Is this the special sheet?
		if (m_sheetNames[sheet].CompareNoCase(m_varMapSheet) == 0)
			continue;

		// Open the sheet
		CSpreadSheet worksheet(m_xlFile, m_sheetNames[sheet], false);

		// How many rows are there
		int rows = worksheet.GetTotalRows();

		DebugLog("Scanning sheet \"%s\" with %d rows\n", (const char *)m_sheetNames[sheet], rows);

		// Can't make output for less than two rows
		if (rows < 2)		
		{
			Warning("Sheet \"%s\" has too little data for output.\n", (const char *)m_sheetNames[sheet]);
			continue;
		}
	
		// read the header row
		CStringArray headerRow;
		bool ret = worksheet.ReadRow(headerRow);
		if (!ret)
			Error("Cannot process header row in \"%s\"\n", (const char *)m_sheetNames[sheet]);	
		else
		{
			// Clean up the header row
			int headers = headerRow.GetCount();
			for (int h = 0; h < headers; h++)
			{
				CString &hdr = headerRow[h];
				hdr.TrimLeft(" \t");
				hdr.TrimRight(" \t");
			}

			// Convert it
			for (int row = 1; row < rows; row++)
			{
				// Make the output filename
				CString varFile;
				if (!GetVarFile(m_sheetNames[sheet], row, rows, varFile))
					Warning("Could not generate output filename for \"%s\"\n", (const char *)m_sheetNames[sheet]);
				else
				{
					CStringArray dataRow;
					bool ret = worksheet.ReadRow(dataRow);
					if (!ret)
						Error("Cannot process data row %d in \"%s\"\n", row, (const char *)m_sheetNames[sheet]);	
					else
						WriteVar(m_sheetNames[sheet], varFile, headerRow, dataRow);
				}
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WMD::BuildSheetNames()
{
	// use dao to get a list of excel worksheet names
	CDaoDatabase dbExcel;
	dbExcel.Open(m_xlFile, FALSE, TRUE, "Excel 8.0;" );
	int tables = dbExcel.GetTableDefCount();
	if (tables < 0)
		return false;

	// Log!
	DebugLog("Discovering sheet names from \"%s\"\n", (const char *)m_xlFile);

	m_sheetNames.RemoveAll();
	for (int i = 0; i < tables; i++)
	{
		CDaoTableDefInfo tableInfo;
		dbExcel.GetTableDefInfo(i, tableInfo);

		// remove ' and $ chars that are contained in the table string
		CString tableName = tableInfo.m_strName;
		tableName.Remove( '\'' );
		tableName.Remove( '$' );
		m_sheetNames.Add(tableName);

		DebugLog("Got sheet \"%s\"\n", (const char *)tableName);
	}

	dbExcel.Close();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WMD::BuildVarMap()
{
	// Hello, how do we see errors here?
	CSpreadSheet sheet(m_xlFile, m_varMapSheet, false);

	// How many columns do we have?
	int totalColumns = sheet.GetTotalColumns();
	if (totalColumns <= 0)
		return true;
	
	DebugLog("Discovering var file mappings from \"%s\"\n", (const char *)m_varMapSheet);

	// Allocate the array
	m_varMap = new VarMap[totalColumns];
	m_varMaps = totalColumns;

	// Fill it
	for (int column = 0; column < totalColumns; column++)
	{
		CStringArray columnData;
		bool ret = sheet.ReadColumn(columnData, column + 1);
		if (!ret)
			Warning("Cannot process column %d in %s\n", column, m_varMapSheet);
		else if (columnData.GetSize() > 0)
		{
			m_varMap[column].m_varFile = columnData.GetAt(0);
			DebugLog("Varfile \"%s\" mapped to: ", (const char *)m_varMap[column].m_varFile);

			int cells = columnData.GetSize();
			for (int cell = 1; cell < cells; cell++)
			{
				CString cellData = columnData.GetAt(cell);
				m_varMap[column].m_sheetName.Add(cellData);
				DebugLog("\"%s\" ", (const char *)cellData);
			}
			DebugLog("\n");
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WMD::GetVarFile(
const CString &sheetName, 
int onRow, 
int rows,
CString &varName)
{
	// Find the right entry
	for (int v = 0; v < m_varMaps; v++)
	{	
		int sheets = m_varMap[v].m_sheetName.GetCount();
		for (int s = 0; s < sheets; s++)
		{
			if (m_varMap[v].m_sheetName[s].CompareNoCase(sheetName) == 0)
			{
				varName.Format("%s%s%d.var", m_outputDir, m_varMap[v].m_varFile, m_varMap[v].m_onOutputFile++);  
				return true;
			}
		}
	}

	// We did not find it?
	if (rows == 2)
		varName = m_outputDir + sheetName + ".var";
	else
		varName.Format("%s%s%d.var", m_outputDir, sheetName, onRow - 1);

	return true;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WMD::WriteVar(
const CString &sheetName,
const CString &varFile,
const CStringArray &headerRow,
const CStringArray &dataRow)
{
	FILE *fh = fopen((const char *)varFile, "wt");
	if (!fh)
		return Error("Cannot open \"%s\" for output.\n", (const char *)varFile);

	// Log
	Log("Writing \"%s\" from sheet \"%s\"\n", (const char *)varFile, (const char *)sheetName);

	// A nice header
	fprintf(fh, "# WMD Generated file from sheet %s in %s\n\n", (const char *)sheetName, (const char *)m_xlFile);

	// Walk the data
	int vars = headerRow.GetCount();
	for (int v = 0; v < vars; v++)
	{
		CString value = dataRow[v];
		value.TrimLeft("\t ");
		value.TrimRight("\t ");

		if (strnicmp((const char *)headerRow[v], "blank", 5) == 0)
			fprintf(fh, "\n");
		else if (value.GetLength() > 0)
		{	
			int i;
			if (IsInt(value, i))
				fprintf(fh, "%s %d\n", (const char *)headerRow[v], i);
			else
			{
				value.Replace("\\", "/");
				fprintf(fh, "%s %s\n", (const char *)headerRow[v], (const char *)value);
			}
		}
	}
	fprintf(fh, "\n");

	fclose(fh);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Is this CString an int	
///////////////////////////////////////////////////////////////////////////////
bool WMD::IsInt(
const CString &value, 
int &i)
{
	// First scan to make sure it is all legal
	for (int c = 0; c < value.GetLength(); c++)
	{
		char ch = value[c];
		if (ch != '-' && ch != '+' && ch != '.' && (ch < '0' || ch > '9'))
			return false;
	}
	
	// okay, it looks legal
	double floatValue = atof((const char *)value);
	i = (int)floatValue;

	float d = floatValue - (double)i;
	return (fabs(d) < 1e-6);
}

///////////////////////////////////////////////////////////////////////////////
// Log!
///////////////////////////////////////////////////////////////////////////////
void WMD::DebugLog(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
#ifdef _DEBUG
	vprintf(format, args);
#endif
	va_end(args);
}

///////////////////////////////////////////////////////////////////////////////
// Log!
///////////////////////////////////////////////////////////////////////////////
void WMD::Log(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

///////////////////////////////////////////////////////////////////////////////
// Error handling
///////////////////////////////////////////////////////////////////////////////
void WMD::Warning(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

///////////////////////////////////////////////////////////////////////////////
// Always returns false
///////////////////////////////////////////////////////////////////////////////
bool WMD::Error(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);

	m_errorExit = 1;
	return false;
}
