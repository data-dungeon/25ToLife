; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CIEApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ie.h"
LastPage=0

ClassCount=5
Class1=CExportModelDialog
Class2=CIEApp

ResourceCount=2
Resource1=IDD_EXPORTMODELSDIALOG
Class3=CExportModelsDialog
Class4=CImportModelDialog
Class5=CExportActionsDialog
Resource2=IDD_EXPORTMODELDIALOG

[CLS:CExportModelDialog]
Type=0
BaseClass=CDialog
HeaderFile=ExportModelDialog.h
ImplementationFile=ExportModelDialog.cpp
Filter=D
VirtualFilter=dWC
LastObject=CExportModelDialog

[CLS:CIEApp]
Type=0
BaseClass=CWinApp
HeaderFile=IE.h
ImplementationFile=IE.cpp
LastObject=CIEApp
Filter=N
VirtualFilter=AC

[DLG:IDD_EXPORTMODELDIALOG]
Type=1
Class=CExportModelDialog
ControlCount=11
Control1=IDC_STATIC,button,1342177287
Control2=IDC_GENERATEMODELMAPS,button,1342242819
Control3=IDC_STATIC,static,1342308354
Control4=IDC_MINSIZE_COMBO,combobox,1344339971
Control5=IDC_STATIC,static,1342308354
Control6=IDC_MAXSIZE_COMBO,combobox,1344339971
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816
Control9=IDC_STATIC,button,1342177287
Control10=IDC_FILENAME,static,1342308352
Control11=IDC_BROWSE,button,1342242816

[DLG:IDD_EXPORTMODELSDIALOG]
Type=1
Class=CExportModelsDialog
ControlCount=13
Control1=IDC_LISTLABEL,static,1342308352
Control2=IDC_MODELLIST_COMBO,listbox,1352730963
Control3=IDC_STATIC,button,1342177287
Control4=IDC_OUTPUTDIR,static,1342308352
Control5=IDC_BROWSE,button,1342242816
Control6=IDC_STATIC,button,1342177287
Control7=IDC_GENERATEMODELMAPS,button,1342242819
Control8=IDC_STATIC,static,1342308354
Control9=IDC_MINSIZE_COMBO,combobox,1344339971
Control10=IDC_STATIC,static,1342308354
Control11=IDC_MAXSIZE_COMBO,combobox,1344339971
Control12=IDOK,button,1342242817
Control13=IDCANCEL,button,1342242816

[CLS:CExportModelsDialog]
Type=0
HeaderFile=ExportModelsDialog.h
ImplementationFile=ExportModelsDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CExportModelsDialog

[CLS:CImportModelDialog]
Type=0
HeaderFile=ImportModelDialog.h
ImplementationFile=ImportModelDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CImportModelDialog

[CLS:CExportActionsDialog]
Type=0
HeaderFile=ExportActionsDialog.h
ImplementationFile=ExportActionsDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CExportActionsDialog

