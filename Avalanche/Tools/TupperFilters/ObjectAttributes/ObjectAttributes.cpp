////////////////////////////////////////////////////////////////////////////
//
// ObjectAttributes
//
// Class to handle the ObjectAttributes Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "ObjectAttributesPch.h"
#include "ObjectAttributes.h"

// defines for the attribute block headers
#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK	"END OBJECTATTRIBUTES"

//============================================================================

int ObjectAttributesFilter::GetTotalPasses(void)	const
{
	return 1;
}

//============================================================================

char *ObjectAttributesFilter::m_readDependencyNames[NUM_READ_DEPENDENCIES] = 
{
		"Annotate_InstanceIndexes", // INSTANCE_INDEXES
};

//============================================================================

Revision ObjectAttributesFilter::m_readDependencyRevisions[NUM_READ_DEPENDENCIES] = 
{
	Revision(1,0), // INSTANCE_INDEXES
};

//============================================================================

int ObjectAttributesFilter::GetTotalReadDependencies(int pass,const FilterOptions &options) const
{
	return NUM_READ_DEPENDENCIES;
}

//============================================================================

const char *ObjectAttributesFilter::GetReadDependencyKey(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyNames[index];
}

//============================================================================

Revision ObjectAttributesFilter::GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyRevisions[index];
}

//============================================================================

void ObjectAttributesFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	// set up log
	PluginLog::SetLog(&m_log);
	TupString outputFilename(dataManager.GetOutputFilename());

	TupperwareAggregate *sceneAgg = dataManager.GetSceneRoot();
	try
	{
		// read any key exclusions
		TupString excludeKeys = options.GetValue("ExcludeKeys");
		if (excludeKeys && strlen(excludeKeys) > 0)
		{
			TupString relativePath,fullPath,name,ext;
         FilePath::GetPathsToFile(relativePath,fullPath,name,ext,excludeKeys,dataManager.GetConfigDirectory());
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Reading actor parm exclude keys file '%s'",fullPath);
			if (!m_exclusions.AddKeysFromFile(fullPath))
				throw(PluginException(ERROR_FATAL,"Error loading exlude keys file '%s'",fullPath));
		}

		// write the attribute data
		FilePath::ChangeExtension(outputFilename, ".AP" );
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Saving Actor Parameter File %s", outputFilename);
		if (!WriteActorParameterFile(sceneAgg, outputFilename))
			throw(PluginException(ERROR_FATAL,"Error writing %s", outputFilename));
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"ObjectAttributes - Process\r\n  %s",(const char *)(e)));
	}
}

//============================================================================

void ObjectAttributesFilter::GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray)
{
	PluginLog::SetLog(&m_log);
	TupString outputFilename(dataManager.GetOutputFilename());

	FilePath::ChangeExtension(outputFilename,".AP");
	filePathArray.AddFilePath(outputFilename,FilePathArray::FILE_OUTPUT|FilePathArray::FILE_ACCESS_FATAL);

	TupString excludeKeys = options.GetValue("ExcludeKeys");
	if (excludeKeys && strlen(excludeKeys) > 0)
	{
		TupString relativePath,fullPath,name,ext;
      FilePath::GetPathsToFile(relativePath,fullPath,name,ext,excludeKeys,dataManager.GetConfigDirectory());
		filePathArray.AddFilePath(fullPath);
	}
}

//============================================================================

static bool IsNumber( const char* string, float& value )
{
	const char* p;
	for ( p = string ; *p ; p++ )
		if ( !isdigit( *p ) && *p != '-' && *p != '.' && *p != 'e' )
			return false;
	if ( p == string )
		return false;

	sscanf( string, "%f", &value );
	return true;
}

//============================================================================

bool ObjectAttributesFilter::WriteActorParameterFile( TupperwareAggregate *pRoot, const char* outputFilename )
{
	FILE* fp;
	fp = fopen( outputFilename, "wb" );
	if ( !fp )
		return false;

	int nodeIndex;
	TupScene tupScene(pRoot);
	// build the tree
	TupperwareAggregate *pAttributeRoot = new TupperwareAggregate;
	TupAccessRoot tupAccessRoot(pAttributeRoot);

	TupConfigRoot tupConfigRoot;
	tupConfigRoot.BuildConfigTree( tupScene.GetNodeAttributeTree() );

	// save out instance attributes
	TupArray<int> totalInstancesArray;
	tupScene.GetBaseAggregate()->FindListByKey("Annotate_TotalRenderInstances")->GetAsInt(totalInstancesArray);
	int totalInstances = totalInstancesArray[0];
	// we only worry about the first partition
	if (totalInstances>0)
	{
		int instanceIndex;
		int *pInstanceReferences = new int[totalInstances];

		TupperwareAggregate *pNodePoolAgg = tupScene.GetNodePool();
		assert(pNodePoolAgg);
		TupNodePool tupNodePool(pNodePoolAgg);
		int totalNodes = tupNodePool.GetNumNodes();
		// go through the nodes and build the reference table for all the instances
		// the nodes and the instances may not be in sync
		for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
		{
			TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
			if (pNodeAgg)
			{
				TupperwareScalar *pRenderPartitionIndexScalar = pNodeAgg->FindScalarByKey("Annotate_RenderPartitionIndex");
				// must be a render instance
				if (!pRenderPartitionIndexScalar)
					continue;
				// must be in the 0 partition (world)
				int renderPartitionIndex = pRenderPartitionIndexScalar->GetAsInt();
				if (renderPartitionIndex!=0)
					continue;
				instanceIndex = pNodeAgg->FindScalarByKey("Annotate_RenderInstanceIndex")->GetAsInt();
				assert(instanceIndex<totalInstances);
				pInstanceReferences[instanceIndex] = nodeIndex;
			}
		}

		TupperwareAggregate *pInstanceAttributeRoot = tupAccessRoot.AddInstanceAttributes();
		TupAccessInstancePool tupAccessInstancePool(pInstanceAttributeRoot);
		
		for (instanceIndex = 0;instanceIndex<totalInstances;instanceIndex++)
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH,"instance %d/%d", instanceIndex, totalInstances);

			nodeIndex = pInstanceReferences[instanceIndex];
			TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
			TupNode tupNode(pNodeAgg);

			// get the attributes block for this instance
			char *pInstanceAttributesString;
			tupNode.GetUserProperties( &pInstanceAttributesString );
			TupKeyValue tupKeyValue( pInstanceAttributesString, BEGIN_BLOCK, END_BLOCK );
			TupKeyValueContext tupKeyValueContext;
			tupConfigRoot.GetKeyValueContexts( tupKeyValue, tupKeyValueContext, true );

			// don't output this unless we have a "class" key
			int classKeyIndex = tupKeyValueContext.KeyExistsAt( "class" );
			const char* classValue = ( classKeyIndex >= 0 ) ? tupKeyValueContext.GetKeyAsString( classKeyIndex ) : NULL;
			if ( !classValue || !classValue[0] || !stricmp( classValue, "none" ) )
				continue;

			m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH,"  class=%s", classValue );

			// print the instance name
			char* pNodeName;
			tupNode.GetName(&pNodeName);
			fprintf( fp, "[%s]\n", pNodeName );

			m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH,"  instance=%s", pNodeName );
			m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH,"  attribs=%d", tupKeyValueContext.GetNumAttribs() );

			// print the parameters
			for ( int k = 0 ; k < tupKeyValueContext.GetNumAttribs() ; k++ )
			{
				const char* key   = tupKeyValueContext.GetKeyAsString( k );
				const char* value = tupKeyValueContext.GetValueAsString( k );
				m_log.Add(LogFlags::VERBOSE_LEVEL_HIGH,"   '%s'='%s'", key, value );

				if ( strlen( value ) > 0 )
				{
					if ( ! m_exclusions.IsExcluded( key ) )
					{
						float fValue;
						if ( IsNumber( value, fValue ) )
						{
							// trim the number so it takes less space and is easier to parse
							char num[ 32 ], *p;
							sprintf( num, "%.7f", fValue );
							for ( p = num + strlen( num ) - 1 ; p >= num && *p == '0' ; )
								*p-- = 0;
							if ( p >= num && *p == '.' )
								*p-- = 0;

							fprintf( fp, "%s=%s\n", key, num );
						}
						else
						{
							fprintf( fp, "%s=%s\n", key, value );
						}
					}
				}
			}
			
			fprintf( fp, "\n" );
		}
		delete [] pInstanceReferences;
	}

	delete pAttributeRoot;

	fclose( fp );

	return true;
}

//============================================================================

void memSetOneShotNewName(char *pcName)
{
}

//============================================================================
