/**********************************************************************
 *<
	FILE: TupExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

/*
 *  $History: TupExport.cpp $
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Removed the Collapse meshes checkbox
 * Now evaluates the modifier stack and saves what it can and collapses
 * the rest
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 9:49a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Removed xref update options
 * Added exporter data & time as well as database version to export
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 6/07/02    Time: 9:48a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Removed output xrefs and modifiers
 * Changed to Collapse Meshes
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 5/09/02    Time: 10:22a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Moved to TupArray and TupString
 * Fixed bug with keepers and TupStrings (GetHashValue)
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 2:04p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Relative Path Support for Images
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 12/31/01   Time: 3:36p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Modifiers - pass 1
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs exporting
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs & derived objects (skeleton)
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 4:08p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Dialog Box working (doesn't do modifiers or xrefs yet)
 * Put transform back in (opps)
 * Export selected working
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:39p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Start of standard material exporting
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:25p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 3:08p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Use the loadsavemanager
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:50p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Full use of tupper3d for exporting
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Level A  complete
 * 
 * *****************  Version 3  *****************
 * User: Mkimball     Date: 11/30/01   Time: 2:21p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Some minimal stuff
 */

#include "TupExportPch.h"

#include "TupExport.h"


//--- TupExport -------------------------------------------------------
TupExport::TupExport() 
{
	m_outputASCII = TRUE;
}

TupExport::~TupExport() 
{
}

int TupExport::ExtCount() 
{
	return 1;
}

const TCHAR *TupExport::Ext(int n) 
{
	return "TUP";
}

const TCHAR *TupExport::LongDesc() 
{
	return "Tupperware File";
}
	
const TCHAR *TupExport::ShortDesc() 
{
	return "Tupperware";
}

const TCHAR *TupExport::AuthorName() 
{
	return "Avalanche Software";
}

const TCHAR *TupExport::CopyrightMessage() 
{
	return "Copyright 2001";
}

const TCHAR *TupExport::OtherMessage1() 
{
	return "";
}

const TCHAR *TupExport::OtherMessage2() 
{
	return "";
}

unsigned int TupExport::Version() 
{
	return 100;
}

void TupExport::ShowAbout(HWND wnd) 
{
}

BOOL TupExport::SupportsOptions(int ext, DWORD options) 
{
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

BOOL CALLBACK TupExportOptionsDlgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	TupExport *pExport = (TupExport*)GetWindowLongPtr(hWnd,GWLP_USERDATA); 
	switch (msg)
	{
		case WM_INITDIALOG:	 
			{
				pExport = (TupExport*)lParam;
				SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam); 
				CenterWindow(hWnd, GetParent(hWnd)); 

				CheckRadioButton(hWnd, IDC_OUTPUT_ASCII, IDC_OUTPUT_BINARY, 
					pExport->GetOutputASCII() ? IDC_OUTPUT_ASCII : IDC_OUTPUT_BINARY);
				char tempText[80];
				sprintf(tempText,"Export Date %s %s",g_pExporterDate,g_pExporterTime);
				SetWindowText(GetDlgItem(hWnd,IDC_EXPORTER_INFO),tempText);
				sprintf(tempText,"Database Version %s",TupScene::GetSceneVersion());
				SetWindowText(GetDlgItem(hWnd,IDC_DATABASE_INFO),tempText);
			}
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					pExport->SetOutputASCII(IsDlgButtonChecked(hWnd, IDC_OUTPUT_ASCII)); 
					EndDialog(hWnd, 1);
					break;

				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
			}
			break;

		default:
			return FALSE;
	}
	return TRUE;
}       


TupString TupExport::GetCfgFilename(void)
{
	TupString filename;
	
	filename += m_pInterface->GetDir(APP_PLUGCFG_DIR);
	filename += "\\";
	filename += CFGFILENAME;

	return filename;
}

// NOTE: Update anytime the CFG file changes
#define CFG_VERSION 0x04

BOOL TupExport::ReadConfig(void)
{
	TupString filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "rb");
	if (!cfgStream)
		return FALSE;

	// First item is a file version
	int fileVersion = _getw(cfgStream);

	if (fileVersion > CFG_VERSION) 
	{
		// Unknown version
		fclose(cfgStream);
		return FALSE;
	}
	// 0x01
	if (fileVersion == 0x01)
	{
		fgetc(cfgStream); // output xrefs (no longer used)
		fgetc(cfgStream); // output modifiers (no longer used)
		fgetc(cfgStream); // xref replace (no longer used)
		SetOutputASCII(fgetc(cfgStream));
	}
	// 0x02
	else if (fileVersion == 0x02)
	{
		fgetc(cfgStream); // collapse meshes (no longer used)
		fgetc(cfgStream); // xref replace (no longer used)
		SetOutputASCII(fgetc(cfgStream));
	}
	// 0x03
	else if (fileVersion == 0x03)
	{
		fgetc(cfgStream); // collapse meshes (no longer used)
		SetOutputASCII(fgetc(cfgStream));
	}
	// 0x04
	else if (fileVersion == CFG_VERSION)
	{
		SetOutputASCII(fgetc(cfgStream));
	}

	fclose(cfgStream);

	return TRUE;
}

void TupExport::WriteConfig(void)
{
	TupString filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "wb");
	if (!cfgStream)
		return;

	// Write CFG version
	_putw(CFG_VERSION,cfgStream);

	// 0x04
	fputc(GetOutputASCII(),cfgStream);

	fclose(cfgStream);
}

int	TupExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options) 
{
	m_pInterface = i;
	BOOL exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	ReadConfig();
	if(!suppressPrompts)
	{
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EXPORT_DIALOG), 
				m_pInterface->GetMAXHWnd(),  TupExportOptionsDlgProc, (LPARAM)this))
		{
			return TRUE;
		}
	}
	WriteConfig();

	return ExportScene(i->GetCurFilePath(),name,exportSelected);
}

