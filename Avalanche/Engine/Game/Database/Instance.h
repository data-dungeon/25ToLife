//============================================================================
//=
//= Instance.h -
//=   This file handles instances.  An instance is an occurence of a
//=   model in the world, with a transform matrix from model space space
//=   to world space, as well as other visual properties
//=
//============================================================================

#ifndef _INSTANCE_H
#define _INSTANCE_H

#include "Game/Database/Som.h"			// for DisplayListRecord field in CInstance
#include "Game/Misc/Bounds.h"				// for BoundingSphere, BoundingBox fields in CInstance
#include "Game/Database/World.h"			// for ts_Geometry
#include "platform/CompilerControl.h"	// for NAMED_SECTION

class DynamicObject;
class TextureAnimatorArray;
class CLightInfluences;
class CActor;
struct ts_DBChunkHdr;
struct ts_RecordList;
class CInstance;

//============================================================================

extern CInstance *pRenderInst;			// current instance being rendered

//============================================================================

#define INSTANCE_ARTIST_RENDER_BUCKET_MAX	8
#define INSTANCE_RENDER_BUCKET_MAX			20
#define INSTANCE_NO_FADE_DISTANCE			0.0f

//============================================================================
//= values for view flags

#define INSTANCE_VIEW_CLIPPED					0x1
#define INSTANCE_VIEW_CLIPPED_FAR_PLANE	0x2
#define INSTANCE_VIEW_FOGGED					0x4

//============================================================================
//= values for lighting bits

#define INSTANCE_LIGHTING_NONE				0x00
#define INSTANCE_LIGHTING_ENABLED			0x01
#define INSTANCE_LIGHTING_AMBIENT			0x02
#define INSTANCE_LIGHTING_DIRECTIONAL		0x04
#define INSTANCE_LIGHTING_POINT				0x08
#define INSTANCE_LIGHTING_VERTEX				0x10
#define INSTANCE_LIGHTING_CACHED				0x20
#define INSTANCE_LIGHTING_ALL					( INSTANCE_LIGHTING_ENABLED | INSTANCE_LIGHTING_AMBIENT | INSTANCE_LIGHTING_DIRECTIONAL | INSTANCE_LIGHTING_POINT )

//============================================================================
//= Instance Class

class CInstance
{
public:
	CInstance()
	{
		Reset();
	}

	~CInstance()
	{
	}

	void Destroy();

	void Render( float cameraZDist );
	void RenderShadow( Graphics4x4* localToCamera, float cameraZDist );
	void RenderOffScreen();

	void Reset()
	{
		memset( this, 0, sizeof(*this) );
      d_alpha = 1.0f;
      d_localToWorld.Identity();
      d_attenuationScale = 1.0f;
		d_preAssignedLights[0] = -1;
		d_preAssignedLights[1] = -1;
		d_name[0] = '\0';
	}

	const char* Name() const { return d_name; }
	void SetName( const char* i_name )
	{
		if ( i_name )
		{
			strncpy( d_name, i_name, sizeof(d_name) - 1 );
			d_name[ sizeof(d_name) - 1 ] = 0;
		}
	}

	inline Graphics4x4&           Matrix()                  { return d_localToWorld;   }
	inline ts_BoundingSphere&     BoundingSphere()          { return d_boundingSphere; }
	inline ts_BoundingBox&        BoundingBox()             { return d_boundingBox;    }
	ts_BoundingBox*					ModelBoundingBox();
	inline ts_DisplayListRecord&  DisplayList()             { return d_displayList;    }
	inline TextureAnimatorArray*& GetTextureAnimatorArray() { return d_textureAnimatorArray; }

	inline float	 Alpha() const             { return d_alpha;  }
	inline void		 SetAlpha( float alpha )   { d_alpha = alpha; }
	inline float*	 AlphaPtr()					   { return &d_alpha; }

	inline ts_Body* Body() const              { return d_body;   }
	inline void     SetBody( ts_Body* body )  { d_body = body;   }

	inline CActor*	 Owner() const             { return d_owner;  }
	inline void     SetOwner( CActor* owner ) { d_owner = owner; }

	inline u32		 ViewFlags() const			{ return d_viewFlags; }
	inline void		 SetViewFlags( u32 flags ) { d_viewFlags = flags; }

	inline float	 FadeOutDistance() const	{ return d_fadeOutDistance; }
	inline void		 SetFadeOutDistance( float dist ) { d_fadeOutDistance = dist; }

	inline float    SortBias() const          { return d_sortBias; }
	inline void     SetSortBias( float bias ) { d_sortBias = bias; }

	inline void*	 VertexColorPtr() const { return d_vertexColors; }
	inline void		 SetVertexColorPtr( void* vertexColors ) { d_vertexColors = vertexColors; }

	inline int		 ColorSet() const            { return d_colorSet;     }
	inline void		 SetColorSet( int colorSet ) { d_colorSet = colorSet; }

	inline CLightInfluences* DynamicLightInfluences() const { return d_curLightInfluences; }
	inline void		 SetDynamicLightInfluences( CLightInfluences* influences ) { d_curLightInfluences = influences; }

	inline float	 LightAttenuationScale() const { return d_attenuationScale; }
	inline void		 SetLightAttenuationScale( float scale ) { d_attenuationScale = scale; }

	inline const int*	PreAssignedLights() const { return d_preAssignedLights; }
	inline void		 SetPreAssignedLights( int *lights ) { d_preAssignedLights[0] = lights[0]; d_preAssignedLights[1] = lights[1]; }

	inline ts_PatchSOM* Model() const { return d_SOM; }
	inline void		 SetModel( ts_PatchSOM* som ) { d_SOM = som; }

	inline DynamicObject* DynamicModel() const { return d_dynamicObject; }
	inline void		 SetDynamicModel( DynamicObject* model ) { d_dynamicObject = model; }

	inline u32		 UniqueID() const      { return d_uniqueID; }
	inline void		 SetUniqueID( u32 id ) { d_uniqueID = id; }

	inline bool     Hidden() const { return ( d_flags & INSTANCE_HIDDEN ) != 0; }
   inline void     Hide( bool flag )
	{
		if ( flag ) d_flags |=  INSTANCE_HIDDEN;
		else        d_flags &= ~INSTANCE_HIDDEN;
   }

	inline bool		 ReceiveShadows() const { return ( d_flags & INSTANCE_NO_RECEIVE_SHADOWS ) == 0; }
	inline void		 SetReceiveShadows( bool flag )
	{
		if ( flag )	d_flags &= ~INSTANCE_NO_RECEIVE_SHADOWS;
		else        d_flags |=  INSTANCE_NO_RECEIVE_SHADOWS;
	}

	inline bool		 AlphaSort() const { return ( d_flags & INSTANCE_ALPHA_SORT ) != 0; }
	inline void		 SetAlphaSort( bool flag )
		{
			if ( flag ) d_flags |=  INSTANCE_ALPHA_SORT;
			else        d_flags &= ~INSTANCE_ALPHA_SORT;
		}

	inline bool	    ZWriteDisabled() const { return ( d_flags & INSTANCE_NO_ZWRITE ) != 0; }
	inline void		 SetZWriteDisabled( bool flag )
	{
		if ( flag ) d_flags |=  INSTANCE_NO_ZWRITE;
		else        d_flags &= ~INSTANCE_NO_ZWRITE;
	}

	inline bool	    FogDisabled() const { return ( d_flags & INSTANCE_NO_FOG ) != 0; }
	inline void		 SetFogDisabled( bool flag )
	{
		if ( flag ) d_flags |=  INSTANCE_NO_FOG;
		else        d_flags &= ~INSTANCE_NO_FOG;
	}

	inline bool		 AdditiveBlend() const { return ( d_flags & INSTANCE_ADDITIVE_BLEND ) != 0; }
	inline void     SetAdditiveBlend( bool flag )
	{
		if ( flag ) d_flags |=  INSTANCE_ADDITIVE_BLEND;
		else        d_flags &= ~INSTANCE_ADDITIVE_BLEND;
	}

	inline bool		 Billboard() const { return ( d_flags & INSTANCE_BILLBOARD ) != 0; }
	inline void     SetBillboard( bool flag )
	{
		if ( flag ) d_flags |=  INSTANCE_BILLBOARD;
		else        d_flags &= ~INSTANCE_BILLBOARD;
	}

	inline bool		 Billboard3D() const { return ( d_flags & INSTANCE_BILLBOARD3D ) != 0; }
	inline void     SetBillboard3D( bool flag )
	{ 
		if ( flag ) d_flags |=  INSTANCE_BILLBOARD3D;
		else        d_flags &= ~INSTANCE_BILLBOARD3D;
	}

	inline bool		 FarClipDisabled() const { return ( d_flags & INSTANCE_NO_FAR_CLIP ) != 0; }
	inline void     SetFarClipDisabled( bool flag )
	{
		if ( flag ) d_flags |=  INSTANCE_NO_FAR_CLIP;
		else        d_flags &= ~INSTANCE_NO_FAR_CLIP;
	}

	inline bool		 ClockwiseVertices() const { return ( d_flags & INSTANCE_CLOCKWISE_VERTICES ) != 0; }
	inline void     SetClockwiseVertices( bool flag )
	{
		if ( flag ) d_flags |=  INSTANCE_CLOCKWISE_VERTICES;
		else        d_flags &= ~INSTANCE_CLOCKWISE_VERTICES;
	}

	inline bool		 DynamicallyLit() const
	{
		return ( d_lightingBits & INSTANCE_LIGHTING_ENABLED ) &&
			    ( d_lightingBits & ( INSTANCE_LIGHTING_POINT | INSTANCE_LIGHTING_DIRECTIONAL | INSTANCE_LIGHTING_AMBIENT ) );
	}
	inline void		 SetDynamicallyLit( bool flag )
	{
		if ( flag )  d_lightingBits = INSTANCE_LIGHTING_ALL;
		else         d_lightingBits = INSTANCE_LIGHTING_NONE;
	}

	inline bool		VertexColor32Bit() const { return ( d_flags & INSTANCE_VERTEX_COLOR_32BIT ) != 0; }
	inline void		SetVertexColor32Bit( bool flag )
	{
		if ( flag ) d_flags |=  INSTANCE_VERTEX_COLOR_32BIT;
		else        d_flags &= ~INSTANCE_VERTEX_COLOR_32BIT;
	}

	inline u32      DynamicLightFlags() const { return d_lightingBits; }
	void				 SetDynamicLightFlags( u32 bits )
	{
		if ( bits & ( INSTANCE_LIGHTING_POINT | INSTANCE_LIGHTING_DIRECTIONAL | INSTANCE_LIGHTING_AMBIENT | INSTANCE_LIGHTING_VERTEX ) )
			bits |= INSTANCE_LIGHTING_ENABLED;
		else
			bits &= ~INSTANCE_LIGHTING_ENABLED;
		d_lightingBits = bits;
	}

	inline s32  RenderBucket() const          { return d_renderBucket; }
	inline void SetRenderBucket( s32 bucket )
	{
		// if <0, draw before all opaque and transparent, if >0 draw after.
		ASSERT( bucket >= -INSTANCE_RENDER_BUCKET_MAX && bucket < INSTANCE_RENDER_BUCKET_MAX );
		d_renderBucket = bucket;
	}

	inline void GetGeometry( ts_Geometry* geom ) const
	{
		ASSERT_PTR( geom );
		geom->pLOD = NULL;
		geom->pSOM = d_SOM;
	}

	void RemoveDisplayList();

	void MakeBones();
	void RemoveBones();

	void InitFromGeometry( ts_Geometry& geometry );
	void InitFromDynamicObject( DynamicObject* dynamicObject );

	void SetVisualPropertiesFromInstance( CInstance* inst );

	void UpdateBoundingVolumes();
	void UpdateBoundingVolumes( float scaleX, float scaleY, float scaleZ );
	void UpdateBoundingVolumesAccurate();

	void FixDiskLoad( ts_RecordList *pInstList, ts_RecordList *pSOMList );

	static bool LoadInstances( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize );
	static bool LoadInstanceAttributes( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize );

	bool RenderedLastFrame() const;

	inline bool VerifyAlignment() const
	{
		#define INSTANCE_ALIGN_CHECK(field,align) ( ( ( (int)&field - (int)this ) & ( (align) - 1 ) ) == 0 )
		if ( !INSTANCE_ALIGN_CHECK( d_localToWorld, 16 ) )
			return false;
		if ( !INSTANCE_ALIGN_CHECK( d_boundingSphere, 16 ) )
			return false;
		if ( !INSTANCE_ALIGN_CHECK( d_boundingBox, 16 ) )
			return false;
		return true;
	}

private:
	void SetupBillboardMatrix();
	void RenderDynamic( float cameraZDist, Graphics4x4* localToWorld );
	void RenderNormal( float cameraZDist );
	void DynamicallyAllocateDL( ts_PatchSOM* som );
	void FixForMemoryReport( ts_PatchSOM* som );

private:
	friend class ExportInstance;
   friend class DBLSwap;
	//
	// NOTICE: this structure is exported from the filter chain, and if
   //         modified the filter chain and all art must be rebuilt.
	//
	Graphics4x4					d_localToWorld;
	ts_BoundingSphere			d_boundingSphere;
	ts_BoundingBox				d_boundingBox;
	u32							d_flags;
	u32							d_viewFlags;
	u32							d_lightingBits;
	float							d_alpha;
	s32							d_renderBucket;
	ts_PatchSOM*				d_SOM;
	DynamicObject*				d_dynamicObject;
	float							d_fadeOutDistance;
	u32							d_lastRenderFrame;
	TextureAnimatorArray*	d_textureAnimatorArray;
	ts_DisplayListRecord		d_displayList;
	void*							d_vertexColors;
	int							d_colorSet;
	ts_Body*						d_body;
	CLightInfluences*			d_curLightInfluences;
	float							d_attenuationScale;
	CActor*						d_owner;
	u32							d_uniqueID;
	float							d_sortBias;
	int							d_preAssignedLights[2]; // preassigned lights (up to 2 for now)
	char							d_name[ 32 ];

	enum InstanceFlags
	{
		INSTANCE_HIDDEN					= 0x0001,
		INSTANCE_DYNAMIC_ALLOC_DL		= 0x0002,
		INSTANCE_ADDITIVE_BLEND			= 0x0004,
		INSTANCE_ALPHA_SORT				= 0x0008,
		INSTANCE_BILLBOARD            = 0x0010,
		INSTANCE_VERTEX_COLOR_32BIT   = 0x0020,
		INSTANCE_CLOCKWISE_VERTICES   = 0x0040,
		INSTANCE_NO_RECEIVE_SHADOWS	= 0x0080,
		INSTANCE_NO_ZWRITE				= 0x0100,
		INSTANCE_NO_FAR_CLIP				= 0x0200,
		INSTANCE_NO_FOG					= 0x0400,
		INSTANCE_BILLBOARD3D				= 0x0800,
	};
};

//============================================================================

#endif //_INSTANCE_H

