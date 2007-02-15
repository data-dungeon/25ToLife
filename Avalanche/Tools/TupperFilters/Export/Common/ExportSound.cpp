////////////////////////////////////////////////////////////////////////////
//
// ExportSound
//
// Export sound sources and fields into the DBL file
//
////////////////////////////////////////////////////////////////////////////
#include "ExportPch.h"
#include "ExportSound.h"
#include "EachNode.h"
#include "audio\environment\EnvDBL.h"
#include "math\mathclass.h"

#define SOUNDSOURCE_TAG	"SoundSource"
#define SOUNDFIELD_TAG	"SoundField"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
ExportSound::ExportSound(
const char *pDBLFileName,
const FilterOptions &options,
FilterDataManager &dataManager,
TupConfigRoot &tupConfigRoot) :
m_pDBLFileName(pDBLFileName),
m_options(options),
m_dataManager(dataManager),
m_tupConfigRoot(tupConfigRoot)
{
}

////////////////////////////////////////////////////////////////////////////
// Write the sources
////////////////////////////////////////////////////////////////////////////
u32 ExportSound::WriteSources(
TupNodePool &tupNodePool)
{
	// Use the handy eachnode class to do the grunt work
	EachNode eachNode(m_tupConfigRoot, tupNodePool, "instance_type", SOUNDSOURCE_TAG);

	// How many sources are there?
	int sources = eachNode.Count();
	if (!sources)
		return 0;

	// Get the dbl file
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// Start the chunk
	DBLUtil::StartChunk(DB_SOUNDSOURCE, DB_SOUNDSOURCE_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	// add the record list (dummy data is written and later replaced when the chunk is finished)
	DBLUtil::StartRecordList(sources, RECORD_LIST_SAME_SIZE_RECORDS, sizeof(ts_EnvSoundSource));

	// write each named point
	TupNode *tupNode = eachNode.First();
	while (tupNode)
	{
		// Setup the key value shiznit
		char *nodeAttributesString;
		tupNode->GetUserProperties(&nodeAttributesString);
		TupKeyValue tupKeyValue(nodeAttributesString, BEGIN_BLOCK, END_BLOCK);

		TupKeyValueContext keyValue;
		keyValue.AddKey("instance_type");
		keyValue.AddKey("sourceName");
		keyValue.AddKey("audibleRange");
		keyValue.AddKey("sourceVolume");
		keyValue.AddKey("sourcePitch");
		keyValue.AddKey("sourceAmbience");
		keyValue.AddKey("sourcePresence");
		keyValue.AddKey("sourceTriggerMin");
		keyValue.AddKey("sourceTriggerMax");
		keyValue.AddKey("sourceDirectional");
		keyValue.AddKey("sourceMinConeAngle");
		keyValue.AddKey("sourceMaxConeAngle");
		keyValue.AddKey("sourceStream");
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue, keyValue, false);

		ASSERT(strcmp(keyValue.GetValueAsString(0), SOUNDSOURCE_TAG) == 0);

		// Inform the DBL util stuff that's it's api is obfuscated
	 	DBLUtil::RecordListEntry();

		// Start our structure
		ts_EnvSoundSource	source;
		memset(&source, '\0', sizeof(source));

		// Fill out the structure
		const char *name = keyValue.GetValueAsString(1);
		strncpy(source.name, name, sizeof(source.name) - 1);

		// Stereo amibient
		const char *sourceStream = keyValue.GetValueAsString(12);
		if (strcmp(sourceStream, "STREAM") == 0)
			source.flags |= ENVSOUNDSOURCE_STREAM;
		else if (strcmp(sourceStream, "STREAMLOOPED") == 0)
			source.flags |= ENVSOUNDSOURCE_STREAMLOOPED;

		// Audible distance
		source.audibleDistance = keyValue.GetValueAsFloat(2);

		// volume
		int percent = keyValue.GetValueAsInt(3);
		source.volume = Math::Clamp((float)percent / 100.0f);

		// pitch
		source.pitch = keyValue.GetValueAsFloat(4);

		// ambience
		percent = keyValue.GetValueAsInt(5);
		source.ambience = Math::Clamp((float)percent / 100.0f);

		// presence
		percent = keyValue.GetValueAsInt(6);
		source.presence = Math::Clamp((float)percent / 100.0f);

		// Trigger time
		source.minTriggerTime = (s32)(keyValue.GetValueAsFloat(7) * 1000.0f);
		source.maxTriggerTime = (s32)(keyValue.GetValueAsFloat(8) * 1000.0f);
		if (source.minTriggerTime > source.maxTriggerTime)
			source.minTriggerTime = source.maxTriggerTime = 0;

	 	// Get the position
	 	TupperwareAggregate *transformAgg = tupNode->GetTransform();
	 	TupTransform tupTransform(transformAgg);
		Vector3 dir;
		dir.Clear();
	 	if (tupTransform.GetTransformType() == TupTransform::STATIC_MATRIX)
	 	{
	 		TupStaticMatrix tupStaticMatrix(transformAgg);
	 		float *matrix;
	 		tupStaticMatrix.GetMatrix(&matrix);
	 		source.position.x(matrix[12]);
	 		source.position.y(matrix[13]);
	 		source.position.z(matrix[14]); // get translation off of matrix

			// Pull the direction off of Z
			dir.x(matrix[8]);
			dir.y(matrix[9]);
			dir.z(matrix[10]);
	 	}

		// Here are some values we are more hesitant to include
		if (keyValue.ValueValidAt(9))
		{
			const char *directional = keyValue.GetValueAsString(9);
			if (strcmp(directional, "DIRECTIONAL") == 0)
			{
				source.flags |= ENVSOUNDSOURCE_DIRECTIONAL;

				// Assign the dir (from up earlier)
				source.dir = dir;

				// The cone angles, only for directional sounds
				source.minConeAngle = Math::Deg2Rad(keyValue.ValueValidAt(10) ? keyValue.GetValueAsFloat(10) : 90.0f);
				source.maxConeAngle = Math::Deg2Rad(keyValue.ValueValidAt(11) ? keyValue.GetValueAsFloat(11) : 90.0f);
			}
		}

	 	// write the source out
	 	DBLWrite::WriteRecord(&source, pDBLFile);

		// Next node
		tupNode = eachNode.Next(tupNode);
	}
	DBLUtil::FinishRecordList();

	return DBLUtil::FinishChunk(FALSE);
}

////////////////////////////////////////////////////////////////////////////
// Write the sound fields
////////////////////////////////////////////////////////////////////////////
u32 ExportSound::WriteFields(
TupNodePool &tupNodePool)
{
	// Use the handy eachnode class to do the grunt work
	EachNode eachNode(m_tupConfigRoot, tupNodePool, "instance_type", SOUNDFIELD_TAG);

	// How many sources are there?
	int sources = eachNode.Count();
	if (!sources)
		return 0;

	// Get the dbl file
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// Start the chunk
	DBLUtil::StartChunk(DB_SOUNDFIELD, DB_SOUNDFIELD_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	// add the record list (dummy data is written and later replaced when the chunk is finished)
	DBLUtil::StartRecordList(sources, RECORD_LIST_SAME_SIZE_RECORDS, sizeof(ts_EnvSoundField));

	// write each named point
	TupNode *tupNode = eachNode.First();
	while (tupNode)
	{
		// Setup the key value shiznit
		char *nodeAttributesString;
		tupNode->GetUserProperties(&nodeAttributesString);
		TupKeyValue tupKeyValue(nodeAttributesString, BEGIN_BLOCK, END_BLOCK);

		TupKeyValueContext keyValue;
		keyValue.AddKey("instance_type");
		keyValue.AddKey("reverbEchoEffect");
		keyValue.AddKey("reverbEchoVolume");
		keyValue.AddKey("ambientSoundName");
		keyValue.AddKey("ambientStereo");
		keyValue.AddKey("ambientVolume");
		keyValue.AddKey("ambientRetriggerMin");
		keyValue.AddKey("ambientRetriggerMax");
		keyValue.AddKey("ambientFadeInTime");
		keyValue.AddKey("ambientFadeOutTime");
		keyValue.AddKey("reverbEchoTaper");
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue, keyValue, false);

		ASSERT(strcmp(keyValue.GetValueAsString(0), SOUNDFIELD_TAG) == 0);

		// Inform the DBL util stuff that's it's api is obfuscated
	 	DBLUtil::RecordListEntry();

		// Start our structure
		ts_EnvSoundField	field;
		memset(&field, '\0', sizeof(field));

		// Do the obb
  		TupperwareAggregate *nodeAgg = tupNode->GetBaseAggregate();
		field.position = *(Vector3 *)nodeAgg->FindListByKey("Bounding_OBBCenter")->GetAsFloat();
		field.orientation = *(DirCos3x3 *)nodeAgg->FindListByKey("Bounding_OBBOrientation")->GetAsFloat();
		field.extents = *(Vector3 *)nodeAgg->FindListByKey("Bounding_OBBExtents")->GetAsFloat();

		// The effect name (run-time makes into ID)
		const char *effect = keyValue.GetValueAsString(1);
		strncpy(field.effect, effect, sizeof(field.effect) - 1);

		int effectVolumePercent = keyValue.GetValueAsInt(2);
		field.effectVolume = Math::Clamp((float)effectVolumePercent / 100.0f);

		int effectTaperPercent = keyValue.ValueValidAt(10) ? keyValue.GetValueAsInt(10) : 0;
		field.effectTaper = Math::Clamp((float)effectTaperPercent / 100.0f);

		// Ambient sound
		const char *ambientSound = keyValue.GetValueAsString(3);
		strncpy(field.ambient, ambientSound, sizeof(field.ambient) - 1);

		// Stereo amibient
		const char *ambientStereo = keyValue.GetValueAsString(4);
		if (strcmp(ambientStereo, "STEREO") == 0)
			field.flags |= ENVSOUNDFIELD_AMBIENTSTEREO;

		// volume
		int percent = keyValue.GetValueAsInt(5);
		field.volume = Math::Clamp((float)percent / 100.0f);

		// Trigger time
		field.minRetriggerTime = keyValue.GetValueAsInt(6);
		field.maxRetriggerTime = keyValue.GetValueAsInt(7);
		if (field.minRetriggerTime > field.maxRetriggerTime)
			field.minRetriggerTime = field.maxRetriggerTime = 0;

		// Values we are on the fence about supporting
		field.fadeInTime = keyValue.ValueValidAt(8) ? keyValue.GetValueAsFloat(8) : 1.0f;
		field.fadeOutTime = keyValue.ValueValidAt(9) ? keyValue.GetValueAsFloat(9) : 2.0f;

	 	// write the source out
	 	DBLWrite::WriteRecord(&field, pDBLFile);

		// Next node
		tupNode = eachNode.Next(tupNode);
	}
	DBLUtil::FinishRecordList();

	return DBLUtil::FinishChunk(FALSE);
}

