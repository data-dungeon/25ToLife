// MotionSet5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KFM.h"
#include "CommandLine.h"
#include "MotionManager.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("MotionSet5 - KFM to motion database converter\n");
	printf("Version %s - Build %s %s\n",g_pMotionSet5Version,g_pMotionSet5Date,g_pMotionSet5Time);

	CommandLine commandLine;
	if (!commandLine.ParseCommandLine(argc,argv))
	{
		commandLine.PrintCommands();
		return 1; // error
	}

	printf("Using Default Translation ErrorTolerance = %f\n", commandLine.m_transErrorTolerance);
	printf("Using Default Rotation ErrorTolerance = %f\n", commandLine.m_rotErrorTolerance);
	printf("Using Default Scale ErrorTolerance = %f\n", commandLine.m_scaleErrorTolerance);

	if(commandLine.m_bExcludeModelBone)
		printf("Excluding the Model Bone\n");
	else
		printf("Including the Model Bone\n");

	if(commandLine.m_bWriteNamesAsChunk)
		printf("Writing motion names as a separate chunk in the database file\n");
	else
		printf("Writing motion names as a text header file\n");

	int nStartTickCount = GetTickCount();

	int result = 0;

	MotionManager motionManager(commandLine);

	try
	{
		motionManager.ReadBoneGroupTableFile();
		motionManager.ReadInputFile();
		motionManager.ReadMotionAndAnalyze();
		motionManager.PrintAnalyzeStats();
		if (commandLine.m_outputFileName.GetLength())
		{
			motionManager.WriteDatabaseFile();
		}
		if (!commandLine.m_bWriteNamesAsChunk || commandLine.m_headerFileName.GetLength())
		{
			motionManager.WriteHeaderFile();
		}
		if (commandLine.m_outputMSetFileName.GetLength())
		{
			motionManager.WriteMSetFile();
		}
	}
	catch (Exception e)
	{
		printf("Error %s",(const char *)(e));
		printf("Processing Aborted\n");
		result = 1;
	}
	catch (...)
	{
		printf("Unhandled Exception\r\nProcessing Aborted\n");
		result = 1;
	}

	int nTicks = GetTickCount() - nStartTickCount;
	printf("Total Time: %02d Min %02d Sec\n", (nTicks / 1000) / 60, (nTicks / 1000) % 60);

	if( commandLine.m_bPauseWhenDone)
	{
		printf("Press any key to continue...\n");
		getch( );
	}

	return result;
}

