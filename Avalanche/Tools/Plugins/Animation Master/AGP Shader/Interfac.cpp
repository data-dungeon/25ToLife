// KB  6/14/00  \Ken85\Plugin\Shaders\Gradient\Interfac.cpp

#include "stdafx.h"
#include "AGPShader.h"
#include "HashHand.h"
#include "ColorBuf.h"
#include "resource.h"

#define UNIT   100.0f

//Setting the two window handles as globals so I can tell which one we're working with in the OnCommand procedure.
HWND dlg1hwnd;
HWND dlg2hwnd;

BOOL AGPShader::OnCommand( HWND hwnddlg, UINT id )
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDialog *dialog = (CDialog *)CWnd::FromHandle( hwnddlg );
	CFileDialog	filedlg (TRUE, "*", "*.*", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "All Files (*.*)|*.*|Windows Bitmap(*.bmp)|*.bmp||", NULL);


		switch (id) {
			case IDC_COMBO_PROJECT:
				m_project=SendDlgItemMessage(hwnddlg,IDC_COMBO_PROJECT,CB_GETCURSEL,0,0L);
				dialog->GetDlgItemText(IDC_COMBO_PROJECT,m_projecttext,100);
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE, FALSE);
				return TRUE;
			case IDC_COMBO_COLLISION:
				m_collision=SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_GETCURSEL,0,0L);
				dialog->GetDlgItemText(IDC_COMBO_COLLISION,m_collisiontext,100);
				return TRUE;
			case IDC_COMBO_TERRAIN:
				m_terrain=SendDlgItemMessage(hwnddlg,IDC_COMBO_TERRAIN,CB_GETCURSEL,0,0L);
				dialog->GetDlgItemText(IDC_COMBO_TERRAIN,m_terraintext,100);
				return TRUE;
			case IDC_CHECK_FLAT_2:
				m_flat = dialog->IsDlgButtonChecked(IDC_CHECK_FLAT_2);
				return TRUE;
			case IDC_CHECK_DNCLIP_2:
				m_dnclip = dialog->IsDlgButtonChecked(IDC_CHECK_DNCLIP_2);
				return TRUE;
			case IDC_CHECK_DNFOG:
				m_dnfog = dialog->IsDlgButtonChecked(IDC_CHECK_DNFOG);
				return TRUE;
			case IDC_CHECK_TWOSIDED:
				m_twosided = dialog->IsDlgButtonChecked(IDC_CHECK_TWOSIDED);
				return TRUE;
			case IDC_CHECK_ZBIAS:
				m_zbias = dialog->IsDlgButtonChecked(IDC_CHECK_ZBIAS);
				return TRUE;
			case IDC_CHECK_LOD:
				m_lod = dialog->IsDlgButtonChecked(IDC_CHECK_LOD);
				return TRUE;
			case IDC_CHECK_DELETE:
				m_deletepatch = dialog->IsDlgButtonChecked(IDC_CHECK_DELETE);
				return TRUE;
			case IDC_CHECK_INVISIBLE:
				m_invisible = dialog->IsDlgButtonChecked(IDC_CHECK_INVISIBLE);
				return TRUE;	
			case IDC_CHECK_COLOROVERRIDE:
				m_coloroverride = dialog->IsDlgButtonChecked(IDC_CHECK_COLOROVERRIDE);
				return TRUE;
			case IDC_CHECK_32BITVC:
				m_32bitvc = dialog->IsDlgButtonChecked(IDC_CHECK_32BITVC);
				return TRUE;
			case IDC_EDIT_LAYER:
				m_layer=dialog->GetDlgItemInt(IDC_EDIT_LAYER,NULL,TRUE);
				if(m_layer<0)
					m_layer=0;
				if(m_layer>7)
					m_layer=7;
			     RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE, FALSE);
				return TRUE;	
		}
		switch (id) {
			case IDC_CHECK_ENVIRON_MAP:
				m_environmap = dialog->IsDlgButtonChecked(IDC_CHECK_ENVIRON_MAP);
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;
			case IDC_CHECK_PRIVATE_FIX:
				m_private = dialog->IsDlgButtonChecked(IDC_CHECK_PRIVATE_FIX);
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;	
			case IDC_CHECK_ALPHA:
				m_alpha = dialog->IsDlgButtonChecked(IDC_CHECK_ALPHA);
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;
			case IDC_RADIO_TESS_NONE:
				if (dialog->IsDlgButtonChecked(IDC_RADIO_TESS_NONE))
					m_tessadj=0;
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;
			case IDC_RADIO_TESS_ABS:
				if (dialog->IsDlgButtonChecked(IDC_RADIO_TESS_ABS))
					m_tessadj=1;
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;
			case IDC_RADIO_TESS_MUL:
				if (dialog->IsDlgButtonChecked(IDC_RADIO_TESS_MUL))
					m_tessadj=2;
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;
			case IDC_EDIT_MAP_PERCENT:
				m_environmentmappercent=dialog->GetDlgItemInt(IDC_EDIT_MAP_PERCENT,NULL,TRUE);
				if(m_environmentmappercent<0)
					m_environmentmappercent=0;
				if(m_environmentmappercent>100)
					m_environmentmappercent=100;
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;
			case IDC_EDIT_PRETESS:
				char buf[100];
				//This edit box allows floats, so get as text then convert.
				dialog->GetDlgItemText(IDC_EDIT_PRETESS,buf,100);
				m_pretess=(float)atof(buf);
				//m_pretess=dialog->GetDlgItemInt(IDC_EDIT_PRETESS,NULL,TRUE);
				if(m_pretess<0.0f)
					m_pretess=0.0f;
				if(m_pretess>10000.0f)
					m_pretess=10000.0f;
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;
			case IDC_BUTTON_BROWSE:
				if(filedlg.DoModal()==IDOK)
				{
					strcpy(m_environmappath,(LPCTSTR)filedlg.GetPathName());
				};
				RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE2, FALSE);
				return TRUE;
		}
	return FALSE;
}

void AGPShader::InitPropertyPage( HWND hwnddlg, UINT dialogid, BOOL instance )
{
	FILE *fp;
	bool bMatch=false;

	HKEY			hKey;
	char			path[_MAX_PATH];
	unsigned long	iSize = _MAX_PATH;
	long			ret;
	unsigned long	dword;


	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Hash, Inc.\\Animation Master 2000 V8.5",0,KEY_READ,&hKey);
	if (ret!=ERROR_SUCCESS)
	{
		MessageBox(NULL,"Animation Master Information Not In Registry (v8.5)", "ERROR", 1);
	} else
	{
		ret=RegQueryValueEx(hKey,"InstallationDir",NULL,&dword,(unsigned char *)path,&iSize);
		if (ret!=ERROR_SUCCESS)
		{
			MessageBox(NULL,"Hash Installation Path Not Found in the Registry", "ERROR", 1);
		}
	}

	strcat(path,"\\shaders\\terraintypes.txt");
	if((fp=fopen(path,"rb"))==NULL)
	{
		MessageBox(hwnddlg,"Unable to open file:","File Access Error",1);
		MessageBox(hwnddlg,path,"File Access Error",1);
	} else
	{	
		CDialog *dialog = (CDialog *)CWnd::FromHandle(hwnddlg);

		if (dialogid == IDD_ATTRIBUTES_PAGE) {
			do{
				char buf[250];
				fgets(buf,250,fp);
				//Strip off CR/LF
				for (unsigned int i=0; i<strlen(buf); i++)
					if (buf[i]<16)
						buf[i]=0;

				if (buf[0]!='[')
				{
					SendDlgItemMessage(hwnddlg,IDC_COMBO_TERRAIN,CB_ADDSTRING,0,(LPARAM)buf);
				} else
				{
					SendDlgItemMessage(hwnddlg,IDC_COMBO_PROJECT,CB_ADDSTRING,0,(LPARAM)buf);
				}
			}while(!feof(fp));
			fclose(fp);
			
			//Populate the Collision Combo Box
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"None");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"Geometry");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"OBB #1");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"OBB #2");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"OBB #3");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"OBB #4");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"OBB #5");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"OBB #6");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"OBB #7");
			SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_ADDSTRING,0,(LPARAM)"OBB #8");
			//Populate the Terrain Combo Box
		}
	}
	RefreshPropertyPage(hwnddlg, IDD_ATTRIBUTES_PAGE, FALSE);
}

void AGPShader::RefreshPropertyPage( HWND hwnddlg, UINT dialogidtoupdate, BOOL instance )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CDialog *dialog = (CDialog *)CWnd::FromHandle(hwnddlg);
	HKEY			hKey;
	char			path[_MAX_PATH];
	unsigned long	iSize = _MAX_PATH;
	long			ret;
	unsigned long	dword;


	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Hash, Inc.\\Animation Master 2000 V8.5",0,KEY_READ,&hKey);
	if (ret!=ERROR_SUCCESS)
	{
		MessageBox(NULL,"Animation Master Information Not In Registry (v8.5)", "ERROR", 1);
	} else
	{
		ret=RegQueryValueEx(hKey,"InstallationDir",NULL,&dword,(unsigned char *)path,&iSize);
		if (ret!=ERROR_SUCCESS)
		{
			MessageBox(NULL,"Hash Installation Path Not Found in the Registry", "ERROR", 1);
		}
	}

	strcat(path,"\\shaders\\terraintypes.txt");

	if (dialogidtoupdate == IDD_ATTRIBUTES_PAGE2)
	{
		dlg2hwnd=hwnddlg;
		char buf[30];
		//Update the Pre-Tesselation settings
		_gcvt(m_pretess,20,buf);
		dialog->SetDlgItemText(IDC_EDIT_PRETESS,buf);
		//If the Environment map check is marked, enable the button and window
		dialog->CheckDlgButton(IDC_CHECK_ENVIRON_MAP,m_environmap);
		dialog->CheckDlgButton(IDC_CHECK_PRIVATE_FIX,m_private);
		dialog->CheckDlgButton(IDC_CHECK_ALPHA,m_alpha);
		dialog->GetDlgItem(IDC_EDIT_MAP_PERCENT)->EnableWindow(m_environmap);
		dialog->GetDlgItem(IDC_CHECK_PRIVATE_FIX)->EnableWindow(m_environmap);
		dialog->GetDlgItem(IDC_CHECK_ALPHA)->EnableWindow(m_environmap);
		dialog->GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(m_environmap);
		dialog->GetDlgItem(IDC_EDIT_MAP_PATH)->EnableWindow(m_environmap);
		if(m_environmap)
		{
			dialog->GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(m_private);
			dialog->GetDlgItem(IDC_EDIT_MAP_PATH)->EnableWindow(m_private);
		};

		//Update the file name and percentage for the Environment map
		dialog->SetDlgItemInt(IDC_EDIT_MAP_PERCENT,m_environmentmappercent);
		dialog->SetDlgItemText(IDC_EDIT_MAP_PATH,m_environmappath);
		//Update tessadj radio buttons
		dialog->CheckDlgButton(IDC_RADIO_TESS_NONE,(m_tessadj==0)?TRUE:FALSE);
		dialog->CheckDlgButton(IDC_RADIO_TESS_ABS,(m_tessadj==1)?TRUE:FALSE);
		dialog->CheckDlgButton(IDC_RADIO_TESS_MUL,(m_tessadj==2)?TRUE:FALSE);
      dialog->GetDlgItem(IDC_EDIT_PRETESS)->EnableWindow(m_tessadj);
	}
	if (dialogidtoupdate == IDD_ATTRIBUTES_PAGE) {
		FILE *fp;

		dlg1hwnd=hwnddlg;
		//Clear the terrain types dialog
		SendDlgItemMessage(hwnddlg,IDC_COMBO_TERRAIN,CB_RESETCONTENT,0,0);

		dialog->GetDlgItemText(IDC_COMBO_PROJECT,m_projecttext,100);

		//Update the project combo box value and list selection
		SendDlgItemMessage(hwnddlg,IDC_COMBO_PROJECT,CB_SETCURSEL,m_project,0L);
		//Make sure we're displaying the correct project's terrain types
		if((fp=fopen(path,"rb"))==NULL)
		{
			MessageBox(hwnddlg,"Unable to open file:","File Access Error",1);
			MessageBox(hwnddlg,path,"File Access Error",1);
		} else
		{
			int i=0;
			bool bMatch=false;
			do {
				char buf[250];
				fgets(buf,250,fp);
				//Strip off CR/LF
				for (unsigned int i=0; i<strlen(buf); i++)
					if (buf[i]<16)
						buf[i]=0;

				if (buf[0]!='[')
				{
					if (bMatch)
					{
						if (strcmp(m_terraintext,buf)==0)
							m_terrain=i;
						SendDlgItemMessage(hwnddlg,IDC_COMBO_TERRAIN,CB_ADDSTRING,0,(LPARAM)buf);
						i++;
					}
				} else
				{
					if (strcmp(buf,m_projecttext)==0)
						bMatch=true;
					else
						bMatch=false;
				}
			}while (!feof(fp));
			fclose(fp);
		}

		
		//Update the Display Layer settings
		dialog->SetDlgItemInt(IDC_EDIT_LAYER,m_layer,TRUE);
		//Update the collision list box selection
		SendDlgItemMessage(hwnddlg,IDC_COMBO_COLLISION,CB_SETCURSEL,m_collision,0L);
		//Update the terrain combo box value and list selection
		SendDlgItemMessage(hwnddlg,IDC_COMBO_TERRAIN,CB_SETCURSEL,m_terrain,0L);
		dialog->GetDlgItemText(IDC_COMBO_TERRAIN,m_terraintext,100);
		//Update the check boxes
		dialog->CheckDlgButton(IDC_CHECK_FLAT_2,m_flat);
		dialog->CheckDlgButton(IDC_CHECK_DNCLIP_2,m_dnclip);
		dialog->CheckDlgButton(IDC_CHECK_DNFOG,m_dnfog);
		dialog->CheckDlgButton(IDC_CHECK_TWOSIDED,m_twosided);
		dialog->CheckDlgButton(IDC_CHECK_ZBIAS,m_zbias);
		dialog->CheckDlgButton(IDC_CHECK_LOD,m_lod);
		dialog->CheckDlgButton(IDC_CHECK_DELETE,m_deletepatch);
		dialog->CheckDlgButton(IDC_CHECK_INVISIBLE,m_invisible);
		dialog->CheckDlgButton(IDC_CHECK_COLOROVERRIDE,m_coloroverride);
		dialog->CheckDlgButton(IDC_CHECK_32BITVC,m_32bitvc);
	}
}

UINT AGPShader::GetPanelPage(int index)
{
   switch (index) {
      case 0:
         return IDD_ATTRIBUTES_PAGE;
         break;
      case 1:
         return IDD_ATTRIBUTES_PAGE2;
         break;
   }
   return 0;
}
