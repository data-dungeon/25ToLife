//*************************************************************************
// TJunc - t-junction analyzer
//*************************************************************************
#ifndef TJUNC_H
#define TJUNC_H

#include <stdio.h>

#include <platform\win32\winapp.h>
#include <Game/Database/DBFile.h>
#include <Game/Database/Instance.h>
#include <Game/Managers/RenderMgr.h>
#include <Game/Managers/SceneMgr.h>

class TJunc : public WindowsApp
{
public:
	TJunc(void);
	~TJunc(void);

	// Initialize
	bool InitInstance(HINSTANCE hPrevInst, LPSTR cmdLine, int nCmdShow);

	// Terminate
	void TerminateInstance(void);

	// Called every processing "frame"
	void Process(void);

	int OnCreate(LPCREATESTRUCT createStuff);
	void OnDestroy(void);
	int OnSize(int sizeType, int width, int height);

private:
	// Internal proto's
	bool ParseCommandLine(char *cmdLine);
	bool Initialize(void);
	bool LoadWorld(void);

	bool CreateReport(void);
	void AnalyzeSOMs(void);
	void AnalyzeWorld(void);
	bool CloseReport(void);

	bool ErrorExit(char *reason, bool ret = false);

	ts_CollisionSOM *GetSOM(CInstance &instance);

	// The command line param's
	char 						*infile;
	FILE						*reportFile;
	float 					vertexMerge;
	bool						terrainOnly;

	// The data to convert
	ts_Group 				*groupList;
	unsigned int 			groupCount;
	t_Handle					worldHandle;	// returned handle from dblLoad to identify world database

	// some stats
	uint						somMergedVertices;
	uint						somTJunctions;
	uint						somRedundantTriangles;
};

#endif
