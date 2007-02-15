/**********************************************************************
 *<
	FILE: TupImport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

/*
 *  $History: TupImport.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */

#include "TupImportPch.h"

#include "TupImport.h"


//--- TupImport -------------------------------------------------------
TupImport::TupImport() 
{
	m_outputASCII = TRUE;
}

TupImport::~TupImport() 
{
}

int TupImport::ExtCount() 
{
	return 1;
}

const TCHAR *TupImport::Ext(int n) 
{
	return "TUP";
}

const TCHAR *TupImport::LongDesc() 
{
	return "Tupperware File";
}
	
const TCHAR *TupImport::ShortDesc() 
{
	return "Tupperware";
}

const TCHAR *TupImport::AuthorName() 
{
	return "Avalanche Software";
}

const TCHAR *TupImport::CopyrightMessage() 
{
	return "Copyright 2001";
}

const TCHAR *TupImport::OtherMessage1() 
{
	return "";
}

const TCHAR *TupImport::OtherMessage2() 
{
	return "";
}

unsigned int TupImport::Version() 
{
	return 100;
}

void TupImport::ShowAbout(HWND wnd) 
{
}

BOOL TupImport::SupportsOptions(int ext, DWORD options) 
{
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

BOOL CALLBACK TupImportOptionsDlgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	TupImport *pImport = (TupImport*)GetWindowLongPtr(hWnd,GWLP_USERDATA); 
	switch (msg)
	{
		case WM_INITDIALOG:	 
			{
				pImport = (TupImport*)lParam;
				SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam); 
				CenterWindow(hWnd, GetParent(hWnd)); 

				CheckRadioButton(hWnd, IDC_OUTPUT_ASCII, IDC_OUTPUT_BINARY, 
					pImport->GetOutputASCII() ? IDC_OUTPUT_ASCII : IDC_OUTPUT_BINARY);
				char tempText[80];
				sprintf(tempText,"Import Date %s %s",g_pImporterDate,g_pImporterTime);
				SetWindowText(GetDlgItem(hWnd,IDC_EXPORTER_INFO),tempText);
				sprintf(tempText,"Database Version %s",TupScene::GetSceneVersion());
				SetWindowText(GetDlgItem(hWnd,IDC_DATABASE_INFO),tempText);
			}
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					pImport->SetOutputASCII(IsDlgButtonChecked(hWnd, IDC_OUTPUT_ASCII)); 
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


TupString TupImport::GetCfgFilename(void)
{
	TupString filename;
	
	filename += m_pInterface->GetDir(APP_PLUGCFG_DIR);
	filename += "\\";
	filename += CFGFILENAME;

	return filename;
}

// NOTE: Update anytime the CFG file changes
#define CFG_VERSION 0x04

BOOL TupImport::ReadConfig(void)
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

void TupImport::WriteConfig(void)
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

int	TupImport::DoImport(const TCHAR *name,ImpInterface *ii,Interface *i, BOOL suppressPrompts) 
{
	m_pInterface = i;

	ReadConfig();
#if 0
	if(!suppressPrompts)
	{
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_IMPORT_DIALOG), 
				m_pInterface->GetMAXHWnd(),  TupImportOptionsDlgProc, (LPARAM)this))
		{
			return TRUE;
		}
	}
#endif
	WriteConfig();

	return ImportScene(name);
}

