#pragma once

#define COMMAND_LINE_H

class CommandLine
{
public:
	bool m_bVerbose;
	bool m_bPauseWhenDone;
	bool m_bExcludeModelBone;
	bool m_bWriteNamesAsChunk;
	float m_transErrorTolerance;
	float m_rotErrorTolerance;
	float m_scaleErrorTolerance;

	bool m_bInputFileSetOutputFileName;
	bool m_bInputFileSetHeaderFileName;

	TupString m_inputFileName;
	TupString m_outputFileName;
	TupString m_headerFileName;
	TupString m_boneGroupTableFileName;
	TupString m_outputMSetFileName;

	CommandLine(void);
	bool ParseCommandLine(int argc, char **argv);
	void PrintCommands(void);

private:
	void ProcessCommand(const char *pCmd);
	const char *EatWhiteSpace(const char *pString);
	bool IsArg(const char *pArg);
	void PrintError(const char *pString);
};