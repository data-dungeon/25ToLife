////////////////////////////////////////////////////////////////////////////
//
// ExportInstanceAttributes
//
// Export a bunch of key/value pairs into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportInstanceAttributes.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * new chunk
 */

#include "ExportPch.h"
#include "ExportInstanceAttributes.h"


ExportInstanceAttributes::ExportInstanceAttributes(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot,int currentPartition) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager), m_tupConfigRoot(tupConfigRoot), m_currentPartition(currentPartition)
{
}

u32 ExportInstanceAttributes::WriteInstanceAttributes(TupNodePool &tupNodePool)
{
	int totalNamedPoints = 0;
	int totalNodes = tupNodePool.GetNumNodes();
	int nodeIndex;

	bool bStartedChunk = false;
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;

		TupperwareScalar *pRenderPartitionIndexScalar = pNodeAgg->FindScalarByKey("Annotate_RenderPartitionIndex");
		// skip ones that don't have this value
		if (!pRenderPartitionIndexScalar)
			continue;
		
		int renderPartitionIndex = pRenderPartitionIndexScalar->GetAsInt();
		// skip ones that are not in the current partition
		if (renderPartitionIndex!=m_currentPartition)
			continue;
		
		int instanceIndex = pNodeAgg->FindScalarByKey("Annotate_RenderInstanceIndex")->GetAsInt();

		TupNode tupNode(pNodeAgg);

		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many named points we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,true); // get them all

		TupArray<TupKeyValue> groupData;

		BuildGroupData(tupKeyValueContext,groupData);

		int numGroups = groupData.GetSize();
		if (numGroups)
		{
			if (!bStartedChunk)
			{
				DBLUtil::StartChunk(DB_INSTANCE_ATTRIBUTES, DB_INSTANCE_ATTRIBUTES_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
				bStartedChunk = true;
			}
			WriteGroupData(groupData,instanceIndex);
		}
	}
	if (bStartedChunk)
	{
		FILE *pDBLFile = DBLUtil::GetDBLFile();

		char nullEntry = 0;
		DBLWrite::WriteData(&nullEntry,1,pDBLFile);

		return DBLUtil::FinishChunk(FALSE);
	}
	else
	{
		return 0; // didn't do it
	}
}	

void ExportInstanceAttributes::BuildGroupData(TupKeyValueContext &tupKeyValueContext,TupArray<TupKeyValue> &groupData)
{
	int numKeyValues = tupKeyValueContext.GetNumAttribs();
	for (int parentAttribIndex=0;parentAttribIndex<numKeyValues;parentAttribIndex++)
	{
		TupArray<TupString> tokenizedParentString;
		TokenizeString::BuildArray(tupKeyValueContext.GetKeyAsString(parentAttribIndex),"\\",tokenizedParentString);

		const int INST_INFO_INDEX = 0;
		const int GROUP_NAME_INDEX = 1;
		const int PARENT_CHILD_INDEX = 2;
		const int KEY_NAME_INDEX = 3;

		if (tokenizedParentString.GetSize()!=4)
			continue;

		// make sure the first token matches
		if (strcmp(tokenizedParentString[INST_INFO_INDEX],"inst_info")!=0)
			continue;

		// make sure these are not empty strings
		if (tokenizedParentString[GROUP_NAME_INDEX].GetLength()==0 || tokenizedParentString[KEY_NAME_INDEX].GetLength()==0)
			continue;

		// make sure this is a parent
		if (strcmp(tokenizedParentString[PARENT_CHILD_INDEX],"parent_only_if_true")!=0)
			continue;

		// make sure the value is true
		if (!tupKeyValueContext.GetValueAsBool(parentAttribIndex))
			return;

		TupKeyValue newGroup;
		// add in the parent
		newGroup.AddKeyValue((const char *)tokenizedParentString[KEY_NAME_INDEX],tupKeyValueContext.GetValueAsString(parentAttribIndex));

		// try to add in the children
		for (int childAttribIndex=0;childAttribIndex<numKeyValues;childAttribIndex++)
		{
			if (childAttribIndex==parentAttribIndex)
				continue;

			TupArray<TupString> tokenizedChildString;
			TokenizeString::BuildArray(tupKeyValueContext.GetKeyAsString(childAttribIndex),"\\",tokenizedChildString);

			if (tokenizedChildString.GetSize()!=4)
				continue;
			
			// make sure the first token matches
			if (strcmp(tokenizedChildString[INST_INFO_INDEX],"inst_info")!=0)
				continue;

			// make sure the group name matches
			if (strcmp(tokenizedChildString[GROUP_NAME_INDEX],tokenizedParentString[GROUP_NAME_INDEX])!=0)
				continue;
			
			// make sure the key is not an empty string
			if (tokenizedChildString[KEY_NAME_INDEX].GetLength()==0)
				continue;
			
			// make sure this is a child
			if (strcmp(tokenizedChildString[PARENT_CHILD_INDEX],"child")!=0)
				continue;

			// make sure the value is not an empty string
			if (strlen(tupKeyValueContext.GetValueAsString(childAttribIndex))==0)
				continue;

			// append the child
			newGroup.AddKeyValue((const char *)tokenizedChildString[KEY_NAME_INDEX],tupKeyValueContext.GetValueAsString(childAttribIndex));
		}
		// new group
		groupData.Add(newGroup);
	}
}

void ExportInstanceAttributes::WriteGroupData(const TupArray<TupKeyValue> &groupData,int instanceIndex)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	TupString instanceIndexString("instance_index");
	char indexString[10];
	itoa(instanceIndex,indexString,10);

	int numGroups = groupData.GetSize();
	for (int groupIndex=0;groupIndex<numGroups;groupIndex++)
	{
		// write the instance index information
		DBLWrite::WriteData(instanceIndexString,instanceIndexString.GetLength()+1,pDBLFile);
		DBLWrite::WriteData(indexString,strlen(indexString)+1,pDBLFile);

		// write the key value pairs (parent is first)
		const TupKeyValue &keyValue = groupData[groupIndex];
		int numKeyValues = keyValue.GetNumAttribs();
		for (int keyValueIndex=0;keyValueIndex<numKeyValues;keyValueIndex++)
		{
			DBLWrite::WriteData(keyValue.GetKeyAsString(keyValueIndex),strlen(keyValue.GetKeyAsString(keyValueIndex))+1,pDBLFile);
			DBLWrite::WriteData(keyValue.GetValueAsString(keyValueIndex),strlen(keyValue.GetValueAsString(keyValueIndex))+1,pDBLFile);
		}
	}
}
