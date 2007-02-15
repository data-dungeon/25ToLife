#ifndef __MDLIMPORT__H
#define __MDLIMPORT__H
////////////////////////////////////////////////////////////////////////////
//
// MdlImport
//
// Hash Model Loader for 3dsMax
//
////////////////////////////////////////////////////////////////////////////

#define MDLIMPORT_CLASS_ID	Class_ID(0x5688e15f, 0x73614bcb)

#define CFGFILENAME		_T("MDLIMPORT.CFG")	// Configuration file

#include "maxscrpt/MAXScrpt.h"
#include "maxmodelinfo.h"


extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

class mdlObject;
class chorObject;
class HPath;
class HModel;

class MdlImport : public SceneImport 
{
public:
		static HWND hParams;

		int ExtCount(); // Number of extensions supported
		const TCHAR *Ext(int n); // Extension #n (i.e. "3DS")
		const TCHAR *LongDesc(); // Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *ShortDesc(); // Short ASCII description (i.e. "3D Studio")
		const TCHAR *AuthorName(); // ASCII Author name
		const TCHAR *CopyrightMessage(); // ASCII Copyright message
		const TCHAR *OtherMessage1(); // Other message #1
		const TCHAR *OtherMessage2(); // Other message #2
		unsigned int Version(); // Version number * 100 (i.e. v3.01 = 301)
		void ShowAbout(HWND hWnd); // Show DLL's "About..." box
		int DoImport(const TCHAR *name,ImpInterface *impI,Interface *mainI, BOOL suppressPrompts=FALSE);	// Import file

		int ImportModelFile(const TCHAR *filename,Interface *mainI,maxModelInfo &modelinfo);

		int ReadModelFile( const TCHAR *filename,mdlObject &model);
		int ImportChoreographyFile(const TCHAR *filename,Interface *mainI);

		void AddChoreographyModel(HModel *model,std::vector<maxModelInfo> &maxModelInfoArray,Interface *mainI);	
		void AddChoreographyModels(chorObject &choreography,Interface *mainI);
		void AddChoreographyCameras(chorObject &choreography,Interface *mainI);
		void AddChoreographyLights(chorObject &choreography,Interface *mainI);
		void AddChoreographyPath(chorObject &choreography,HPath *path,BezierShape *beziershape);
		void AddChoreographyPaths(chorObject &choreography,Interface *mainI);

		TSTR GetCfgFilename(void);
		BOOL ReadConfig(void);
		void WriteConfig(void);
		
		//Constructor/Destructor
		MdlImport();
		~MdlImport();		

//		void	SetVariableTesselation(BOOL val) { m_variableTesselation = val; }
//		void	SetFixTJunctions(BOOL val) { m_fixTJunctions = val; }
//		void	SetPlanarCheck(BOOL val) { m_planarCheck = val; }

		void	SetNumSubDivisions(int val) { m_numSubdivisions = val; }
//		void	SetStraightnessTolerance(int val) { m_straightnessTolerance = val; }

//		BOOL	GetVariableTesselation(void) { return m_variableTesselation; }
//		BOOL	GetFixTJunctions(void) { return m_fixTJunctions; }
//		BOOL	GetPlanarCheck(void) { return m_planarCheck; }

		int	GetNumSubDivisions(void) { return m_numSubdivisions; }
//		int	GetStraightnessTolerance(void) { return m_straightnessTolerance; }

		void	SetImportAsSplines(BOOL val) { m_importAsSplines = val; }
		BOOL	GetImportAsSplines() { return m_importAsSplines; }


		int		GetImportMDLasXRef() { return m_importMDLasXRef; };
		void	SetImportMDLasXRef(int importMDLasXRef) { m_importMDLasXRef = importMDLasXRef; }

		CharStream* out;

private:
//		BOOL m_variableTesselation;
//		BOOL m_fixTJunctions;
		int m_numSubdivisions;
//		int m_straightnessTolerance;
//		BOOL m_planarCheck;
		BOOL m_importAsSplines;
		int m_importMDLasXRef;

		Interface*	m_mainI;

		void		XRefIt(Interface *mainI,TSTR &filename,maxModelInfo &modelinfo);
		void		BuildXRef(Interface *mainI,INode *node,TSTR &filename,IXRefObject **pXRef);
		void		DeleteModelInfo(Interface *mainI,maxModelInfo &modelinfo);
		void		AddModelToMaxScene(Interface *mainI,HModel &model,maxModelInfo &modelinfo);
		INode		*CreateXRef(Interface *mainI,INode *mainNode,INode *nameNode);

};


class MdlImportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new MdlImport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_IMPORT_CLASS_ID; }
	Class_ID		ClassID() { return MDLIMPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("MdlImport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

#endif // __MDLIMPORT__H
