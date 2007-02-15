////////////////////////////////////////////////////////////////////////////
//
// ExportInstance
//
// Export Instances into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportInstance.cpp $
 * 
 * *****************  Version 62  *****************
 * User: Adam Clayton Date: 8/05/03    Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 61  *****************
 * User: Adam Clayton Date: 8/05/03    Time: 11:15a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 60  *****************
 * User: Adam Clayton Date: 8/05/03    Time: 11:07a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * fadedistance
 * 
 * *****************  Version 59  *****************
 * User: Nmartin      Date: 8/02/03    Time: 2:48p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * had "additive_render" string in wrong place!
 * 
 * *****************  Version 58  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:26a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 57  *****************
 * User: Nmartin      Date: 7/25/03    Time: 4:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * add additive render setting for instances
 * 
 * *****************  Version 56  *****************
 * User: Adam Clayton Date: 7/24/03    Time: 12:22p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 55  *****************
 * User: Dwain Skinner Date: 6/25/03    Time: 7:46a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 54  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 5:55p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 53  *****************
 * User: Adam Clayton Date: 6/04/03    Time: 9:58a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * use base mesh to determine alpha (oops)
 * 
 * *****************  Version 52  *****************
 * User: Adam Clayton Date: 5/28/03    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * gamecube 16 bit colors
 * 
 * *****************  Version 51  *****************
 * User: Dwain Skinner Date: 5/20/03    Time: 5:40p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 50  *****************
 * User: Dwain Skinner Date: 5/02/03    Time: 11:11a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 49  *****************
 * User: Dwain Skinner Date: 4/30/03    Time: 11:03a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 48  *****************
 * User: Dwain Skinner Date: 4/29/03    Time: 2:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 47  *****************
 * User: Adam Clayton Date: 4/29/03    Time: 1:49p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 46  *****************
 * User: Dwain Skinner Date: 4/25/03    Time: 4:12p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 45  *****************
 * User: Dwain Skinner Date: 4/25/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 44  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:22p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * get bounding from bounding filter
 * 
 * *****************  Version 43  *****************
 * User: Dwain Skinner Date: 4/04/03    Time: 11:20a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 42  *****************
 * User: Dwain Skinner Date: 3/29/03    Time: 6:06p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 41  *****************
 * User: Dwain Skinner Date: 3/25/03    Time: 1:27p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 40  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 39  *****************
 * User: Dwain Skinner Date: 2/27/03    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 38  *****************
 * User: Adam Clayton Date: 2/13/03    Time: 9:47a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * warn as well as truncate instance names that are too long
 * 
 * *****************  Version 37  *****************
 * User: Adam Clayton Date: 1/14/03    Time: 4:05p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * added animated texture playback setting on instances
 * 
 * *****************  Version 36  *****************
 * User: Adam Clayton Date: 10/17/02   Time: 10:45a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * no receive shadows
 * 
 * *****************  Version 35  *****************
 * User: Adam Clayton Date: 10/08/02   Time: 12:23p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * texture animation speed fix
 * 
 * *****************  Version 34  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 3:24p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * render vs collision instances
 * 
 * *****************  Version 33  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 12:24p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * sourceAlpha
 * 
 * *****************  Version 32  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 12:21p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Vertex color alpha support
 * 
 * *****************  Version 31  *****************
 * User: Adam Clayton Date: 9/04/02    Time: 4:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * master instance has no instance index fatal added
 * 
 * *****************  Version 30  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 12:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Texture Animation Complete (_ATX) Method
 * 
 * *****************  Version 29  *****************
 * User: Adam Clayton Date: 7/26/02    Time: 4:25p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * texture animation complete (almost)
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 7/15/02    Time: 3:45p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * World Emitters
 * Config tree change for named points
 * 
 * *****************  Version 27  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 4:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth SOM start
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 6/21/02    Time: 8:39a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * render last layer
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 6/18/02    Time: 3:03p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 4:23p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * took out a few throws
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 5/22/02    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * No zwrite flag
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 5/20/02    Time: 8:44a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Instance specific GCN code for vertex colors
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 5/16/02    Time: 12:43p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Note for gamecube and 32 bit vertex colors
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 5/13/02    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * If there are no vertex colors on a mesh generate one that is white for
 * the gamecube
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 5/10/02    Time: 2:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * PS2_EXPORT define changed to EXPORT_PS2 
 * LIghting checked on meshes
 * can handle node with no material
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 5/07/02    Time: 2:38p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * PS2- setting color set value 
 * PS2 - Does not output vertex colors on the instance (rely on the dma
 * som stuff)
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 5/02/02    Time: 4:05p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * warning for non standard materials within a multi-sub object material
 * 2 sided materials
 * hard coded vertex colors to 32 bit for now
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * remove instance name from config tree (use the node name only)
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 9:10a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Environmental instances are not added to scene tree
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 8:52a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 16 bit vertex colors
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * First pass of instance flags accessing node attribute data
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * hybrid configuration
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:56p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed to use Annotate filter
 * Added boundign and annotate helper references
 * Removed export pass
 * Added Dependencies
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/08/02    Time: 3:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * memset problem
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/07/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * UV Offsets and Tiling now altering the uvs within the model
 * Pushed pools up to top of export dbl 
 * Renamed all inserted tupperware atoms to ExportPC - 
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/06/02    Time: 4:44p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Vertex Color Support (hold vc modifier too)
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/06/02    Time: 2:38p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Vertex Colors working with vertex paint and applyvc
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 2:48p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instances that point to a Derived object working properly
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

#include "ExportPch.h"
#include "ExportInstance.h"

InstanceHolder::InstanceHolder(void) :
		m_instanceFilePos(0),
		m_instanceVertexColorFilePos(0),
		m_vertexColorInstanceIndex(-1),
		m_pNode(NULL)
{
	m_pInstanceBuffer = new CInstance;
	memset(m_pInstanceBuffer,0,sizeof(CInstance));
}

InstanceHolder::~InstanceHolder()
{
	delete m_pInstanceBuffer;
}

void InstanceHolder::SetInstanceFilePos(u32 instanceFilePos) 
{
	m_instanceFilePos = instanceFilePos;
}

void InstanceHolder::SetInstanceVertexColorFilePos(u32 instanceVertexColorFilePos)
{
	m_instanceVertexColorFilePos = instanceVertexColorFilePos;
}

void InstanceHolder::SetNodeAgg(TupperwareAggregate *pNode)
{
	m_pNode = pNode;
}

void InstanceHolder::SetVertexColorInstanceIndex(s32 index)
{
	m_vertexColorInstanceIndex = index;
}

u32 InstanceHolder::GetInstanceFilePos(void)
{
	return m_instanceFilePos;
}

u32 InstanceHolder::GetInstanceVertexColorFilePos(void)
{
	return m_instanceVertexColorFilePos;
}

s32 InstanceHolder::GetVertexColorInstanceIndex()
{
	return m_vertexColorInstanceIndex;
}

TupperwareAggregate *InstanceHolder::GetNodeAgg(void)
{
	return m_pNode;
}

CInstance &InstanceHolder::GetInstanceBuffer(void)
{
	return *m_pInstanceBuffer;
}


ExportInstance::ExportInstance(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager), m_tupConfigRoot(tupConfigRoot)
{
}

enum eInstanceAttributeKeys
{
	DISPLAY_LAYER,
	RENDER_FIRST_LAYER,
	RENDER_LAST_LAYER,
	HIDDEN,
	DO_NOT_FOG,
	DO_NOT_FAR_CLIP,
	NO_ZWRITE,
	MAKE_DYNAMIC_TEXTURE_SET,
	BILLBOARD,
	BILLBOARD3D,
	USE_OVERRIDE_COLOR,
	OVERRIDE_COLOR,
	VISIBLE_INTERNALS,
	NO_RECEIVE_SHADOWS,
	ANIMATED_TEXTURE_PLAYBACK_SPEED,
	ANIMATED_TEXTURE_PLAYBACK_SPEED_VALUE,
	ENV_TYPE,
	ADDITIVE_RENDER,
	SIMPLE_FADEOUT_DISTANCE,
	SORT_BIAS,
	NUM_INSTANCE_KEYS,
};

static char *InstanceAttributeKeys[NUM_INSTANCE_KEYS] =
{
	"display_layer",										// DISPLAY_LAYER,
	"render_first_layer",								// RENDER_FIRST_LAYER,
	"render_last_layer",									// RENDER_LAST_LAYER,
	"instance_hidden",									// HIDDEN,
	"instance_no_fog",									// DO_NOT_FOG,
	"instance_no_far_clip",								// DO_NOT_FAR_CLIP,
	"instance_no_zwrite",								// NO_ZWRITE,
	"dynamic_texture_set",								// MAKE_DYNAMIC_TEXTURE_SET,
	"billboard",											// BILLBOARD,
	"billboard3D",											// BILLBOARD3D,
	"override_color_enable",							// USE_OVERRIDE_COLOR,
	"override_color",										// OVERRIDE_COLOR,
	"visible_internals",									// VISIBLE_INTERNALS,
	"no_receive_shadows",								// NO_RECEIVE_SHADOWS,
	"animated_texture_playback_speed",				// ANIMATED_TEXTURE_PLAYBACK_SPEED,
	"animated_texture_playback_speed_value",		// ANIMATED_TEXTURE_PLAYBACK_SPEED_VALUE,
	"env_type",												// ENV_TYPE,
	"additive_render",									// ADDITIVE_RENDER,
	"simple_fadeout_distance",							// SIMPLE_FADEOUT_DISTANCE,
	"sort_bias",											// SORT_BIAS,
};

// write out an instance
void ExportInstance::WriteInstance(InstanceHolder *pInstanceHolder,int instanceIndex,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool)
{
	CInstance* dummy = (CInstance*) 0;
	if (!dummy->VerifyAlignment())
		throw(PluginException(ERROR_FATAL,"CInstance has alignment problems"));
	if (!IS_VU_ALIGNED(sizeof(CInstance)))
		throw(PluginException(ERROR_FATAL,"CInstance struct size isn't aligned (exceeding %d boundary by %d bytes)", VU_ALIGN, sizeof(CInstance)%VU_ALIGN));

	FILE *pDBLFile = DBLUtil::GetDBLFile();
	InstanceHolder &instanceHolder = pInstanceHolder[instanceIndex];
	CInstance &instance = instanceHolder.GetInstanceBuffer();

	instanceHolder.SetInstanceFilePos(ftell(pDBLFile));

	instance.Reset();

	TupperwareAggregate *pNodeAgg = instanceHolder.GetNodeAgg();
	TupNode tupNode(pNodeAgg);

	// which parition we are in
	int currentPartition = pNodeAgg->FindScalarByKey("Annotate_RenderPartitionIndex")->GetAsInt();

	// check and set flags based on instance attributes
	// get my attributes from the node
	char *pNodeAttributesString;
	tupNode.GetUserProperties(&pNodeAttributesString);
	TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
	TupKeyValueContext tupKeyValueContext;
	// get all key/value pairs for the instance
	m_tupConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,true);
	int keyIndex;

	// include name for use in attaching to .oa file data
	char *pName;
	tupNode.GetName(&pName);
	instance.SetName(pName);
	if ( strcmp( instance.Name(), pName ) != 0 )
	{
		// name is too long, this could be troublesome for environmental object lookups.
		keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[ENV_TYPE]);
		// if not none then it is an environmental instance
		if (keyIndex!=-1 && strcmp(tupKeyValueContext.GetValueAsString(keyIndex),"None")!=0)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Instance Name \"%s\" exceeds 31 characters.  Truncating to \"%s\".",pName,instance.Name());
		}
	}

	// get lighting value from instance aggregate
	bool bALit = pNodeAgg->FindScalarByKey("RealTimeLighting_Ambient")->GetAsInt() ? true : false;
	bool bPLit = pNodeAgg->FindScalarByKey("RealTimeLighting_Point")->GetAsInt() ? true : false;
	bool bDLit = pNodeAgg->FindScalarByKey("RealTimeLighting_Directional")->GetAsInt() ? true : false;
	bool bVLit = pNodeAgg->FindScalarByKey("RealTimeLighting_Vertex")->GetAsInt() ? true : false;
	float lightingAttenuationScale = pNodeAgg->FindScalarByKey("RealTimeLighting_AttenuationScale")->GetAsFloat();
	instance.SetLightAttenuationScale(lightingAttenuationScale);

	// set assigned lights (-1 if not assigned)
	int pointLights[2];
	pointLights[0] = pointLights[1] = -1;
	TupperwareList *pLightsList = pNodeAgg->FindListByKey("RealTimeLighting_Lights");
	if (pLightsList && pLightsList->GetLength()>0)
	{
		int numLights = pLightsList->GetLength();
		int *pLights = pLightsList->GetAsInt();
		if (numLights>2)
			numLights=2;
		for (int lightIndex=0;lightIndex<numLights;lightIndex++)
		{
			pointLights[lightIndex] = pLights[lightIndex];
		}
	}
	instance.SetPreAssignedLights(pointLights);

	u32 lightingBits = ( INSTANCE_LIGHTING_ENABLED | INSTANCE_LIGHTING_CACHED );
	if (bALit)
	{
		lightingBits |= INSTANCE_LIGHTING_AMBIENT;
	}
	if (bPLit)
	{
		lightingBits |= INSTANCE_LIGHTING_POINT;
	}
	if (bDLit)
	{
		lightingBits |= INSTANCE_LIGHTING_DIRECTIONAL;
	}
	if (bVLit)
	{
		lightingBits |= INSTANCE_LIGHTING_VERTEX;
	}
	instance.SetDynamicLightFlags( lightingBits );

	// check the display layer
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[DISPLAY_LAYER]);
	if (keyIndex==-1) throw(PluginException(ERROR_FATAL,"Display layer key not available"));
	if (!strcmp(tupKeyValueContext.GetValueAsString(keyIndex),"normal"))
	{
		// default is 0 do nothing here
	}
	else if (!strcmp(tupKeyValueContext.GetValueAsString(keyIndex),"alpha_sort"))
	{
		instance.SetAlphaSort( true );
	}
	else if (!strcmp(tupKeyValueContext.GetValueAsString(keyIndex),"render_last"))
	{
		keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[RENDER_LAST_LAYER]);
		if (keyIndex==-1) throw(PluginException(ERROR_FATAL,"Render last layer key not available"));
		int renderLastLayer = tupKeyValueContext.GetValueAsInt(keyIndex);
		if (renderLastLayer<1 || renderLastLayer>7) throw(PluginException(ERROR_FATAL,"Render last layer not within bounds - is %d",renderLastLayer));
		instance.SetRenderBucket( renderLastLayer );
	}
	else if (!strcmp(tupKeyValueContext.GetValueAsString(keyIndex),"render_first"))
	{
		keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[RENDER_FIRST_LAYER]);
		if (keyIndex==-1) throw(PluginException(ERROR_FATAL,"Render first layer key not available"));
		int renderFirstLayer = tupKeyValueContext.GetValueAsInt(keyIndex);
		if (renderFirstLayer<1 || renderFirstLayer>7) throw(PluginException(ERROR_FATAL,"Render first layer not within bounds - is %d",renderFirstLayer));
		instance.SetRenderBucket( -renderFirstLayer );
	}

	// check the hidden flag
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[HIDDEN]);
	if (keyIndex!=-1 && tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		instance.Hide( true );
	}

	// check the no fog flag
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[DO_NOT_FOG]);
	if (keyIndex!=-1 && tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		instance.SetFogDisabled( true );
	}

	// check the no far clip flag
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[DO_NOT_FAR_CLIP]);
	if (keyIndex!=-1 && tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		instance.SetFarClipDisabled( true );
	}

	// check the no zwrite flag
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[NO_ZWRITE]);
	if (keyIndex!=-1 && tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		instance.SetZWriteDisabled( true );
	}

	// check the no receive shadows
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[NO_RECEIVE_SHADOWS]);
	if (keyIndex!=-1 && tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		instance.SetReceiveShadows( false );
	}

	// check the additive render
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[ADDITIVE_RENDER]);
	if (keyIndex!=-1 && tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		instance.SetAdditiveBlend( true );
	}

	// fade distance
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[SIMPLE_FADEOUT_DISTANCE]);
	instance.SetFadeOutDistance( INSTANCE_NO_FADE_DISTANCE );	// default is to not fade
	if (keyIndex!=-1)
	{
		instance.SetFadeOutDistance( tupKeyValueContext.GetValueAsFloat(keyIndex) );
	}

	// sort bias
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[SORT_BIAS]);
	instance.SetSortBias( 0.0f );
	if (keyIndex!=-1)
	{
		instance.SetSortBias( tupKeyValueContext.GetValueAsFloat(keyIndex) );
	}

	// check the billboard flag
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[BILLBOARD]);
	if (keyIndex!=-1 && tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		instance.SetBillboard( true );
	}

	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[BILLBOARD3D]);
	if (keyIndex!=-1 && tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		instance.SetBillboard3D( true );
	}

	// check the override color
	keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[USE_OVERRIDE_COLOR]);
	if (keyIndex==-1) throw(PluginException(ERROR_FATAL,"Use Override Color key not available"));
	if (tupKeyValueContext.GetValueAsBool(keyIndex))
	{
		keyIndex = tupKeyValueContext.KeyExistsAt(InstanceAttributeKeys[OVERRIDE_COLOR]);
		if (keyIndex==-1) throw(PluginException(ERROR_FATAL,"Override Color key not available"));
		TupAttribColor tupAttribColor = tupKeyValueContext.GetValueAsColor(keyIndex);
//		instancePart2.OverrideColor.R = (u8)tupAttribColor.m_nRed;
//		instancePart2.OverrideColor.G = (u8)tupAttribColor.m_nGreen;
//		instancePart2.OverrideColor.B = (u8)tupAttribColor.m_nBlue;
//		instancePart2.OverrideColor.A = 255;
	}
	else
	{
		// if not set then use white
//		instancePart2.OverrideColor.R = 255;
//		instancePart2.OverrideColor.G = 255;
//		instancePart2.OverrideColor.B = 255;
//		instancePart2.OverrideColor.A = 255;
	}

	// get my index within the node pool
	int nodeReference = tupNode.GetNodeReference();
	int objectRef;
	tupNode.GetObjectReference(objectRef);
	assert(objectRef>=0);
	TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectRef);
	assert(pObjectAgg);

#ifdef EXPORT_GCN
	// on the GameCube we use 16 bit colors (565) unless some alpha is used on the mesh
	// in this case we use 32 bit colors (RGBA)
	bool bFoundAlpha=false;
	TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(tupObjectPool,objectRef);
	assert(pBaseMeshAgg);
	TupperwareList *pVertexAlphaRefList = pBaseMeshAgg->FindListByKey("Annotate_VertexAlphaRefList");
	if (pVertexAlphaRefList)
	{
		int numColorSets = pVertexAlphaRefList->GetLength();
		int *pVertexAlphaRefData = pVertexAlphaRefList->GetAsInt();
		for (int setIndex=0;setIndex<numColorSets;setIndex++)
		{
			if (pVertexAlphaRefData[setIndex] !=-1)
			{
				bFoundAlpha = true;
				break;
			}
		}
	}
	if (bFoundAlpha)
		instance.SetVertexColor32Bit( true );
#else
	instance.SetVertexColor32Bit( true );
#endif

	TupArray<int> parentNodeReferenceList;
	pObjectAgg->FindListByKey("Annotate_ParentNodeReferenceList")->GetAsInt(parentNodeReferenceList);
	int numEntries = parentNodeReferenceList.GetSize();

	// try to find another instance that is using the same vertex colors and is before me in the parent node reference list on the mesh
	// it also needs to be using the same partition
	int instanceMaster = -1;
	for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
	{
		int tryRef = parentNodeReferenceList[entryIndex];
		// stop if it gets to me
		if (tryRef==nodeReference)
			break;
		TupperwareAggregate *pTryNodeAgg = tupNodePool.GetNode(tryRef);

		TupperwareScalar *pRenderPartitionIndexScalar = pTryNodeAgg->FindScalarByKey("Annotate_RenderPartitionIndex");
		if (pRenderPartitionIndexScalar)
		{
			int renderPartitionIndex = pRenderPartitionIndexScalar->GetAsInt();
			// only use ones that are in the same partition
			if (renderPartitionIndex-=currentPartition)
			{
				// use this one
				instanceMaster = pTryNodeAgg->FindScalarByKey("Annotate_RenderInstanceIndex")->GetAsInt();
				break;
			}
		}
	}
	instanceHolder.SetVertexColorInstanceIndex( instanceMaster );

	int somIndex = pNodeAgg->FindScalarByKey("Annotate_SOMIndex")->GetAsInt();
	instance.SetModel( (ts_PatchSOM*) somIndex );	// this will be fixed at load time.

	instance.GetTextureAnimatorArray() = NULL;
 
	// for now we have no vertex color information
	instance.SetVertexColorPtr( 0 );

	// Get Transformation information
	TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
	TupTransform tupTransform(pTransformAgg);
	if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
	{
		TupStaticMatrix tupStaticMatrix(pTransformAgg);
		float *pMatrix;
		tupStaticMatrix.GetMatrix(&pMatrix);

		instance.Matrix() = Graphics4x4(
			pMatrix[0], pMatrix[1], pMatrix[2],
			pMatrix[4], pMatrix[5], pMatrix[6],
			pMatrix[8], pMatrix[9], pMatrix[10],
			pMatrix[12], pMatrix[13], pMatrix[14]);      // translation
	}

	// Get Bounding information
	instance.BoundingBox().Min = *((Vector3 *)instanceHolder.GetNodeAgg()->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat());
	instance.BoundingBox().Max = *((Vector3 *)instanceHolder.GetNodeAgg()->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat());
	instance.BoundingSphere().Center = *((Vector3 *)instanceHolder.GetNodeAgg()->FindListByKey("Bounding_BoundingSphereCenter")->GetAsFloat());
	instance.BoundingSphere().Radius = instanceHolder.GetNodeAgg()->FindScalarByKey("Bounding_BoundingSphereRadius")->GetAsFloat();

#if defined(EXPORT_PS2)  ||  defined(EXPORT_DX)
	instance.SetColorSet( instanceHolder.GetNodeAgg()->FindScalarByKey("Annotate_MeshVertexColorAlphaOffset")->GetAsInt() );
#endif

	// mark new record list entry
	DBLUtil::RecordListEntry();

	// write out instance
	DBLWrite::WriteRecord(&instance, pDBLFile);

	// alignment
	DBLUtil::AlignFile();
}

void ExportInstance::WriteInstanceVertexColors(InstanceHolder *pInstanceHolder,int instanceIndex,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	int realTimeLit = false;

	InstanceHolder &instanceHolder = pInstanceHolder[instanceIndex];
	CInstance &instance = instanceHolder.GetInstanceBuffer();
	if (instanceHolder.GetVertexColorInstanceIndex() >= 0)
	{
		return; // if I am a slave then don't save my colors
	}
	TupperwareAggregate *pNodeAgg = instanceHolder.GetNodeAgg();
	TupNode tupNode(pNodeAgg);
	int objectRef;	
	tupNode.GetObjectReference(objectRef);
	assert(objectRef!=-1);
	TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(tupObjectPool,objectRef);
	assert(pBaseMeshAgg);
	TupperwareAggregate *pChildVertexColorMeshMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(tupObjectPool,objectRef);
	TupperwareAggregate *pChildVertexAlphaMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(tupObjectPool,objectRef);

	// if there is no vertex colors and alpha then don't do this
	if (!pChildVertexColorMeshMapAgg && !pChildVertexAlphaMeshMapAgg)
	{
		return;
	}

	bool bALit = pBaseMeshAgg->FindScalarByKey("RealTimeLighting_Ambient")->GetAsInt() ? true : false;
	bool bPLit = pBaseMeshAgg->FindScalarByKey("RealTimeLighting_Point")->GetAsInt() ? true : false;
	bool bDLit = pBaseMeshAgg->FindScalarByKey("RealTimeLighting_Directional")->GetAsInt() ? true : false;

	// if real time lit but not wanting vertex colors then stop
	if (bALit || bPLit || bDLit)
	{
		bool bNeedsVertexColors = pBaseMeshAgg->FindScalarByKey("RealTimeLighting_Vertex")->GetAsInt() ? true : false;
		if (!bNeedsVertexColors)
			return;
	}

	// we have vertex colors or alpha
	// save the offset into the file
	instanceHolder.SetInstanceVertexColorFilePos(ftell(pDBLFile));
	TupTriMesh tupTriMesh(pBaseMeshAgg);
	// get the number of faces in the base mesh
	int *pPositionIndices;
	int numFaces;
	tupTriMesh.GetPositionIndices( &pPositionIndices, numFaces );
	int *pFaceFlags;
	tupTriMesh.GetFaceFlags( &pFaceFlags, numFaces );

	Vector3 *pVertexColorVerts=NULL;
	int *pVertexColorIndices=NULL;
	int numVertexColorVerts=0;
	int numVertexColorFaces=0;

	Vector3 *pVertexAlphaVerts=NULL;
	int *pVertexAlphaIndices=NULL;
	int numVertexAlphaVerts=0;
	int numVertexAlphaFaces=0;

	// get the vertex color values (if there)
	if (pChildVertexColorMeshMapAgg)
	{
		TupTriMeshMap tupTriMeshMap(pChildVertexColorMeshMapAgg);
		tupTriMeshMap.GetMapVerts((float **) &pVertexColorVerts, numVertexColorVerts );
		tupTriMeshMap.GetMapIndices( &pVertexColorIndices, numVertexColorFaces );
		assert(numVertexColorFaces==numFaces);
	}

	// get the vertex alpha values (if there)
	if (pChildVertexAlphaMeshMapAgg)
	{
		TupTriMeshMap tupTriMeshMap(pChildVertexAlphaMeshMapAgg);
		tupTriMeshMap.GetMapVerts((float **) &pVertexAlphaVerts, numVertexAlphaVerts );
		tupTriMeshMap.GetMapIndices( &pVertexAlphaIndices, numVertexAlphaFaces );
		assert(numVertexAlphaFaces==numFaces);
	}

	TupperwareAggregate *pTriangleStripsAgg = tupTriMesh.GetTriangleStrips();
	if (!pTriangleStripsAgg)
		throw(PluginException(ERROR_FATAL,"Cannot find TriStrips"));
	TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
	int totalStrips = tupTriangleStripPool.GetNumTriangleStrips();
	for (int stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		TupperwareAggregate *pTriangleStripAgg = tupTriangleStripPool.GetTriangleStrip(stripIndex);
		assert(pTriangleStripAgg);
		TupTriangleStrip tupTriangleStrip(pTriangleStripAgg);
		int *pFaceIndices;
		int *pFacePointIndices;
		int numFaceIndices;
		int numFacePointIndices;
		tupTriangleStrip.GetFaceIndices(&pFaceIndices,numFaceIndices);
		assert(numFaceIndices);
		tupTriangleStrip.GetFacePointIndices(&pFacePointIndices,numFacePointIndices);
		assert(numFacePointIndices);
		assert(numFaceIndices==numFacePointIndices);

		// check if this tristrip is not a cloth strip
		if (pFaceFlags[pFaceIndices[0]]&TupTriMesh::FLAGS_FACE_USES_CLOTH)
			continue; // don't save these here

		// output vertex colors for the strip
		for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
		{
			int faceIndex = pFaceIndices[pointIndex];
			int facePointIndex = pFacePointIndices[pointIndex];

			ts_bRGBA destColor32;

			// if there are some colors
			if (pVertexColorVerts)
			{
				int vertexIndex = pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex];
				Vector3 &sourceColor = pVertexColorVerts[vertexIndex];
				destColor32.R = (u8)(sourceColor.x() *255.0f);
				destColor32.G = (u8)(sourceColor.y() *255.0f);
				destColor32.B = (u8)(sourceColor.z() *255.0f);
			}
			else // no vertex colors
			{
				destColor32.R = 255;
				destColor32.G = 255;
				destColor32.B = 255;
			}

			// if there are a vertex alpha value
			if (pVertexAlphaVerts)
			{
				int vertexIndex = pVertexAlphaIndices[(faceIndex<<1)+faceIndex+facePointIndex];
				Vector3 &sourceAlpha = pVertexAlphaVerts[vertexIndex];
				destColor32.A = (u8)(sourceAlpha.x() *255.0f); // just use the x value
			}
			else // no vertex colors
			{
				destColor32.A = 255;
			}

			if (instance.VertexColor32Bit())
			{
				DBLWrite::WriteRecord(&destColor32, pDBLFile);
			}
			else // 16 bit color
			{
				assert(false); // this should not happen since all should be 32 bit color
				u16 destColor16;

				if (destColor32.R <= 255 - 4)
					destColor32.R += 4;
				if (destColor32.G <= 255 - 4)
					destColor32.G += 4;
				if (destColor32.B <= 255 - 4)
					destColor32.B += 4;
				destColor16 = (destColor32.R >> 3) | ((destColor32.G >> 3) << 5) | ((destColor32.B >> 3) << 10);
				DBLWrite::WriteRecord(&destColor16, pDBLFile);
			}
		}
		DBLUtil::AlignChunkDataQuadWord();
	}
}

void ExportInstance::LinkInstancePasses(InstanceHolder *pInstanceHolder,int instanceIndex,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	InstanceHolder &instanceHolder = pInstanceHolder[instanceIndex];
	CInstance &instance = instanceHolder.GetInstanceBuffer();

	int masterInstanceIndex = instanceIndex;
	if (instanceHolder.GetVertexColorInstanceIndex() >= 0)
		masterInstanceIndex = instanceHolder.GetVertexColorInstanceIndex();

	/* get offset from this instance to its vertex colors */
	InstanceHolder &masterInstanceHolder = pInstanceHolder[masterInstanceIndex];
	CInstance &masterInstance = masterInstanceHolder.GetInstanceBuffer();

#ifndef EXPORT_PS2
	CInstance* dummy = (CInstance*) 0;
	fseek(pDBLFile, instanceHolder.GetInstanceFilePos() + offsetof(CInstance, d_vertexColors), SEEK_SET);
	u32 u32Offset;
	if (masterInstanceHolder.GetInstanceVertexColorFilePos() == 0)
		u32Offset = 0;	// no vertex colors
	else
		u32Offset = masterInstanceHolder.GetInstanceVertexColorFilePos() - instanceHolder.GetInstanceFilePos();
	DBLWrite::WriteRecord(&u32Offset, pDBLFile);
#endif

	// back to end of file
	fseek(pDBLFile, 0, SEEK_END);
}




