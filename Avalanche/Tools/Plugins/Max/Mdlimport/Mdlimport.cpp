////////////////////////////////////////////////////////////////////////////
//
// MdlImport
//
// Hash Model Loader for 3dsMax
//
////////////////////////////////////////////////////////////////////////////

#include "Max.h"
#include "stdmat.h"

#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include <direct.h>
#include <commdlg.h>
#include <io.h>
#include "MdlImport.h"
#include "maxtools.h"
#include "filepath.h"

#include "mdlobject.h"
#include "hstatus.h"

#include "chorobject.h"
#include <units.h>
#include "max_macros.h"
#include <splshape.h>
#include <modstack.h>
#include <iskin.h>

static MdlImportClassDesc MdlImportDesc;
ClassDesc2* GetMdlImportDesc() { return &MdlImportDesc; }


//--- MdlImport -------------------------------------------------------
MdlImport::MdlImport()
{
//	m_variableTesselation = FALSE;
//	m_fixTJunctions = TRUE;
//	m_planarCheck = FALSE;
	m_numSubdivisions = 1;
	m_importAsSplines = FALSE;
	m_importMDLasXRef = false;
//	m_straightnessTolerance = 5;
}

MdlImport::~MdlImport() 
{

}

int MdlImport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 2;
}

const TCHAR *MdlImport::Ext(int n)
{		
	switch(n)
	{
		case 0:
			return _T("MDL");
		case 1:
			return _T("CHO");
	}
	return _T("");
}

const TCHAR *MdlImport::LongDesc()
{
	return GetString(IDS_LONG_DESCRIPTION);
}
	
const TCHAR *MdlImport::ShortDesc() 
{			
	return GetString(IDS_SHORT_DESCRIPTION);
}

const TCHAR *MdlImport::AuthorName()
{			
	return _T("Avalanche Software");
}

const TCHAR *MdlImport::CopyrightMessage() 
{	
	return _T("Copyright 2002 Avalanche Software");
}

const TCHAR *MdlImport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *MdlImport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int MdlImport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

BOOL CALLBACK AboutBoxDlgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
			CenterWindow(hWnd, GetParent(hWnd)); 
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					EndDialog(hWnd, 1);
					break;
			}
			break;
			default:
				return FALSE;
	}
	return TRUE;
}       

void MdlImport::ShowAbout(HWND hWnd)
{			
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}

BOOL CALLBACK MdlImportOptionsDlgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	Interval animRange;
	ISpinnerControl  *spin;

	MdlImport *imp = (MdlImport*)GetWindowLongPtr(hWnd,GWLP_USERDATA); 
	switch (msg)
	{
		case WM_INITDIALOG:	 
			{
				imp = (MdlImport*)lParam;
				SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam); 
				CenterWindow(hWnd, GetParent(hWnd)); 

//				CheckRadioButton(hWnd, IDC_RADIO_MIDPOINT, IDC_RADIO_VARIABLE, 
//					imp->GetVariableTesselation() ? IDC_RADIO_VARIABLE : IDC_RADIO_MIDPOINT);

				CheckRadioButton(hWnd, IDC_RADIO_POLYGONS, IDC_RADIO_SPLINES, 
					!imp->GetImportAsSplines() ? IDC_RADIO_POLYGONS : IDC_RADIO_SPLINES);


				// Setup the spinner controls for the number of subdivisions
				spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_NUM_SUBDIVISIONS)); 
				spin->LinkToEdit(GetDlgItem(hWnd,IDC_EDIT_NUM_SUBDIVISIONS), EDITTYPE_INT ); 
				spin->SetLimits(1, 10, TRUE); 
				spin->SetScale(1.0f);
				spin->SetValue(imp->GetNumSubDivisions() ,FALSE);
				ReleaseISpinner(spin);

				// Setup the spinner controls for the straightness tolerance
//				spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_STRAIGHTNESS_TOLERANCE)); 
//				spin->LinkToEdit(GetDlgItem(hWnd,IDC_EDIT_STRAIGHTNESS_TOLERANCE), EDITTYPE_INT ); 
//				spin->SetLimits(1, 10, TRUE); 
//				spin->SetScale(1.0f);
//				spin->SetValue(imp->GetStraightnessTolerance() ,FALSE);
//				ReleaseISpinner(spin);

				// Enable / disable controls
//				ShowWindow(GetDlgItem(hWnd, IDC_PLANAR_CHECK), !imp->GetVariableTesselation());
//				BOOL showStraightnessTolerance = (imp->GetVariableTesselation() || imp->GetPlanarCheck());
//				ShowWindow(GetDlgItem(hWnd, IDC_STATIC_STRAIGHTNESS_TOLERANCE), showStraightnessTolerance);
//				ShowWindow(GetDlgItem(hWnd, IDC_EDIT_STRAIGHTNESS_TOLERANCE), showStraightnessTolerance);
//				ShowWindow(GetDlgItem(hWnd, IDC_SPIN_STRAIGHTNESS_TOLERANCE), showStraightnessTolerance);

//				BOOL showNumberSubdivisions = (!imp->GetVariableTesselation() && !imp->GetPlanarCheck());
				BOOL showNumberSubdivisions = !imp->GetImportAsSplines();
				ShowWindow(GetDlgItem(hWnd, IDC_STATIC_NUMBER_OF_SUBDIVISIONS), showNumberSubdivisions);
				ShowWindow(GetDlgItem(hWnd, IDC_EDIT_NUM_SUBDIVISIONS), showNumberSubdivisions);
				ShowWindow(GetDlgItem(hWnd, IDC_SPIN_NUM_SUBDIVISIONS), showNumberSubdivisions);
			}
			break;

		case CC_SPINNER_CHANGE:
			spin = (ISpinnerControl*)lParam; 
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDC_RADIO_POLYGONS:
					{
						BOOL showNumberSubdivisions = !imp->GetImportAsSplines();
						ShowWindow(GetDlgItem(hWnd, IDC_STATIC_NUMBER_OF_SUBDIVISIONS), showNumberSubdivisions);
						ShowWindow(GetDlgItem(hWnd, IDC_EDIT_NUM_SUBDIVISIONS), showNumberSubdivisions);
						ShowWindow(GetDlgItem(hWnd, IDC_SPIN_NUM_SUBDIVISIONS), showNumberSubdivisions);
					}
					break;
				case IDC_RADIO_SPLINES:
					{
						BOOL showNumberSubdivisions = imp->GetImportAsSplines();
						ShowWindow(GetDlgItem(hWnd, IDC_STATIC_NUMBER_OF_SUBDIVISIONS), showNumberSubdivisions);
						ShowWindow(GetDlgItem(hWnd, IDC_EDIT_NUM_SUBDIVISIONS), showNumberSubdivisions);
						ShowWindow(GetDlgItem(hWnd, IDC_SPIN_NUM_SUBDIVISIONS), showNumberSubdivisions);
					}
					break;
				case IDC_IMPRT_MDL_AS_XREF:
					{
						imp->SetImportMDLasXRef(IsDlgButtonChecked(hWnd,IDC_IMPRT_MDL_AS_XREF));
					}
					break;
//				case IDC_RADIO_MIDPOINT:
//				case IDC_RADIO_VARIABLE:
//				case IDC_PLANAR_CHECK:
//					{
//						ShowWindow(GetDlgItem(hWnd, IDC_PLANAR_CHECK), IsDlgButtonChecked(hWnd,IDC_RADIO_MIDPOINT));
//						BOOL showStraightnessTolerance = (IsDlgButtonChecked(hWnd,IDC_RADIO_VARIABLE) || 
//														  IsDlgButtonChecked(hWnd,IDC_PLANAR_CHECK));
//						ShowWindow(GetDlgItem(hWnd, IDC_STATIC_STRAIGHTNESS_TOLERANCE), showStraightnessTolerance);
//						ShowWindow(GetDlgItem(hWnd, IDC_EDIT_STRAIGHTNESS_TOLERANCE), showStraightnessTolerance);
//						ShowWindow(GetDlgItem(hWnd, IDC_SPIN_STRAIGHTNESS_TOLERANCE), showStraightnessTolerance);
//
//						BOOL showNumberSubdivisions = (IsDlgButtonChecked(hWnd,IDC_RADIO_MIDPOINT) && 
//														  !IsDlgButtonChecked(hWnd,IDC_PLANAR_CHECK));
//						ShowWindow(GetDlgItem(hWnd, IDC_STATIC_NUMBER_OF_SUBDIVISIONS), showNumberSubdivisions);
//						ShowWindow(GetDlgItem(hWnd, IDC_EDIT_NUM_SUBDIVISIONS), showNumberSubdivisions);
//						ShowWindow(GetDlgItem(hWnd, IDC_SPIN_NUM_SUBDIVISIONS), showNumberSubdivisions);
//					}
//					break;
				case IDOK:
//					imp->SetVariableTesselation(IsDlgButtonChecked(hWnd, IDC_RADIO_VARIABLE)); 
//					imp->SetPlanarCheck(IsDlgButtonChecked(hWnd, IDC_PLANAR_CHECK)); 

					imp->SetImportAsSplines(IsDlgButtonChecked(hWnd,IDC_RADIO_SPLINES));
					
					spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_NUM_SUBDIVISIONS)); 
					imp->SetNumSubDivisions(spin->GetIVal()); 
					ReleaseISpinner(spin);

//					spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_STRAIGHTNESS_TOLERANCE)); 
//					imp->SetStraightnessTolerance(spin->GetIVal());
//					ReleaseISpinner(spin);

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

int MdlImport::DoImport(const TCHAR *filename,ImpInterface *impI,Interface *mainI, BOOL suppressPrompts)
{
	m_mainI = mainI;

	TCHAR ext[_MAX_EXT];
	FilePath::GetExt(filename,ext);
	int fileType = 0; // default to model file
	if (!stricmp(ext,".CHO"))
		fileType=1; // choreography file

	int result = FALSE;

	switch (fileType)
	{
		case 0:
			ReadConfig();
			if(!suppressPrompts)
			{
				if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MDL_IMPORT_DLG), 
						mainI->GetMAXHWnd(),  MdlImportOptionsDlgProc, (LPARAM)this))
				{
					return TRUE;
				}
			}
			WriteConfig();
			{
				if (!this->GetImportMDLasXRef())
				{
					maxModelInfo modelinfo;
					result = ImportModelFile(filename,mainI,modelinfo);
					if (result) 
					{
						TSTR name = modelinfo.m_node->GetName();
						mainI->MakeNameUnique(name);
						modelinfo.m_node->SetName(name);
						if (modelinfo.m_noBonesNode)
						{
							TSTR noBonesName = name + "nobones";
							modelinfo.m_noBonesNode->SetName(noBonesName);
						}
					}
					else 
					{
						DISPLAY_IN_LISTENER("Failed to import model.\n");
					}
				}
				else
				{
					HModel model;
					model.name = filename;
					std::vector<maxModelInfo> maxModelInfoArray;
					AddChoreographyModel(&model,maxModelInfoArray,mainI);
					AddModelToMaxScene(mainI,model,maxModelInfoArray[0]);
					DeleteModelInfo(mainI,maxModelInfoArray[0]);
					return 1;
				}
			}

			break;
		case 1:
			ReadConfig();
			if(!suppressPrompts)
			{
				if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MDL_IMPORT_DLG), 
						mainI->GetMAXHWnd(),  MdlImportOptionsDlgProc, (LPARAM)this))
				{
					return TRUE;
				}
			}
			WriteConfig();
			result = ImportChoreographyFile(filename,mainI);
			break;
	}
	mainI->RedrawViews(mainI->GetTime(),REDRAW_NORMAL);
	return result;
}


int MdlImport::ImportModelFile(const TCHAR *filename,Interface *mainI,maxModelInfo &modelinfo)
{
	HStatus status;
	mdlObject theModel;

	if(!ReadModelFile(filename,theModel))
		return FALSE;

	theModel.ui_flags.recalcnormals = 0;
	theModel.ui_flags.calcsmoothinggroups = 1;
	MaxTools mt(theModel);

	Object *object;
	Mesh *mesh;
	INode *node;
	INode *noBonesNode=NULL;
	mt.skinModifier = NULL;

	if (this->m_importAsSplines) {
		node = mt.CreateSplinePatchModel(mainI,status);
		if (status != HStatus::kSuccess)
			return false;
	}
	else {
		
		object = (Object *)mainI->CreateInstance(GEOMOBJECT_CLASS_ID, Class_ID(EDITTRIOBJ_CLASS_ID, 0));
		if(!object)
			return FALSE;
		mesh = &((TriObject *)object)->GetMesh();

		status = mt.CreatePolygons(mesh);
		if (status != HStatus::kSuccess) {
			UString comment = UString("ERROR: ") + status.errorString() + "\n";
			DISPLAY_IN_LISTENER(comment.asChar());
			return FALSE;
		}

		status = mt.CreateMaterials(mainI);

		if (!mt.theModel->HasBones())
		{
			node = mainI->CreateObjectNode(object);
		}
		else
		{
			mt.derivedObject = CreateDerivedObject(object);

			mt.skinModifier = (Modifier *)mainI->CreateInstance(OSM_CLASS_ID,SKIN_CLASSID);

			int nummods = mt.derivedObject->NumModifiers();

			mt.derivedObject->AddModifier(mt.skinModifier,NULL,nummods);

			node = mainI->CreateObjectNode(mt.derivedObject);

			noBonesNode = mainI->CreateObjectNode(object);
		}
	}

	Matrix3 tm;
	tm.IdentityMatrix();

	Control *tmc = NewDefaultMatrix3Controller();
	node->SetTMController(tmc);
	node->SetNodeTM(0,tm);

	TSTR name;
	TCHAR fname[_MAX_FNAME];
	FilePath::GetFileName(filename,fname);
	name = fname;
	node->SetName(name);

	if (noBonesNode)
	{
		Control *tmc = NewDefaultMatrix3Controller();
		noBonesNode->SetTMController(tmc);
		noBonesNode->SetNodeTM(0,tm);
		TSTR noBonesName = name + "nobones";
		noBonesNode->SetName(noBonesName);
	}

	modelinfo.m_node = node;
	modelinfo.m_noBonesNode = noBonesNode;

	if (!this->m_importAsSplines)
	{
		mt.AssignMaterials(mainI,node,mesh);
		if (noBonesNode)
		{
			noBonesNode->SetMtl(node->GetMtl());
		}
//		Skeleton Setup
		mt.CreateSkeleton(mainI,modelinfo);
	}

	return TRUE;
}

int MdlImport::ReadModelFile(const TCHAR *filename,mdlObject &model)
{
	HStatus status;

   //tells mdlImport whether to kick back responses to output window
	model.ui_flags.verbose = 0;
	
	// potential use in football
	model.ui_flags.bakeuvtile = 0;
	// potential use in football
	model.ui_flags.relativetexpaths = 0;

	// future implementation
//	model.ui_flags.planarcheck = m_planarCheck;
//	model.ui_flags.variabletessellation = m_variableTesselation;
//	if (m_straightnessTolerance<1) m_straightnessTolerance=1;
//	if (m_straightnessTolerance>10) m_straightnessTolerance=10;
//	model.ui_straightnesstolerance = straightnessToleranceTable[m_straightnessTolerance-1];

	// the number of polys you want to tesselate to in each span
	if (m_numSubdivisions<1) m_numSubdivisions=1;
	if (m_numSubdivisions>10) m_numSubdivisions=10;
	model.ui_numsubdivisions = m_numSubdivisions;

	cerr << endl << "+-----  LOAD MDL  -----------------------------------------------------------+" << endl;

	/////////////////////////////////////////////////////////////////////////////////////

	status = model.load(filename);

	if (status != HStatus::kSuccess)
	{
		UString comment = UString("ERROR: ") + status.errorString() + "\n";
		DISPLAY_IN_LISTENER(comment.asChar());

		cerr << "load failed." << endl;

		return FALSE;
	}
	return TRUE;
}

/****************************************************************************

 Configuration.
 To make all options "sticky" across sessions, the options are read and
 written to a configuration file every time the exporter is executed.

 ****************************************************************************/

TSTR MdlImport::GetCfgFilename(void)
{
	TSTR filename;
	
	filename += m_mainI->GetDir(APP_PLUGCFG_DIR);
	filename += "\\";
	filename += CFGFILENAME;

	return filename;
}

// NOTE: Update anytime the CFG file changes
#define CFG_VERSION 0x02

BOOL MdlImport::ReadConfig(void)
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "rb");
	if (!cfgStream)
		return FALSE;

	// First item is a file version
	int fileVersion = _getw(cfgStream);

	if (fileVersion > CFG_VERSION) {
		// Unknown version
		fclose(cfgStream);
		return FALSE;
	}

	SetImportAsSplines(fgetc(cfgStream));

	// 0x01
//	SetVariableTesselation(fgetc(cfgStream));
	SetNumSubDivisions(fgetc(cfgStream));
//	SetStraightnessTolerance(fgetc(cfgStream));

	// 0x02
//	if (fileVersion>0x01)
//	{
//		SetPlanarCheck(fgetc(cfgStream));
//	}
	SetImportMDLasXRef(fgetc(cfgStream));

	fclose(cfgStream);

	return TRUE;
}

void MdlImport::WriteConfig(void)
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "wb");
	if (!cfgStream)
		return;

	// Write CFG version
	_putw(CFG_VERSION,cfgStream);
	// 0x01


	fputc(GetImportAsSplines(),cfgStream);
//	fputc(GetVariableTesselation(),cfgStream);
	fputc(GetNumSubDivisions(),cfgStream);
//	fputc(GetStraightnessTolerance(),cfgStream);
	// 0x02
//	fputc(GetPlanarCheck(),cfgStream);
	fputc(GetImportMDLasXRef(),cfgStream);

	fclose(cfgStream);
}



int MdlImport::ImportChoreographyFile(const TCHAR *filename,Interface *mainI)
{
	chorObject choreography;
	HStatus status;

	mainI->ClearNodeSelection(FALSE);

	status = choreography.load(filename);
	if (status != HStatus::kSuccess)
	{
		UString comment = UString("ERROR: ") + status.errorString() + "\n";
		DISPLAY_IN_LISTENER(comment.asChar());
		return FALSE;
	}
	AddChoreographyModels(choreography,mainI);
	AddChoreographyCameras(choreography,mainI);
	AddChoreographyLights(choreography,mainI);
	AddChoreographyPaths(choreography,mainI);

	return TRUE;
}

void MdlImport::AddChoreographyPath(chorObject &choreography,HPath *path,BezierShape *beziershape) {
	HStatus status;
	Spline3D *maxspline;
	double unitratio = MaxTools::GetUnitRatio();

	path->splineArray.calculateTangents();
	
	beziershape->NewShape();

	for (HSpline *spline = path->splineArray.begin(); spline != NULL; spline = spline->next()) {
		maxspline = beziershape->NewSpline();
		MaxTools::ConvertHSplineToSpline3D(spline,maxspline,unitratio);
	}

	beziershape->UpdateSels();
	beziershape->InvalidateGeomCache();
}


void MdlImport::AddChoreographyPaths(chorObject &choreography,Interface *mainI)
{
	for (HPath *path = choreography.paths.begin(); path != NULL; path=path->next()) {

		UString comment = UString("Import[") + path->name + UString("]\n");
		DISPLAY_IN_LISTENER(comment.asChar())

		Object *object = (Object *)mainI->CreateInstance(SHAPE_CLASS_ID,Class_ID(SPLINESHAPE_CLASS_ID,0));
		BezierShape *beziershape = &((SplineShape *)object)->GetShape();
		AddChoreographyPath(choreography,path,beziershape);
		INode *node = mainI->CreateObjectNode(object);

		HVector translate = path->chorchannels.getTranslate(0);
		HVector rotate = path->chorchannels.getRotate(0);
		HVector scale = path->chorchannels.getScale(0);

		Matrix3 tm = MaxTools::GetTransformMatrixFromHash(translate,rotate,scale);

		Control *tmc = NewDefaultMatrix3Controller();
		node->SetTMController(tmc);
		node->SetNodeTM(0,tm);
	}
}

void MdlImport::BuildXRef(Interface *mainI,INode *node,TSTR &filename,IXRefObject **pXRef) {
	*pXRef = (IXRefObject *)mainI->CreateInstance(SYSTEM_CLASS_ID, Class_ID(XREFOBJ_CLASS_ID,0));

	TSTR objectName = node->GetName();
	pXRef[0]->Init(filename, objectName, node->GetObjectRef(), FALSE);
	node->SetObjectRef(*pXRef);
}

void MdlImport::XRefIt(Interface *mainI,TSTR &filename,maxModelInfo &modelinfo) {

	modelinfo.m_fileName = filename;

	// does only on boned models
	if (modelinfo.m_noBonesNode)
	{
		BuildXRef(mainI,modelinfo.m_noBonesNode,modelinfo.m_fileName,&modelinfo.m_pXRef);
	}
	else
	{
		// TODO - make this conditional based on a setting
		BuildXRef(mainI,modelinfo.m_node,modelinfo.m_fileName,&modelinfo.m_pXRef);
	}
}

void MdlImport::DeleteModelInfo(Interface *mainI,maxModelInfo &modelinfo) {
	mainI->DeleteNode(modelinfo.m_node,FALSE,FALSE);
	if (modelinfo.m_noBonesNode)
		mainI->DeleteNode(modelinfo.m_noBonesNode,FALSE,FALSE);
//	BONES
	for (int i = 0; i < modelinfo.m_bones.size(); i++)
		mainI->DeleteNode(modelinfo.m_bones[i]);
//	LINKS
	for (i = 0; i < modelinfo.m_links.size(); i++)
		mainI->DeleteNode(modelinfo.m_links[i]);
}

INode *MdlImport::CreateXRef(Interface *mainI,INode *mainNode,INode *nameNode) {
	Object *object = (Object *)mainNode->GetObjectRef();
	INode *node = mainI->CreateObjectNode(object);
	TSTR name = nameNode->GetName(); // use the name from this node
	mainI->MakeNameUnique(name);
	node->SetName(name);
	return node;
}

void MdlImport::AddModelToMaxScene(Interface *mainI,HModel &model,maxModelInfo &modelinfo) {
	INode *mainNode;

	// if there is a no bones node use it
	if (modelinfo.m_noBonesNode)
	{
		mainNode = modelinfo.m_noBonesNode;
	}
	else
	{
		mainNode = modelinfo.m_node;
	}

	INode *node = CreateXRef(mainI,mainNode,modelinfo.m_node);

	Matrix3 oldtm = mainNode->GetNodeTM(0);

	node->SetMtl(mainNode->GetMtl());

	HVector translate = model.chorchannels.getTranslate(0);
	HVector rotate = model.chorchannels.getRotate(0);
	HVector scale = model.chorchannels.getScale(0);

	Matrix3 tm = MaxTools::GetTransformMatrixFromHash(translate,rotate,scale);

	Control *tmc = NewDefaultMatrix3Controller();
	node->SetTMController(tmc);

	tm *= oldtm;
	node->SetNodeTM(0,tm);
	
//	Started to create references to bones, bones hierarchy, & links
//  Realized that it may not be necessary
/*
	for (int i = 0; i < modelinfo.m_bones.size(); i++) {
		node = CreateXRef(mainI,modelinfo.m_bones[i]);

		oldtm = modelinfo.m_bones[i]->GetNodeTM(0);

		Matrix3 tm;
		tm.IdentityMatrix();

		if (i == 0) {
			HVector translate = model.chorchannels.getTranslate(0);
			HVector rotate = model.chorchannels.getRotate(0);
			HVector scale = model.chorchannels.getScale(0);

			tm = MaxTools::GetTransformMatrixFromHash(translate,rotate,scale);
		}

		Control *tmc = NewDefaultMatrix3Controller();
		node->SetTMController(tmc);

		tm *= oldtm;
		node->SetNodeTM(0,tm);
	}
*/
//	for (i = 0; i < modelinfo.m_links.size(); i++) {
//		node = CreateXRef(mainI,modelinfo.m_links[i]);
//	}
}


void MdlImport::AddChoreographyModel(HModel *model,std::vector<maxModelInfo> &maxModelInfoArray,Interface *mainI)
{
			TSTR relativePath,fullPath,name,ext;
			FilePath::GetPathsToFile(relativePath,fullPath,name,ext,model->name.asChar());

			UString comment = UString("Import[") + model->name + UString("]\n");
			DISPLAY_IN_LISTENER(comment.asChar())


			maxModelInfo modelinfo;
			int result = ImportModelFile(fullPath,mainI,modelinfo);

			if (result)
			{
				INode *bone,*link;
				INodeTab nodeTab;

//				add geometry
				nodeTab.Append(1,&modelinfo.m_node);
				if (modelinfo.m_noBonesNode)
					nodeTab.Append(1,&modelinfo.m_noBonesNode);
//				add bones
				for (int j = 0; j < modelinfo.m_bones.size(); j++) {
					bone = modelinfo.m_bones[j];
					nodeTab.Append(1,&bone);
				}
//				add links
				for (j = 0; j < modelinfo.m_links.size(); j++) {
					link = modelinfo.m_links[j];
					nodeTab.Append(1,&link);
				}

				// if it is a boned model then save and xref
//				if (modelinfo.m_noBonesNode)
//				{
					FilePath::ChangeExtension(fullPath,".max"); // point to max file
				
					mainI->FileSaveNodes(&nodeTab,fullPath);
				
					XRefIt(mainI,fullPath,modelinfo);
//				}

				maxModelInfoArray.push_back(modelinfo);
				model->mdlIndex = maxModelInfoArray.size()-1;
			}
			else {
				DISPLAY_IN_LISTENER("Failed to import model.\n");
				modelinfo.m_node = NULL;
				maxModelInfoArray.push_back(modelinfo);
				model->mdlIndex = maxModelInfoArray.size()-1;
			}
}


void MdlImport::AddChoreographyModels(chorObject &choreography,Interface *mainI)
{
	std::vector<maxModelInfo> maxModelInfoArray;
	
 	HModelArray	&models = choreography.models;
	for (int index = 0; index < models.length(); index++) 
	{
		HModel &model = models[index];
		if (!model.flags.kIsInstance) 
		{
			AddChoreographyModel(&model,maxModelInfoArray,mainI);
		}
	}
	for (index = 0; index < models.length(); index++) 
	{
		HModel &model = models[index];
		if (model.flags.kIsInstance) 
		{
			int modelIndex = model.modelIndex;
			int mdlIndex = models[modelIndex].mdlIndex;

			UString comment = UString("Instance[") + model.name + UString("]\n");
			DISPLAY_IN_LISTENER(comment.asChar())

			if (maxModelInfoArray[mdlIndex].m_node) {
				AddModelToMaxScene(mainI,model,maxModelInfoArray[mdlIndex]);
			}
			else {
				comment = "Geometry NULL. Possibly due to import error.\n";
				DISPLAY_IN_LISTENER(comment.asChar())
			}
		}
	}
	for (index = 0; index < maxModelInfoArray.size(); index++) 
		DeleteModelInfo(mainI,maxModelInfoArray[index]);
}

void MdlImport::AddChoreographyCameras(chorObject &choreography,Interface *mainI)
{
	std::vector<INode *> nodeArray;

	double unitratio = MaxTools::GetUnitRatio();

 	HCameraArray &cameras = choreography.cameras;
	for (int index = 0; index < cameras.length(); index++) 
	{
		HCamera &camera = cameras[index];
		if (!camera.flags.kIsInstance) 
		{
			UString comment = UString("Import[ ") + camera.name + UString("]\n");
			DISPLAY_IN_LISTENER(comment.asChar())

			GenCamera *object = (GenCamera *)mainI->CreateInstance(CAMERA_CLASS_ID,Class_ID(SIMPLE_CAM_CLASS_ID,0));
			object->Enable(1);
			// TODO camera attributes
			INode *node = mainI->CreateObjectNode(object);
			node->SetName("Camera"); //TSTR(camera.name.asChar()));
			camera.cameraIndex = nodeArray.size(); // store what the index in the nodearray this camera is (cameraIndex is only used for instances)
			nodeArray.push_back(node);
		}
	}
	for (index = 0; index < cameras.length(); index++) 
	{
		HCamera &camera = cameras[index];
		if (camera.flags.kIsInstance) 
		{
			UString comment = UString("Instance[ ") + camera.name + UString("]\n");
			DISPLAY_IN_LISTENER(comment.asChar())

			int cameraIndex = camera.cameraIndex;
			int arrayIndex = cameras[cameraIndex].cameraIndex; // get the index into the nodeArray

			INode *mainNode = nodeArray[arrayIndex];

			Object *object = (Object *)mainNode->GetObjectRef();
			INode *node = mainI->CreateObjectNode(object);
			TSTR name = mainNode->GetName();
			mainI->MakeNameUnique(name);
			node->SetName(name);

			Matrix3 tm;
			tm.IdentityMatrix();

			HVector transVector = camera.chorchannels.getTranslate(0);
			HVector rotVector = camera.chorchannels.getRotate(0);
			HVector scaleVector = camera.chorchannels.getScale(0);

			// build translation and scale (make sure we scale)
			Point3 transPoint(transVector.x*unitratio,transVector.y*unitratio,transVector.z*unitratio);
			Point3 scalePoint(scaleVector.x*unitratio,scaleVector.y*unitratio,scaleVector.z*unitratio);

			// build matrix the way it should be in maya
			tm.Scale(scalePoint);
			tm.RotateZ(rotVector.z*piOver180);
			tm.RotateX(rotVector.x*piOver180);
			tm.RotateY(rotVector.y*piOver180);
			tm.Translate(transPoint);

			// convert to a max matrix
			Matrix3 htom,mtoh; // hashtomax - maxtohash
			htom.IdentityMatrix();
			htom.RotateY(180*piOver180); // hashtomax is 180 rotation about y

			mtoh.IdentityMatrix();
			mtoh.RotateX(90.0*piOver180); // maxtohash is 90 rotation about x

			tm = htom*tm; // get local(hash)->parent(max) matrix
			tm*=mtoh; // get local(max)->parent(max) matrix

			Control *tmc = NewDefaultMatrix3Controller();
			node->SetTMController(tmc);
			node->SetNodeTM(0,tm);
		}
	}
	for (index = 0; index < nodeArray.size(); index++) 
	{
		INode *node = nodeArray[index];
		mainI->DeleteNode(node,FALSE,FALSE);
	}
}

void MdlImport::AddChoreographyLights(chorObject &choreography,Interface *mainI)
{
	std::vector<INode *> nodeArray;

	double unitratio = MaxTools::GetUnitRatio();

 	HLightArray &lights = choreography.lights;
	for (int index = 0; index < lights.length(); index++) 
	{
		HLight &light = lights[index];
		if (!light.flags.kIsInstance) 
		{
			UString comment = UString("Import[ ") + light.name + UString("]\n");
			DISPLAY_IN_LISTENER(comment.asChar())

			GenLight *object = mainI->CreateLightObject(DIR_LIGHT);
			object->SetUseLight(1);
			object->Enable(1);
			// TODO light attributes
			INode *node = mainI->CreateObjectNode(object);
			node->SetName("Light"); //TSTR(light.name.asChar()));
			light.lightIndex = nodeArray.size(); // store what the index in the nodearray this light is (lightIndex is only used for instances)
			nodeArray.push_back(node);
		}
	}
	for (index = 0; index < lights.length(); index++) 
	{
		HLight &light = lights[index];
		if (light.flags.kIsInstance) 
		{
			UString comment = UString("Instance[ ") + light.name + UString("]\n");
			DISPLAY_IN_LISTENER(comment.asChar())

			int lightIndex = light.lightIndex;
			int arrayIndex = lights[lightIndex].lightIndex; // get the index into the nodeArray

			INode *mainNode = nodeArray[arrayIndex];

			Object *object = (Object *)mainNode->GetObjectRef();
			INode *node = mainI->CreateObjectNode(object);
			TSTR name = mainNode->GetName();
			mainI->MakeNameUnique(name);
			node->SetName(name);

			Matrix3 tm;
			tm.IdentityMatrix();

			HVector transVector = light.chorchannels.getTranslate(0);
			HVector rotVector = light.chorchannels.getRotate(0);
			HVector scaleVector = light.chorchannels.getScale(0);

			// build translation and scale (make sure we scale)
			Point3 transPoint(transVector.x*unitratio,transVector.y*unitratio,transVector.z*unitratio);
			Point3 scalePoint(scaleVector.x*unitratio,scaleVector.y*unitratio,scaleVector.z*unitratio);

			// build matrix the way it should be in maya
//			tm.Scale(scalePoint);
			tm.RotateZ(rotVector.z*piOver180);
			tm.RotateX(rotVector.x*piOver180);
			tm.RotateY(rotVector.y*piOver180);
			tm.Translate(transPoint);

			// convert to a max matrix
			Matrix3 htom,mtoh; // hashtomax - maxtohash
			htom.IdentityMatrix();
			htom.RotateY(180*piOver180); // hashtomax is 180 rotation about y
			mtoh.IdentityMatrix();
			mtoh.RotateX(90.0*piOver180); // maxtohash is 90 rotation about x
			tm = htom*tm; // get local(hash)->parent(max) matrix
			tm*=mtoh; // get local(max)->parent(max) matrix

			Control *tmc = NewDefaultMatrix3Controller();
			node->SetTMController(tmc);
			node->SetNodeTM(0,tm);
			mainI->AddLightToScene(node);
		}
	}
	for (index = 0; index < nodeArray.size(); index++) 
	{
		INode *node = nodeArray[index];
		mainI->DeleteNode(node,FALSE,FALSE);
	}
}