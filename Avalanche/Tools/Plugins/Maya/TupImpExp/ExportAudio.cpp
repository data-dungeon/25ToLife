///////////////////////////////////////////////////////////////////////////
//
// ExportAudio
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupExport.h"

void TupExport::AddAudio(void)
{
	MStatus status;

	// find all motion paths
	MItDependencyNodes iter( MFn::kAudio);
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObject object = iter.item(&status);
		if (!status)
			continue;

		MFn::Type apiType = object.apiType();
		if (apiType != MFn::kAudio)
			continue;

		MFnDependencyNode audioNode(object);

		int audioIndex = m_pAudioPool->AddAudio();
		TupperwareAggregate *pAudioAgg = m_pAudioPool->GetAudio(audioIndex);
		TupAudio tupAudio(pAudioAgg);

		MString name = audioNode.name();
		tupAudio.AddName(name.asChar());

		tupAudio.AddStartTime(MayaHelper::GetTimeAttrib(audioNode,"offset"));
		MString fileName = MayaHelper::GetStringAttrib(audioNode,"filename");
		tupAudio.AddFileName(fileName.asChar());

		int flags = 0;
		tupAudio.AddFlags(flags);
	}
}
