////////////////////////////////////////////////////////////////////////////
//
// ExportStationaryCam
//
// Export StationaryCam Info
//
////////////////////////////////////////////////////////////////////////////
#include "ExportPch.h"
#include "ExportStationaryCam.h"
#include "EachNode.h"
#include "Camera\StationaryCam\StationaryCamDbl.h"
#include "math\mathclass.h"

#define STATIONARYCAM_TAG	"stationaryCam"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
ExportStationaryCam::ExportStationaryCam(
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
u32 ExportStationaryCam::WriteStationaryCam(
TupNodePool &tupNodePool)
{
	// Use the handy eachnode class to do the grunt work
	EachNode eachNode(m_tupConfigRoot, tupNodePool, "instance_type", STATIONARYCAM_TAG);

	// How many sources are there?
	int sources = eachNode.Count();
	if (!sources)
		return 0;

	// Get the dbl file
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// Start the chunk
	DBLUtil::StartChunk(DB_STATIONARYCAM, DB_STATIONARYCAM_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	// add the record list (dummy data is written and later replaced when the chunk is finished)
	DBLUtil::StartRecordList(sources, RECORD_LIST_SAME_SIZE_RECORDS, sizeof(ts_EnvStationaryCam));

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
		keyValue.AddKey("StationaryCamFov");
		keyValue.AddKey("StationaryCamX");
		keyValue.AddKey("StationaryCamY");
		keyValue.AddKey("StationaryCamSweepSpeed");
		keyValue.AddKey("StationaryCamTime");
		keyValue.AddKey("StationaryCamName");

		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue, keyValue, false);

		ASSERT(strcmp(keyValue.GetValueAsString(0), STATIONARYCAM_TAG) == 0);

		// Inform the DBL util stuff that's it's api is obfuscated
	 	DBLUtil::RecordListEntry();

		// Start our structure
		ts_EnvStationaryCam	source;
		memset(&source, '\0', sizeof(source));

		// size
		source.fov = keyValue.GetValueAsFloat (1);

		// movement in x/y
		source.x = keyValue.GetValueAsFloat (2);
		source.y = keyValue.GetValueAsFloat (3);

		// sweep speed
		source.speed = keyValue.GetValueAsFloat (4);

		// time on each camera
		source.time = keyValue.GetValueAsFloat (5);

	// Fill out the structure
		const char *name = keyValue.GetValueAsString(6);
		strncpy(source.name, name, sizeof(source.name) - 1);

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
	 		source.pos.x(matrix[12]);
	 		source.pos.y(matrix[13]);
	 		source.pos.z(matrix[14]); // get translation off of matrix

			// Pull the direction off of Z
			source.dir.x(matrix[8]);
			source.dir.y(matrix[9]);
			source.dir.z(matrix[10]);
	 	}

	 	// write the source out
	 	DBLWrite::WriteRecord(&source, pDBLFile);

		// Next node
		tupNode = eachNode.Next(tupNode);
	}
	DBLUtil::FinishRecordList();

	return DBLUtil::FinishChunk(FALSE);
}


