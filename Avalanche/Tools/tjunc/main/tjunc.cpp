//***************************************************************************
// TJunc - t-junction analyzer
//***************************************************************************
#include <windows.h>
#include <assert.h>

#include <Game/Database/DBFile.h>
#include <Game/Managers/RenderMgr.h>
#include <Game/Managers/SceneMgr.h>
#include <Game/Misc/GameEngine.h>
#include <Display/Common/DisplayState.h>
#include <Display/Common/GfxFunc.h>
#include <Display/Common/DisplayPlatform.h>
#include <Layers/Media.h>
#include <Layers/debug.h>

#include <toolhelper\cmdline.h>
#include <toolhelper\pathfile.h>

#include "instlist.h"
#include "somlist.h"
#include "tanalyze.h"
#include "tjunc.h"

// Bootstrap ourselves
TJunc 		tjunc;
WindowsApp *WindowsApp::me = &tjunc;

// The command line parser
CommandLine commandLine;

// Nasty globals
extern "C" {
unsigned short 		gbl_KeyBDIn;
bool						TwoPlayerMode;
bool						gbl_bDebugNvidia = FALSE;
}

//***************************************************************************
//***************************************************************************
TJunc::TJunc(void)
{
	infile = 0;
	reportFile = 0;
	vertexMerge = 0.003f;
	terrainOnly = false;

	somMergedVertices = 0;
	somTJunctions = 0;
	somRedundantTriangles = 0;

	groupList = 0;
	groupCount = 0;
	worldHandle = INVALID_HANDLE;
}

//***************************************************************************
//***************************************************************************
TJunc::~TJunc(void)
{
}

//***************************************************************************
// Init everything, load the world, convert, exit
//***************************************************************************
bool TJunc::InitInstance(
HINSTANCE hPrevInst,
LPSTR cmdLine,
int nCmdShow)
{
	// Did the fully specify the conversion?
	if (!ParseCommandLine(cmdLine))
		return(ErrorExit("Usage:\ntjunc -i<inputfile> -v<vertexmerge> [-t (terrain only)"));

	// Initialize all the systems
	if (!Initialize())
		return(ErrorExit("Error: Cannot init game systems."));

	// Load up the input file
	if (!LoadWorld())
		return(ErrorExit("Error: Cannot load input file."));

	// Create the report
	if (!CreateReport())
		return(ErrorExit("Error: Cannot create report file."));

	// Analyze the each SOM
	AnalyzeSOMs();

	// Analyze the whole world
	AnalyzeWorld();

	// Close the report
	if (!CloseReport())
		return(ErrorExit("Error: Cannot close report file."));

	return(true);
}

//***************************************************************************
// Clean up
//***************************************************************************
void TJunc::TerminateInstance(void)
{
	GameEngine__Shutdown();
}

//***************************************************************************
// Called every processing "frame"
//***************************************************************************
void TJunc::Process(void)
{
	Quit();
}

//***************************************************************************
//***************************************************************************
bool TJunc::ParseCommandLine(
char *cmdLine)
{
	// Call the parse
	if (!commandLine.Parse(cmdLine))
		return(false);

	// Get the options into locals
	infile = commandLine.GetParam('i');
	if (!infile || *infile == '\0')
		return(false);

	// Get the terrain flag
	if (commandLine.GetParam('t'))
		terrainOnly = true;

	// Get the vertex merge
	char *temp = commandLine.GetParam('v');
	if (temp)
		vertexMerge = (float)atof(temp);

	// Good command line!
	return(true);
}

//***************************************************************************
// Initialize all the systems
//***************************************************************************
bool TJunc::Initialize(void)
{
	/* no display stuff needed for tool-- this skips all display initialization */
	Display__SetDisplayNeeded(FALSE);

	// init the game engine
	if (GameEngine__Init() == FALSE)
		return(false);

	return(true);
}

//*************************************************************************
// Create message
//*************************************************************************
int TJunc::OnCreate(
LPCREATESTRUCT createStuff)
{
	return(gfxOnCreate(hWnd) == FALSE ? -1 : 0);
}

//*************************************************************************
// Destroy message
//*************************************************************************
void TJunc::OnDestroy(void)
{
	Display__DestroyWindow();	// clean after window is closed
	gfxOnDestroy();
}

//*************************************************************************
// Window is sizing
//*************************************************************************
int TJunc::OnSize(
int sizeType,
int width,
int height)
{
	gfxOnResize(width, height);
	return(0);
}

//***************************************************************************
// Load in the world file
//***************************************************************************
bool TJunc::LoadWorld(void)
{
	medEnableNameMangling(false);	//allow opening of exact filename passed in

	if(strstr(infile, ".dbl"))
		PathFile::StripExt(infile);

	char pathFile[256];
	worldHandle = dblLoad(PathFile::MakePath(pathFile, NULL, infile, "dbc"));
	if (worldHandle == INVALID_HANDLE)
	{
		// error if not found-- can't patch som's
		char temp[512];
		sprintf(temp, "Cannot open %s for input.", pathFile);
		return(ErrorExit(temp, false));
	}
	RenderMgr__SetWorldDBHandle(worldHandle);	// let render manager know about world database for texture context management

	/* get pointer to model data in database */
	ts_RecordList *recordList = (ts_RecordList *)dblGetChunkData(DB_COLLISIONSOMARRAY, DEFAULT_CHUNK_KEY, worldHandle);
	if (recordList == NULL)
	{
		// error if not found-- can't patch som's
		char temp[512];
		sprintf(temp, "%s does not contain any collision data.", pathFile);
		return(ErrorExit(temp, false));
	}

	// Check the datatype header
	ts_PatchSOM *som = (ts_PatchSOM *)GetRecordAddress(recordList, 0);
#ifdef SHIP_IT
	if ((som->dh.u16Flags & SOMM_HAS_NAME) != 0)
#else
	if ((som->dh.u16Flags & SOMM_HAS_NAME) == 0)
#endif
	{
		// This is bad
		assert("Incorrect database version" == 0);
		return(false);
	}

	/* get pointer to group chunk in world database */
	recordList = (ts_RecordList *) dblGetChunkData(DB_GROUP, DEFAULT_CHUNK_KEY, worldHandle);
	if (recordList == NULL)
		return(false);

	/* record group data for collision too */
	groupList = (ts_Group *)GetRecordAddress(recordList, 0);
	groupCount = recordList->usRecordCount;

	return(true);
}

//***************************************************************************
// Create the report
//***************************************************************************
bool TJunc::CreateReport(void)
{
	// Clean up the filename
	char report[256];
	strcpy(report, infile);
	if (PathFile::HasExt(report))
		PathFile::StripExt(report);
	strcat(report, ".txt");

	// Open it!
	reportFile = fopen(report, "wt");
	if (!reportFile)
		return(false);

	// Give it to them!
	if (terrainOnly)
		fprintf(reportFile, "TJunc terrain report for \"%s\" using tolerance %0.4f\n\n", infile, vertexMerge);
	else
		fprintf(reportFile, "TJunc report for \"%s\" using tolerance %0.4f\n\n", infile, vertexMerge);

	return(true);
}

//***************************************************************************
// Analyze the each SOM
//***************************************************************************
void TJunc::AnalyzeSOMs(void)
{
	SOMList somList;
	somList.Build(groupList, groupCount);
	for (int i = 0; i < somList.Count(); i++)
	{
		// Some aliases
		ts_CollisionSOM *som = somList.Get(i);
		Vector3 *vertex = (Vector3 *)som->pVertexBuffer;

		// Debugging
		TRACE("Analyzing SOM \"%s\" (%d of %d)\n", som->Name, i, somList.Count());
		
		// Our analyzer
		TJuncAnalyze analyze(vertexMerge * vertexMerge);

		// Add all the polys
		for (uint p = 0; p < som->u32PolyCount; p++)
		{
			ts_CollisionPoly &poly = som->pPolyBuffer[p];

			if (terrainOnly && !poly.MaterialBitfield.IsTerrain)
				continue;

			Vector3 &v1 = vertex[poly.u16VertexIndex[0]];
			Vector3 &v2 = vertex[poly.u16VertexIndex[1]];
			Vector3 &v3 = vertex[poly.u16VertexIndex[2]];
			Vector3 &v4 = vertex[poly.u16VertexIndex[3]];

			if (poly.u16VertexCount == 4)
				analyze.AddQuad(v1, v2, v3, v4);
			else
				analyze.AddTriangle(v1, v2, v3);
		}

		// Do the analysis
		TJuncAnalyze::Stats stats;
		stats = analyze.Analyze();
		if (analyze.SomethingBad(stats))
		{
			if (!terrainOnly)
				fprintf(reportFile, "Problems found with model \"%s\":\n", som->Name);
			else
				fprintf(reportFile, "Problems found with terrain \"%s\":\n", som->Name);

			if (stats.redundantTriangles > 0)
				fprintf(reportFile, "\t Redundant tri's: %d\n", stats.redundantTriangles);
			if (stats.degenerateTriangles > 0)
				fprintf(reportFile, "\tDegenerate tri's: %d\n", stats.degenerateTriangles);
			if (stats.mergedVertices > 0)
				fprintf(reportFile, "\t Unwelded vert's: %d\n", stats.mergedVertices);
			if (stats.tJunctions > 0)
				fprintf(reportFile, "\t        t-junc's: %d\n", stats.tJunctions);
			fflush(reportFile);

			somMergedVertices += stats.mergedVertices;
			somTJunctions += stats.tJunctions;
			somRedundantTriangles += stats.redundantTriangles;
		}
	}
}

//***************************************************************************
// Analyze the whole world
//***************************************************************************
void TJunc::AnalyzeWorld(void)
{
	// Make a single list of instances
	InstanceList instanceList;
	instanceList.Build(groupList, groupCount);

	// Our analyzer
	TJuncAnalyze analyze(vertexMerge * vertexMerge);

	// Build the data
	for (int i = 0; i < instanceList.Count(); i++)
	{
		// Some aliases
		CInstance *instance = instanceList.Get(i);
		Graphics4x4 &localToWorld = instance->mLocalToWorld;
		ts_CollisionSOM *som = GetSOM(*instance);
		Vector3 *vertex = (Vector3 *)som->pVertexBuffer;

		// Debugging
		TRACE("Building instance \"%s\" (%d of %d)\n", instance->pInstancePart2->Name, i, instanceList.Count());

		// Add all the polys
		for (uint p = 0; p < som->u32PolyCount; p++)
		{
			ts_CollisionPoly &poly = som->pPolyBuffer[p];
			if (terrainOnly && !poly.MaterialBitfield.IsTerrain)
				continue;

			Vector3 v[4];
			for (int t = 0; t < poly.u16VertexCount; t++)
				v[t] = vertex[poly.u16VertexIndex[t]] * localToWorld;

			if (poly.u16VertexCount == 4)
				analyze.AddQuad(v[0], v[1], v[2], v[3]);
			else
				analyze.AddTriangle(v[0], v[1], v[2]);
		}
	}

	// Do the analysis
	TRACE("About to analyze world...\n");
	TJuncAnalyze::Stats stats;
	stats = analyze.Analyze();
	TRACE("Done analyzing world.\n");

	if (terrainOnly)
		fprintf(reportFile, "\n\nTerrain statistics for \"%s\":\n\n", infile);
	else
		fprintf(reportFile, "\n\nWorld statistics for \"%s\":\n\n", infile);
	fprintf(reportFile, "           Triangles: %d\n", stats.triangles);
	fprintf(reportFile, "               Edges: %d\n", stats.edges);
	fprintf(reportFile, "            Vertices: %d\n\n", stats.vertices);

	fprintf(reportFile, " Redundant triangles: %d\n", stats.redundantTriangles);
	fprintf(reportFile, "Degenerate triangles: %d\n\n", stats.degenerateTriangles);

	fprintf(reportFile, "   Unwelded vertices: %d\n", stats.mergedVertices);
	fprintf(reportFile, "         t-junctions: %d\n\n", stats.tJunctions);

	fprintf(reportFile, "Interinstance unwelded vert's: %d\n", stats.mergedVertices - somMergedVertices);
	fprintf(reportFile, "       Interinstance t-junc's: %d\n", stats.tJunctions - somTJunctions);
	fprintf(reportFile, "    Interinstance tri dupes's: %d\n", stats.redundantTriangles - somRedundantTriangles);
}

//***************************************************************************
// Close the report
//***************************************************************************
bool TJunc::CloseReport(void)
{
	if (reportFile)
	{
		fprintf(reportFile, "\nReport complete.\n");
		fclose(reportFile);
		reportFile = NULL;
	}

	return(true);
}

//***************************************************************************
// Exit the program with a reason
//***************************************************************************
bool TJunc::ErrorExit(
char *reason,
bool ret)
{
	MessageBox(NULL, reason, "Error!", MB_ICONHAND | MB_OK);
	return(ret);
}

//***************************************************************************
//***************************************************************************
ts_CollisionSOM *TJunc::GetSOM(
CInstance &instance)
{
	// Get the pointer to the model
	ts_CollisionSOM *som = NULL;
	if ((instance.dh.u16Flags & INSTANCE_HAS_LOD) == 0)
	{
		// No LOD here
		ASSERT(instance.pSOM != NULL);
		som = (ts_CollisionSOM *)instance.pSOM;
	}
	else
	{
		// Get the highest LOD
		ASSERT(instance.pLOD != NULL);
		som = (ts_CollisionSOM *)instance.pLOD->pSOM[0];
	}

	return(som);
}


