//***************************************************************************
// Terrain converter
//***************************************************************************
#include <windows.h>
#include <assert.h>

#include <Math/matrix.h>
#include <Display/Common/DisplayState.h>
#include <Display/Common/GfxFunc.h>
#include <Display/Common/DisplayPlatform.h>
#include <Game/Database/DBFile.h>
#include <Game/Managers/RenderMgr.h>
#include <Game/Managers/SceneMgr.h>
#include <Game/Misc/GameEngine.h>
#include <toolhelper\cmdline.h>
#include <toolhelper\pathfile.h>

#include "angel.h"
#include "resource.h"

// Bootstrap ourselves
Angel angel;
WindowsApp *WindowsApp::me = &angel;

// The command line parser
CommandLine commandLine;

// Nasty globals
extern "C" {
unsigned short 		gbl_KeyBDIn;
bool						TwoPlayerMode;
bool						gbl_bDebugNvidia = TRUE;//if TRUE, then use GL_LINES instead of GL_TRIANGLES so as not to crash nv4ogl.dll
}

//***************************************************************************
//***************************************************************************
Angel::Angel(void)
{
	infile = outfile = modelfile = instanceReport = 0;

	statsDlg = NULL;
	waitForClose = false;
}

//***************************************************************************
//***************************************************************************
Angel::~Angel(void)
{
}

//***************************************************************************
// Init everything, load the world, convert, exit
//***************************************************************************
bool Angel::InitInstance(
HINSTANCE hPrevInst,
LPSTR cmdLine,
int nCmdShow)
{
	// Did the fully specify the conversion?
	if (!ParseCommandLine(cmdLine))
		return(ErrorExit("Usage:\nAngel -i <inputfile> -o <outputfile> -m <model file> [-w -z<instance file>]"));

	// This is ugly
	Display__SetWinInfo(GetDesktopWindow(), hInst, windowProc, nCmdShow, LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)));

	// Initialize all the systems
	if (!Initialize())
		return(ErrorExit("Error: Cannot init game systems."));

	// Load up the input file
	if (!LoadWorld())
		return(ErrorExit("Error: Cannot load input file."));

	// Load the model file
	if (!LoadModelFile(modelfile))
		return(ErrorExit("Error: Cannot load model file."));

	// Create the stats dlg
	g_stats = &stats;
	stats.Clear();
	stats.inputFile = infile;
	stats.outputFile = outfile;
	//stats.totalInstances = converter->TotalInstances();
	stats.SetStatus("Converting world...");
	statsDlg = new StatsDisplayDialog(hInst, Display__GetWnd(), stats);
	if (statsDlg)
		statsDlg->Update();

	// Do an instance report?
	if (instanceReport)
		GenerateInstanceReport(instanceReport);

	return(true);
}

//***************************************************************************
// Clean up
//***************************************************************************
void Angel::TerminateInstance(void)
{
	delete statsDlg;

	GameEngine__Shutdown();
}

//***************************************************************************
// Called every processing "frame"
//***************************************************************************
void Angel::Process(void)
{
	// Process returns true when done
	if (!stats.done)
	{
		// Process an instance
		CInstance *inst = instanceList[stats.onInstance];
		ASSERT(inst);
		stats.SetInstance(inst->pInstancePart2->Name);
		ProcessInstance(*inst);

		// are we done
		++stats.onInstance;
		stats.done = (stats.onInstance == instanceList.Count());
		stats.percentageDone = (float)stats.onInstance / (float)instanceList.Count() * 100.0f;

		if (stats.done)
		{
			// Change the message
			if (statsDlg)
			{
				stats.SetStatus("Writing output file...");
				statsDlg->Update();
			}

			// Write it to a database when done
			if (!WriteFile(outfile))
				ErrorExit("Cannot write output file!");

			// We are done
			if (!waitForClose)
				Quit();
			else
			{
				// Wait for them to press it
				stats.SetStatus("Done.");
				if (statsDlg)
					statsDlg->Update();
			}
		}
	}

	// Update the stats dialog
	if (!stats.done && statsDlg)
		statsDlg->Update();

	// Did they press cancel?
	if (statsDlg && statsDlg->Quit())
		Quit();
}

//***************************************************************************
//***************************************************************************
bool Angel::ParseCommandLine(
char *cmdLine)
{
	// Call the parse
	if (!commandLine.Parse(cmdLine))
		return(false);

	// Get the options into locals
	infile = commandLine.GetParam('i');
	if (!infile || *infile == '\0')
		return(false);

	// get the output file
	DynString *tempStr = commandLine.GetParamDynString('o');
	if (!tempStr)
		return(false);
	if (!PathFile::HasExt(tempStr->Data()))
		tempStr->Cat(".alo");
	outfile = tempStr->Data();

	// get the model file
	tempStr = commandLine.GetParamDynString('m');
	if (!tempStr)
		return(false);
	if (!PathFile::HasExt(tempStr->Data()))
		tempStr->Cat(".mdl");
	modelfile = tempStr->Data();

	// get the report file
	tempStr = commandLine.GetParamDynString('z');
	if (tempStr)
	{
		// Make an extension
		if (!PathFile::HasExt(tempStr->Data()))
			tempStr->Cat(".log");
		instanceReport = tempStr->Data();
	}

	// Do we need to wait?
	if (commandLine.GetParam('w') != NULL)
		waitForClose = true;

	// Good command line!
	return(true);
}

//***************************************************************************
// Initialize all the systems
//***************************************************************************
bool Angel::Initialize(void)
{
	GameEngine__SetMaxSprites(250);
	GameEngine__SetMaxSceneInstances(8000);

	Display__SetScreenWidth(1);
	Display__SetScreenHeight(1);

	// init the game engine
	if (GameEngine__Init() == FALSE)
		return(false);

	// Hide the ugly assed opengl window
	ShowWindow(Display__GetWnd(), SW_HIDE);

	return(true);
}

//*************************************************************************
// Here's were you put your accelerators
//*************************************************************************
bool Angel::HandleMessage(
MSG &msg)
{
	if (statsDlg)
		return(statsDlg->HandleMessage(msg));
	else
		return(WindowsApp::HandleMessage(msg));
}

//*************************************************************************
// Create message
//*************************************************************************
int Angel::OnCreate(
LPCREATESTRUCT createStuff)
{
	return(gfxOnCreate(hWnd) == FALSE ? -1 : 0);
}

//*************************************************************************
// Destroy message
//*************************************************************************
void Angel::OnDestroy(void)
{
	Display__DestroyWindow();	// clean after window is closed
	gfxOnDestroy();
}

//*************************************************************************
// Window is sizing
//*************************************************************************
int Angel::OnSize(
int sizeType,
int width,
int height)
{
	gfxOnResize(width, height);
	return(0);
}

//***************************************************************************
// Exit the program with a reason
//***************************************************************************
bool Angel::ErrorExit(
char *reason,
bool ret)
{
	MessageBox(NULL, reason, "Error!", MB_ICONHAND | MB_OK);
	return(ret);
}

//***************************************************************************
// Load in the world file
//***************************************************************************
bool Angel::LoadWorld(void)
{
	PathFile::StripExt(infile);

	char pathFile[256];
	worldHandle = dblLoad(PathFile::MakePath(pathFile, NULL, infile, "dbc"));
	if (worldHandle == INVALID_HANDLE)
	{
		// error if not found
		char temp[512];
		sprintf(temp, "Cannot open %s for input.", pathFile);
		return(ErrorExit(temp, false));
	}

	// let render manager know about world database for texture context management
	RenderMgr__SetWorldDBHandle(worldHandle);

	/* get pointer to model data in database */
	ts_RecordList *recordList = (ts_RecordList *)dblGetChunkData(DB_COLLISIONSOMARRAY, DEFAULT_CHUNK_KEY_STRING, worldHandle);
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
	recordList = (ts_RecordList *) dblGetChunkData(DB_GROUP, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if (recordList == NULL)
		return(false);

	/* record group data for collision too */
	ts_Group *group= (ts_Group *)GetRecordAddress(recordList, 0);
	uint groupCount = recordList->usRecordCount;

	// Loop through everything
	for (uint index = 0; index < groupCount; index++, group++)
		for (uint instanceCount = 0; instanceCount < group->usDispStatInst; instanceCount++)
		{
			// Get an alias to the instance
			CInstance *instance = group->ppDispStatArray[instanceCount];
			if (!FindInstance(instance))
				instanceList.Add(instance);
		}

	return(true);
}

//***************************************************************************
//***************************************************************************
void Angel::ErrorReport(bool warning, char *error)
{
	if (!warning)
		MessageBox(NULL, error, "Error!", MB_ICONHAND | MB_OK);
	else
	{
		if (g_stats)
			g_stats->SetStatus(error);
	}
}

//***************************************************************************
//***************************************************************************
bool Angel::GenerateInstanceReport(
char *instanceReportName)
{
	ASSERT(instanceReportName);

	if (g_stats)
	{
		char temp[256];
		sprintf(temp, "Generating '%s' instance report.", instanceReportName);
		g_stats->SetStatus(temp);
	}

	// Create the file
	FILE *fh = fopen(instanceReportName, "wt");
	if (!fh)
		return(false);
	fprintf(fh, "Instance report file for %s\n\n", infile);

	// Keep track of the instances we've seen
	for (uint i = 0; i < instanceList.Count(); i++)
	{
		// Get an alias to the instance
		CInstance *instance = instanceList[i];

		// Report it
		fprintf(fh, "Instance %s has models ", instance->pInstancePart2->Name);

		// Loop through the models
		if ((instance->dh.u16Flags & INSTANCE_HAS_LOD) == 0)
		{
			// No LOD here
			ASSERT(instance->pSOM != NULL);
			ts_CollisionSOM *som = (ts_CollisionSOM *)instance->pSOM;
			fprintf(fh, "(%s) ", som->Name);
		}
		else
		{
			// Get the highest LOD
			ASSERT(instance->pLOD != NULL);
			ts_CollisionSOM *som;
			for (uint i = 0; i < MAX_LODS; i++)
			{
				som = (ts_CollisionSOM *)instance->pLOD->pSOM[i];
				fprintf(fh, "(%s) ", som->Name);
			}
		}

		fprintf(fh, "\n");
	}

	fclose(fh);
	return(true);
}

//***************************************************************************
//***************************************************************************
ts_CollisionSOM *Angel::GetSOM(
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

//***************************************************************************
//***************************************************************************
bool Angel::FindInstance(
CInstance *instance)
{
	uint count = instanceList.Count();
	for (uint i = 0; i < count; i++)
		if (instanceList[i] == instance)
			return(true);
	return(false);
}

//***************************************************************************
// Load in the model file
//***************************************************************************
bool Angel::LoadModelFile(
char *modelFile)
{
	FILE *fh = fopen(modelFile, "rt");
	if (!fh)
		return(false);

	// load in the data
	char line[1024];
	while (fgets(line, sizeof(line) - 1, fh) != NULL)
	{
		char *parse = line;

		// skip shit
		parse = SkipBlanks(parse);

		// Comment?
		if (*parse == '#')
			continue;

		// We are setting up one of these
		MODELHALO halo;

		// First parameter is the model name
		parse = ParseName(parse, halo.name, sizeof(halo.name) - 1);
		if (parse == NULL || halo.name[0] == '\0')
			continue;

		// second parameter is offset
		parse = ParseVector(parse, halo.offset);
		if (parse == NULL)
			continue;

		// third parameter is dir
		parse = ParseVector(parse, halo.dir);
		if (parse == NULL)
			continue;

		// next parameter is sprite index
		parse = ParseInt(parse, halo.spriteIndex);
		if (parse == NULL)
			continue;

		// Next parameter is size
		parse = ParseReal(parse, halo.size);
		if (parse == NULL)
			continue;

		// next parameters are rgba
		int color;
		parse = ParseInt(parse, color);
		if (parse == NULL)
			continue;
		halo.color.R = color;

		parse = ParseInt(parse, color);
		if (parse == NULL)
			continue;
		halo.color.G = color;

		parse = ParseInt(parse, color);
		if (parse == NULL)
			continue;
		halo.color.B = color;

		parse = ParseInt(parse, color);
		if (parse == NULL)
			continue;
		halo.color.A = color;

		// next parameter is use angle
		parse = ParseBool(parse, halo.useAngle);
		if (parse == NULL)
			continue;

		// Add it to the list
		modelHalo.Add(halo);
	}

	fclose(fh);
	return(true);
}

//***************************************************************************
// Process an instance
//***************************************************************************
bool Angel::ProcessInstance(
CInstance &instance)
{
	// Get the main som
	ts_CollisionSOM *som = GetSOM(instance);
	if (!som)
		return(true);

	// what is the instance matrix
	Graphics4x4 &localToWorld = *((Graphics4x4 *)&instance.mLocalToWorld);
	
#if 0
	static bool found = false;
	if (strcmpi(som->Name, "las_fancylight.av4") == 0)
		found = true;
#endif

	// Scan through each model for a match
	for (uint i = 0; i < modelHalo.Count(); i++)
	{
		// match?
		char *haloName = modelHalo[i].name;
		if (strcmpi(haloName, som->Name) != 0)
			continue;

		// Transform everything into a new record!
		HALOINIT init;
		init.pos = modelHalo[i].offset * localToWorld;
		init.dir = BasisTransform(modelHalo[i].dir, localToWorld);
		if (init.dir.LengthSquared() > 1e-4f)
			init.dir.Normalize();
		init.color = modelHalo[i].color;
		init.size =	modelHalo[i].size;
		init.flags = 0;
		if (modelHalo[i].useAngle)
			init.flags |= HALOFLAG_ROTATE;
		if (init.dir.LengthSquared() > 0.99f)
			init.flags |= HALOFLAG_HASDIR;

		// Add it!
		haloInit.Add(init);
	}

	return(true);
}

//***************************************************************************
// Write the datafile
//***************************************************************************
bool Angel::WriteFile(
char *outFile)
{
	// Make sure this is gone
	remove(outFile);

	// Anything to do?
	if (haloInit.Count() == 0)
		return(true);

	// Open the file
	FILE *fh = fopen(outFile, "wb");
	if (!fh)
		return(false);

	// setup the header
	HALOINITHDR hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.version = HALOINIT_CURRENTVERSION;
	hdr.totalSpriteTextures = 1;
	hdr.count = haloInit.Count();

	// write it
	if (fwrite(&hdr, sizeof(hdr), 1, fh) != 1)
	{
		fclose(fh);
		return(false);
	}

	// Write the data
	bool ret = (fwrite(haloInit.GetPtr(), sizeof(HALOINIT), hdr.count, fh) == hdr.count);

	// we are done!
	fclose(fh);
	return(ret);
}

//***************************************************************************
// parsing aids
//***************************************************************************
char *Angel::SkipBlanks(
char *parse)
{
	while (*parse == ' ' || *parse == '\t')
		++parse;

	return(parse);
}

//***************************************************************************
//***************************************************************************
char *Angel::ParseName(
char *parse,
char *name,
int maxlen)
{
	// Find a start quote
	while (*parse != '\"' && *parse)
		++parse;

	if (*parse != '\"')
		return(NULL);

	++parse;
	int len = 0;
	while (*parse != '\"' && *parse)
	{
		if (len < maxlen)
			name[len++] = *parse;
		++parse;
	}
	name[len] = '\0';

	if (*parse == '\"')
		++parse;

	return(SkipBlanks(parse));
}

//***************************************************************************
//***************************************************************************
char *Angel::ParseVector(
char *parse,
Vector3 &v)
{
	// Find a start bracket
	while (*parse != '[' && *parse)
		++parse;

	if (*parse != '[')
		return(NULL);
	++parse;

	char temp[256];
	int len = 0;
	while (*parse != ']' && *parse)
	{
		if (len < sizeof(temp))
			temp[len++] = *parse;
		++parse;
	}
	temp[len] = '\0';

	if (*parse == ']')
		++parse;

	// Load it
	sscanf(temp, " %f , %f , %f ", &v.x, &v.y, &v.z);

	return(SkipBlanks(parse));
}

//***************************************************************************
//***************************************************************************
char *Angel::ParseInt(
char *parse,
int &i)
{
	char temp[256];
	int len = 0;
	while (*parse != ' ' && *parse != '\t' && *parse)
	{
		if (len < sizeof(temp))
			temp[len++] = *parse;
		++parse;
	}
	temp[len] = '\0';

	// Load it
	i = atoi(temp);
	return(SkipBlanks(parse));
}

//***************************************************************************
//***************************************************************************
char *Angel::ParseReal(
char *parse,
float &r)
{
	char temp[256];
	int len = 0;
	while (*parse != ' ' && *parse != '\t' && *parse)
	{
		if (len < sizeof(temp))
			temp[len++] = *parse;
		++parse;
	}
	temp[len] = '\0';

	// Load it
	r = (float)atof(temp);
	return(SkipBlanks(parse));
}

//***************************************************************************
//***************************************************************************
char *Angel::ParseBool(
char *parse,
bool &b)
{
	char temp[256];
	int len = 0;
	while (*parse != ' ' && *parse != '\t' && *parse != '\r' && *parse != '\n' && *parse)
	{
		if (len < sizeof(temp))
			temp[len++] = *parse;
		++parse;
	}
	temp[len] = '\0';

	b = (strcmpi(temp, "true") == 0 ||
			strcmpi(temp, "yes") == 0 ||
			strcmpi(temp, "1") == 0);

	return(SkipBlanks(parse));
}

