//*************************************************************************
// Halo builder
//*************************************************************************
#ifndef ANGEL_H
#define ANGEL_H

#include <Game/Database/SOM.h>
#include <Game/Database/World.h>
#include <container\darray.h>
#include <platform\win32\winapp.h>
#include <toolhelper\dynstr.h>
#include <effects\haloele.h>

#include "stats.h"
#include "statsdlg.h"

class Angel : public WindowsApp
{
public:
	Angel(void);
	~Angel(void);

	// Initialize
	bool InitInstance(HINSTANCE hPrevInst, LPSTR cmdLine, int nCmdShow);

	// Terminate
	void TerminateInstance(void);

	// Called every processing "frame"
	void Process(void);

	// Do we handle this message for an accelerator/dlg
	bool HandleMessage(MSG &msg);

	int OnCreate(LPCREATESTRUCT createStuff);
	void OnDestroy(void);
	int OnSize(int sizeType, int width, int height);

private:
	// Internal proto's
	bool ParseCommandLine(char *cmdLine);
	bool Initialize(void);
	bool ErrorExit(char *reason, bool ret = false);
	bool LoadWorld(void);
	static void ErrorReport(bool warning, char *error);

	// Instance report stuff
	bool GenerateInstanceReport(char *instanceReportName);
	ts_CollisionSOM *GetSOM(CInstance &instance);
	bool FindInstance(CInstance *instance);

	// Load in the model file
	bool LoadModelFile(char *modelFile);

	// Process an instance
	bool ProcessInstance(CInstance &instance);

	// Write the datafile
	bool WriteFile(char *outFile);

	// The main parameters
	char 									*infile;
	char 									*outfile;
	char									*modelfile;
	char									*instanceReport;

	// The list of stuff to process
	t_Handle 							worldHandle;	// returned handle from dblLoad to identify world database
	DynamicArray<CInstance *> 	instanceList;

	// The model stuff
	typedef struct
	{
		char				name[32];
		Vector3			offset;
		Vector3			dir;
		int				spriteIndex;
		float				size;
		ts_bRGBA			color;
		bool				useAngle;
	} MODELHALO;
	DynamicArray<MODELHALO>			modelHalo;

	// the output array
	DynamicArray<HALOINIT>			haloInit;

	// Our status display
	Stats									stats;
	StatsDisplayDialog				*statsDlg;
	bool									waitForClose;

	// parsing aids
	char *SkipBlanks(char *parse);
	char *ParseName(char *parse, char *name, int maxlen);
	char *ParseVector(char *parse, Vector3 &v);
	char *ParseInt(char *parse, int &i);
	char *ParseReal(char *parse, float &r);
	char *ParseBool(char *parse, bool &b);
};

#endif
