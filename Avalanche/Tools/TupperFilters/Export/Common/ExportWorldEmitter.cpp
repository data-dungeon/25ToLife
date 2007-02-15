////////////////////////////////////////////////////////////////////////////
//
// ExportWorldEmitter
//
// Export World Emitters into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportWorldEmitter.h"


ExportWorldEmitter::ExportWorldEmitter(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager), m_tupConfigRoot(tupConfigRoot)
{
}

u32 ExportWorldEmitter::WriteWorldEmmitters(TupNodePool &tupNodePool)
{
	int totalWorldEmitters = 0;
	int totalNodes = tupNodePool.GetNumNodes();
	int nodeIndex;
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);

		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many world emitters we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey("instance_type");
		const int INSTANCE_TYPE = 0;
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
		// if set to a world emitter
		if (!strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"world_emitter"))
		{
			totalWorldEmitters++;
		}
	}
	if (!totalWorldEmitters)
		return 0;

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	DBLUtil::StartChunk(DB_WORLD_EMITTERS, DB_WORLD_EMITTERS_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
	/* add the record list (dummy data is written and later replaced when the chunk is finished) */
	DBLUtil::StartRecordList(totalWorldEmitters, RECORD_LIST_HAS_OFFSET_TABLE, 0);
	/* write each named point */
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);

		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many named points we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey("instance_type");
		tupKeyValueContext.AddKey("emitter_name");
		tupKeyValueContext.AddKey("emitter_type");
		tupKeyValueContext.AddKey("align_emitter_to_rotation");
		tupKeyValueContext.AddKey("emitter_path");
		tupKeyValueContext.AddKey("LOD1RadiusOverride");
		tupKeyValueContext.AddKey("LOD2RadiusOverride");
		const int _INSTANCE_TYPE = 0;
		const int _EMITTER_NAME = 1;
		const int _EMITTER_TYPE = 2;
		const int _ALIGN_EMITTER_TO_ROTATION = 3;
		const int _EMITTER_PATH = 4;
		const int _LOD1_RADIUS_OVERRIDE = 5;
		const int _LOD2_RADIUS_OVERRIDE = 6;
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
		// if set to a world emitter
		if (!strcmp(tupKeyValueContext.GetValueAsString(_INSTANCE_TYPE),"world_emitter"))
		{
			DBLUtil::RecordListEntry();
			// Get Transformation information
			TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
			Vector3 vOrigin;
			vOrigin.Set(0.0f,0.0f,0.0f); // default is middle of world
			Vector3 vRotation;
			vRotation.Set(0.0f,0.0f,0.0f); // default is no rotation
			u16 type = tupKeyValueContext.GetSelectionIndex(_EMITTER_TYPE);	// index 2 is "emitter_type"
			u16 flags = 0;
			if (tupKeyValueContext.GetValueAsBool(_ALIGN_EMITTER_TO_ROTATION))	// index 3 is "align_emitter_to_rotation"
			{
				flags |= ALIGN_EMITTER_TO_ROTATION;
			}
			TupTransform tupTransform(pTransformAgg);
			if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
			{
				TupStaticMatrix tupStaticMatrix(pTransformAgg);
				float *pMatrix;
				tupStaticMatrix.GetMatrix(&pMatrix);
				Graphics4x4 localToWorld = Graphics4x4(
					pMatrix[0], pMatrix[1], pMatrix[2],
					pMatrix[4], pMatrix[5], pMatrix[6],
					pMatrix[8], pMatrix[9], pMatrix[10],
					pMatrix[12], pMatrix[13], pMatrix[14]);      // translation

				vOrigin = localToWorld.GetTranslation( ); // get translation
				localToWorld.DecodeToYXZ(vRotation); // get rotation (may need to tweak later)
			}
			float lod1RadiusOverride = 1000.0f;
			float lod2RadiusOverride = 1000.0f;
			if (tupKeyValueContext.ValueValidAt(_LOD1_RADIUS_OVERRIDE))
			{
				lod1RadiusOverride = tupKeyValueContext.GetValueAsFloat(_LOD1_RADIUS_OVERRIDE);
			}
			if (tupKeyValueContext.ValueValidAt(_LOD2_RADIUS_OVERRIDE))
			{
				lod2RadiusOverride = tupKeyValueContext.GetValueAsFloat(_LOD2_RADIUS_OVERRIDE);
			}

			// write location
			DBLWrite::WriteRecord(&vOrigin, pDBLFile);
			// write the rotation
			DBLWrite::WriteRecord(&vRotation, pDBLFile);
			// write the type
			DBLWrite::WriteRecord(&type, pDBLFile);
			// write the flags
			DBLWrite::WriteRecord(&flags, pDBLFile);
			// write the lod radius values
			DBLWrite::WriteRecord(&lod1RadiusOverride, pDBLFile);
			DBLWrite::WriteRecord(&lod2RadiusOverride, pDBLFile);

			// write the name
			const char *pName;
			pName = tupKeyValueContext.GetValueAsString(_EMITTER_NAME);
			DBLWrite::WriteData(pName,strlen(pName)+1,pDBLFile);

			const char *pPath;
			pPath = tupKeyValueContext.GetValueAsString(_EMITTER_PATH);
			DBLWrite::WriteData(pPath,strlen(pPath)+1,pDBLFile);

			DBLUtil::AlignFile();
		}
	}
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}
