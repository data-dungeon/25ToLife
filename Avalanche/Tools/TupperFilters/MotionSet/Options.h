#ifndef OPTIONS_H
#define OPTIONS_H

class Options
{
public:
	bool m_bVerbose;
	bool m_bExcludeModelBone;
	bool m_bWriteNamesAsChunk;
	bool m_bWriteHeaderFile;
	float m_transErrorTolerance;
	float m_rotErrorTolerance;
	float m_scaleErrorTolerance;

	TupString m_boneGroupTableFileName;

	Options(const FilterOptions &parentOptions);
};

#endif