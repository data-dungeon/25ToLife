/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/text3d.h"
#include "Display/Common/RenderState.h"
#include "camera/camera.h"
#include "camera/director.h"

#define VTXBUFFER_NEED_INLINES
#include "Display/Common/VtxBuffer.h"

Text3D::Text3D()
	: InstancedDynamicObject("Text")
{
	mInstance->SetFogDisabled(true);
	mInstance->SetReceiveShadows(false);
	mInstance->SetDynamicallyLit(false);

	m_strip = NULL;
	m_vertex = NULL;

	m_position.Clear();
	m_scale.Set(1.0f, 1.0f, 1.0f);
}

// this code was pretty much ripped from Text.cpp, so blame it.
Texture* Text3D::GetDaTexture()
{
	TextureDatabase *pTextureDatabase = (TextureDatabase *) dblGetChunkData( DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, m_pFont->m_texture.handle );

	if (pTextureDatabase == NULL)
		pTextureDatabase = (TextureDatabase *) dblGetChunkData( DB_MERGED_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, m_pFont->m_texture.handle );

	if( pTextureDatabase )
	{
		// Hack!!   This assumes that the font database contains only
		// 1 texture.
		ASSERT_PTR(pTextureDatabase);
		ASSERTS(pTextureDatabase->TextureCount() == 1, "Font texture database has more than one texture in it");

		// get the first texture
		Texture* tex = pTextureDatabase->GetTexture(0);
		ASSERT_PTR(tex);

		return tex;
	}
	ASSERT_PTR(pTextureDatabase);

	return NULL;
}

void Text3D::AllocStripAndVtxBuffers(int handles)
{
	int lastHandles = m_lastHandles;
	m_lastHandles = handles;

	if(m_vertex != NULL)
	{
		int currHandles = m_vertex->GetVtxCount() / 4;

		// handles the same, just use the same buffers
		if(lastHandles == handles)
			return;

		// are we decreasing handles
		if(lastHandles > handles)
		{
			// zap old verts to degenerates..
			m_vertex->Lock();
			for(int i = handles*4; i < lastHandles * 4; i++)
			{
				m_vertex->Vertex(i).Set(0.0f, 0.0f, 0.0f);
			}
			m_vertex->UnLock();
			return;
		}

		// if we are increasing the handles, and there is still room
		// them to fit, just keep the old buffers.
		if(handles <= currHandles)
			return;
	}

	if(handles == 0)
		return;

   // ok, no easy way out, we need to realloc these things...

	if(m_vertex != NULL)
	{
		delete m_vertex;
		delete m_strip;
	}

	m_vertex = new VtxBuffer();
	m_strip = new StripBuffer();

	u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS] = { VtxBuffer::VERT_TEX_DIM2 };
	m_vertex->Init(handles * 4, VtxBuffer::VERT_HAS_DIFFUSE, &texFlags[0]);
	m_strip->Init();
	m_strip->SetVtxBuffer(m_vertex);

	#ifdef PS2
	// we dont realy use overbrighting, but the colors have already been divided, so same diff
	m_strip->Overbrighting(true);
	#endif
	
	u16 indices[4] = {2, 3, 1, 0};
	while(handles--)
	{
		m_strip->AddStrip(&indices[0], 4);
		indices[0]+=4;
		indices[1]+=4;
		indices[2]+=4;
		indices[3]+=4;
	}

}

//////////////////////////////////////////////////////////////////////////
// Rendering functions
	
void Text3D::Draw(void)
{
	// TODO: figure out how to deal with shadows...

	// do all the normal work... 
	// we want the sprites to be up to date
	Text::Draw();

	// make sure we have room to work in the buffers
	int handles = GetTotalHandles();

	AllocStripAndVtxBuffers(handles);

	float minx, maxx, miny, maxy;
	minx = miny = 10000.0f;
	maxx = maxy = -10000.0f;

	int h = handles;
	while(h--)
	{
		t_Handle handle = GetTotalHandle(h);
		if(!handle) continue;
		Vector3 *verts;
		SpriteMgr__GetDataPointers(handle, NULL, &verts, NULL, NULL);
		for(int i = 0; i < 4; i++)
		{
			if(minx > verts->x())
				minx = verts->x();
			if(maxx < verts->x())
				maxx = verts->x();
			if(miny > verts->y())
				miny = verts->y();
			if(maxy < verts->y())
				maxy = verts->y();
			verts++;
		}
	}

	#define CENTER(x, minx, maxx) (x - minx - (maxx - minx) * 0.5f)
	float aspect = Display__GetScreenAspectRatio();

	int vtx = 0;
	while(handles--)
	{
		// shut it off.
		t_Handle handle = GetTotalHandle(handles);
		if(!handle) continue;

		SpriteMgr__DisableSprite(handle);

		// grab its info and put it into the vtx buffer...
		Vector3 *verts;
		Vector2 *uvs;
		u32 *colors;
		SpriteMgr__GetDataPointers(handle, NULL, &verts, &uvs, (ts_bRGBA**)&colors);
		float z = verts->z();
		if(IsShadowHandleIndex(handles)) z -= 0.001f;
		m_vertex->Lock();
		m_vertex->Diffuse(vtx) = VtxBuffer::cvtRGBA(*colors++);
		m_vertex->Vertex(vtx).Set(
			CENTER(verts->x(),minx,maxx) * aspect, 
			-CENTER(verts->y(),miny,maxy), 
			verts->z());
		verts++;
		m_vertex->TexCoordv2(0, vtx++) = *uvs++;
		m_vertex->Diffuse(vtx) = VtxBuffer::cvtRGBA(*colors++);
		m_vertex->Vertex(vtx).Set(
			CENTER(verts->x(),minx,maxx) * aspect, 
			-CENTER(verts->y(),miny,maxy), 
			verts->z());
		verts++;
		m_vertex->TexCoordv2(0, vtx++) = *uvs++;
		m_vertex->Diffuse(vtx) = VtxBuffer::cvtRGBA(*colors++);
		m_vertex->Vertex(vtx).Set(
			CENTER(verts->x(),minx,maxx) * aspect, 
			-CENTER(verts->y(),miny,maxy), 
			verts->z());
		verts++;
		m_vertex->TexCoordv2(0, vtx++) = *uvs++;
		m_vertex->Diffuse(vtx) = VtxBuffer::cvtRGBA(*colors++);
		m_vertex->Vertex(vtx).Set(
			CENTER(verts->x(),minx,maxx) * aspect, 
			-CENTER(verts->y(),miny,maxy), 
			verts->z());
		verts++;
		m_vertex->TexCoordv2(0, vtx++) = *uvs++;
		m_vertex->UnLock();
	}

	return;
}

Text3D::~Text3D()
{
	if(m_vertex)
		delete m_vertex;
	if(m_strip)
		delete m_strip;
}

void Text3D::Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist)
{
	if(m_strip == NULL)
		return;

	if(IsHidden())
		return;

	// dont know why the glyphs in the Text object are not updated in this
	// case, but they arent.
	if(GetText() == NULL || GetText()[0] == '\0')
		return;

	Camera &camera = g_cameraDirector.CurrentCamera();
	Graphics4x4 camerarotation = camera.WorldToView();

	// we need w at our position.
	Vector2 positionOnScreen;
	float w;
	if (camera.ToScreen( m_position, positionOnScreen, w ))
	{
		//	Graphics4x4 camerarotation = *(Graphics4x4*)MatrixStack__Get(MS_MODELVIEW);
		camerarotation.PokeTranslation(0.0f, 0.0f, 0.0f);
		camerarotation.Invert();
		camerarotation.CatScale(w, w, w);
		Matrix4x4 localToView = camerarotation * *pmLocalToCamera;
	
		Texture* tex = GetDaTexture();
		m_strip->SetPassTexture(0, tex, TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR, RS_BLENDFUNC_DEFAULT);
		tex->Lock();
		m_strip->Render(pInstance, (Graphics4x4*)&localToView, fZCamDist);
		tex->Unlock();
	}
}

