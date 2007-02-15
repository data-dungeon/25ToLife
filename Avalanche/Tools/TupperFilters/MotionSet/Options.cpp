
#include "MotionSetPch.h"
#include "Options.h"

static const float DEFAULT_ERROR_TOLERANCE  = 0.001f;
static const float DEFAULT_TRANSLATION_SCALE_FACTOR = 1.0f;

Options::Options(const FilterOptions &parentOptions) : 
	m_bVerbose(false),
	m_bExcludeModelBone(false),
	m_bWriteNamesAsChunk(false),
	m_bWriteHeaderFile(false),
	m_transErrorTolerance(DEFAULT_ERROR_TOLERANCE),
	m_rotErrorTolerance(DEFAULT_ERROR_TOLERANCE),
	m_scaleErrorTolerance(DEFAULT_ERROR_TOLERANCE),
	m_boneGroupTableFileName("BoneGroupTable.tup")
{
	const char *pValue;
	pValue = parentOptions.GetValue("Verbose");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bVerbose = true;
	}

	pValue = parentOptions.GetValue("ExcludeModelBone");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bExcludeModelBone = true;
	}

	pValue = parentOptions.GetValue("WriteNamesAsChunk");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bWriteNamesAsChunk = true;
	}

	pValue = parentOptions.GetValue("WriteHeaderFile");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bWriteHeaderFile = true;
	}

	pValue = parentOptions.GetValue("ErrorTolerance");
	if (pValue)
	{
		m_transErrorTolerance = m_rotErrorTolerance = m_scaleErrorTolerance = (float)atof(pValue);
	}

	pValue = parentOptions.GetValue("TranslationErrorTolerance");
	if (pValue)
	{
		m_transErrorTolerance = (float)atof(pValue);
	}

	pValue = parentOptions.GetValue("RotationErrorTolerance");
	if (pValue)
	{
		m_rotErrorTolerance = (float)atof(pValue);
	}

	pValue = parentOptions.GetValue("ScaleErrorTolerance");
	if (pValue)
	{
		m_scaleErrorTolerance = (float)atof(pValue);
	}

	m_boneGroupTableFileName = "BoneGroupTable.tup"; // default filename
	pValue = parentOptions.GetValue("BoneGroupTableFile");
	if (pValue)
	{
		m_boneGroupTableFileName = pValue;
	}
}

