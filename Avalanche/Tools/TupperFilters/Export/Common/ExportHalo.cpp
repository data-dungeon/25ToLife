////////////////////////////////////////////////////////////////////////////
//
// ExportHalo
//
// Export Halo sources and fields into the DBL file
//
////////////////////////////////////////////////////////////////////////////
#include "ExportPch.h"
#include "ExportHalo.h"
#include "EachNode.h"
#include "effects\halo\halodbl.h"
#include "math\mathclass.h"

#define HALO_TAG	"halo"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
ExportHalo::ExportHalo(
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
u32 ExportHalo::WriteHalos(
TupNodePool &tupNodePool)
{
	// Use the handy eachnode class to do the grunt work
	EachNode eachNode(m_tupConfigRoot, tupNodePool, "instance_type", HALO_TAG);

	// How many sources are there?
	int sources = eachNode.Count();
	if (!sources)
		return 0;

	// Get the dbl file
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// Start the chunk
	DBLUtil::StartChunk(DB_HALO, DB_HALO_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	// add the record list (dummy data is written and later replaced when the chunk is finished)
	DBLUtil::StartRecordList(sources, RECORD_LIST_SAME_SIZE_RECORDS, sizeof(ts_EnvHalo));

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
		keyValue.AddKey("HaloSize");
		keyValue.AddKey("HaloHasDir");
		keyValue.AddKey("HaloHasRot");
		keyValue.AddKey("HaloRed");
		keyValue.AddKey("HaloGreen");
		keyValue.AddKey("HaloBlue");
		keyValue.AddKey("HaloAlpha");
		keyValue.AddKey("HaloIndex");

		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue, keyValue, false);

		ASSERT(strcmp(keyValue.GetValueAsString(0), HALO_TAG) == 0);

		#ifdef PS2_EXPORT
//		DBLUtil::AlignChunkDataQuadWord();
		#endif

		// Inform the DBL util stuff that's it's api is obfuscated
	 	DBLUtil::RecordListEntry();

		// Start our structure
		ts_EnvHalo	source;
		memset(&source, '\0', sizeof(source));

		// size
		source.size = keyValue.GetValueAsFloat (1);

		// has direction/has rotation
		int	flags = 0;
		if ( keyValue.GetValueAsInt(2) == 1)
			flags |= HALOFLAG_HASDIR;

		if ( keyValue.GetValueAsInt(3) == 1)
			flags |= HALOFLAG_ROTATE;

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

		// get the color
		source.color.R = keyValue.GetValueAsInt(4);
		source.color.G = keyValue.GetValueAsInt(5);
		source.color.B = keyValue.GetValueAsInt(6);
		source.color.A = keyValue.GetValueAsInt(7);

		// get index
		source.textureIndex = keyValue.GetValueAsInt(8);

	 	// write the source out
	 	DBLWrite::WriteRecord(&source, pDBLFile);

		// Next node
		tupNode = eachNode.Next(tupNode);
	}
	DBLUtil::FinishRecordList();

	return DBLUtil::FinishChunk(FALSE);
}


