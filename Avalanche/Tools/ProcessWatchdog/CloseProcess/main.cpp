// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CloseProcess.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("CloseProcess - command line application to close a process by name.\n");
	if (argc<2)
	{
		printf("Usage: CloseProcess processName [waitInSeconds]\n");
		printf("if waitInSeconds is not specified 5 seconds will be used\n");
		return 0;
	}

	int waitForResponse = 5000;
	if (argc==3)
	{
		float waitFloat =  (float) atof(argv[2]);
		printf("Wait set to %f seconds\n",waitFloat);
		waitForResponse = (int)(waitFloat*1000);
	}
	else
	{
		printf("Wait set to default of 5 seconds\n");
	}

	bool bResult = CloseProcess::KillProcess(argv[1],waitForResponse);
	if (bResult)
		return 0;
	else
		return 1;
}

