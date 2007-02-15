////////////////////////////////////////////////////////////////////////////
//
// ExportWater
//
// Export water into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportWater.h"
#include "EachNode.h"
#include "audio\environment\EnvDBL.h"
#include "effects\water\WaterDBL.h"
#include "math\mathclass.h"

#define WATER_TAG	"water"

////////////////////////////////////////////////////////////////////////////

ExportWater::ExportWater(
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

u32 ExportWater::WriteWater(
TupNodePool &tupNodePool)
{
	// Use the handy eachnode class to do the grunt work
	EachNode eachNode(m_tupConfigRoot, tupNodePool, "instance_type", WATER_TAG);

	// How many nodes are there?
	int nodes = eachNode.Count();
	if (!nodes)
		return 0;

	// Get the dbl file
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// Start the chunk
	DBLUtil::StartChunk(DB_WATER, DB_WATER_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	// add the record list (dummy data is written and later replaced when the chunk is finished)
	DBLUtil::StartRecordList(nodes, RECORD_LIST_SAME_SIZE_RECORDS, sizeof(ts_Water));

	// write each node
	TupNode *tupNode = eachNode.First();
	while (tupNode)
	{
		// Setup the key value shiznit
		char *pNodeAttributesString;
		tupNode->GetUserProperties(&pNodeAttributesString);
		TupKeyValue tupKeyValue(pNodeAttributesString, BEGIN_BLOCK, END_BLOCK);

		TupKeyValueContext keyValue;
		keyValue.AddKey("instance_type");
		keyValue.AddKey("varFile");
		keyValue.AddKey("wrap");
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue, keyValue, false);

		ASSERT(strcmp(keyValue.GetValueAsString(0), WATER_TAG) == 0);

		// Fill the chunk structure in
		ts_Water water;
		memset(&water, '\0', sizeof(water));

		char* name;
		tupNode->GetName(&name);
		strncpy(water.name, name, sizeof (water.name) - 2);

		const char* varFile = keyValue.GetValueAsString(1);
		strncpy(water.varFile, varFile, sizeof (water.varFile) - 2);

		water.wrap = keyValue.GetValueAsFloat(2);

		// Do the obb
		TupperwareAggregate *pNodeAgg = tupNode->GetBaseAggregate();
		Vector3 center = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBCenter")->GetAsFloat();
		DirCos3x3 orientation = *(DirCos3x3 *)pNodeAgg->FindListByKey("Bounding_OBBOrientation")->GetAsFloat();
		Vector3 extents = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBExtents")->GetAsFloat();

		Vector3 pivot = -extents;
		water.pos = pivot * orientation + center;
		water.pos.y( center.y( ) );
		water.size = extents * 2;
		Vector3 zaxis = orientation.GetRow2( );
		water.angle = Math::ArcTan2( zaxis.x( ), zaxis.z( ) );

		// Inform the DBL util stuff that's it's api is obfuscated
	 	DBLUtil::RecordListEntry();

	 	// Write the chunk out
	 	DBLWrite::WriteRecord(&water, pDBLFile);

		// Next node
		tupNode = eachNode.Next(tupNode);
	}
	DBLUtil::FinishRecordList();

	return DBLUtil::FinishChunk(FALSE);
}
