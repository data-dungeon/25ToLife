/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/sprite.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"

//=============================================================================
// SpriteInterface::SpriteInterface
//=============================================================================

MAKE_CLASSFACTORY( SpriteInterface );

SpriteInterface::SpriteInterface( void )
{
	m_pSpriteHost = 0;
}

//=============================================================================
// SpriteInterface::SpriteInterface
//=============================================================================

SpriteInterface::~SpriteInterface( void )
{
}

//=============================================================================
// SpriteInterface::Init
//=============================================================================

bool SpriteInterface::Init( void* Param )
{
	return true;
}

//=============================================================================
// SpriteInterface::Kill
//=============================================================================

void SpriteInterface::Kill( void )
{
}

//=============================================================================
// SpriteInterface::Process
//=============================================================================

void SpriteInterface::Process( void )
{
	OnUpdate();

	SpriteInterface *pInterface = Child();

	while( pInterface )
	{
		pInterface->Process();
		pInterface = pInterface->Next();
	}

	return;
}

//=============================================================================
// SpriteInterface::Save
//=============================================================================

bool SpriteInterface::Save( MemoryBank& m )
{
	return true;
}

//=============================================================================
// SpriteInterface::Load
//=============================================================================

bool SpriteInterface::Load( MemoryBank& m )
{
	return true;
}

//=============================================================================
// SpriteInterface::OnUpdate
//=============================================================================

void SpriteInterface::OnUpdate( void )
{
}

//=============================================================================
// SpriteAnim::SpriteAnim() - Currently just a stub as no constructon code is
//										required at this time.
//=============================================================================

SpriteAnim::SpriteAnim( void )
{
}

//=============================================================================
// SpriteAnim::~SpriteAnim() - Currently just a stub as no destructon code is
//										 required at this time.
//=============================================================================

SpriteAnim::~SpriteAnim()
{
}

//=============================================================================
// SpriteAnim::SetTarget()
//=============================================================================

void SpriteAnim::SetTarget( Sprite *pSprite )
{
	if( pSprite )
	{
		texture.texture.SetTarget( (u32 *) &pSprite->m_state.textureset.texture );

		position.x.SetTarget( &pSprite->m_state.position.x );
		position.y.SetTarget( &pSprite->m_state.position.y );

		size.x.SetTarget( &pSprite->m_state.size.x );
		size.y.SetTarget( &pSprite->m_state.size.y );

		scale.x.SetTarget( &pSprite->m_state.scale.x );
		scale.y.SetTarget( &pSprite->m_state.scale.y );

		color.a.SetTarget( &pSprite->m_state.color.a );
		color.r.SetTarget( &pSprite->m_state.color.r );
		color.g.SetTarget( &pSprite->m_state.color.g );
		color.b.SetTarget( &pSprite->m_state.color.b );

		familycolor.a.SetTarget( &pSprite->m_state.familycolor.a );
		familycolor.r.SetTarget( &pSprite->m_state.familycolor.r );
		familycolor.g.SetTarget( &pSprite->m_state.familycolor.g );
		familycolor.b.SetTarget( &pSprite->m_state.familycolor.b );

		rotation.z.SetTarget( &pSprite->m_state.rotation.z );

		origin.x.SetTarget( &pSprite->m_state.origin.x );
		origin.y.SetTarget( &pSprite->m_state.origin.y );

		offset.x.SetTarget( &pSprite->m_state.offset.x );
		offset.y.SetTarget( &pSprite->m_state.offset.y );

		/* chopped
		for( int i = 0; i < 4; i++ )
		{
			vertex[i].x.SetTarget( &pSprite->m_state.vertexposition[i].x );
			vertex[i].y.SetTarget( &pSprite->m_state.vertexposition[i].y );
			vertex[i].r.SetTarget( &pSprite->m_state.vertexcolor[i].r );
			vertex[i].g.SetTarget( &pSprite->m_state.vertexcolor[i].g );
			vertex[i].b.SetTarget( &pSprite->m_state.vertexcolor[i].b );
			vertex[i].a.SetTarget( &pSprite->m_state.vertexcolor[i].a );
			vertex[i].u.SetTarget( &pSprite->m_state.texcoords[i].x );
			vertex[i].v.SetTarget( &pSprite->m_state.texcoords[i].y );

			crop[i].x.SetTarget( &pSprite->m_state.clipcoords[i].x );
			crop[i].y.SetTarget( &pSprite->m_state.clipcoords[i].y );
		}
		*/
	}

	return;
}

//=============================================================================
// SpriteAnim::SetPriority()
//=============================================================================

void SpriteAnim::SetPriority( char priority )
{
	texture.texture.SetPriority( priority );

	position.x.SetPriority( priority );
	position.y.SetPriority( priority );

	size.x.SetPriority( priority );
	size.y.SetPriority( priority );

	scale.x.SetPriority( priority );
	scale.y.SetPriority( priority );

	color.a.SetPriority( priority );
	color.r.SetPriority( priority );
	color.g.SetPriority( priority );
	color.b.SetPriority( priority );

	familycolor.a.SetPriority( priority );
	familycolor.r.SetPriority( priority );
	familycolor.g.SetPriority( priority );
	familycolor.b.SetPriority( priority );

	rotation.z.SetPriority( priority );

	origin.x.SetPriority( priority );
	origin.y.SetPriority( priority );

	offset.x.SetPriority( priority );
	offset.y.SetPriority( priority );

	/* chopped
	for( int i = 0; i < 4; i++ )
	{
		vertex[i].x.SetPriority( priority );
		vertex[i].y.SetPriority( priority );
		vertex[i].r.SetPriority( priority );
		vertex[i].g.SetPriority( priority );
		vertex[i].b.SetPriority( priority );
		vertex[i].a.SetPriority( priority );
		vertex[i].u.SetPriority( priority );
		vertex[i].v.SetPriority( priority );

		crop[i].x.SetPriority( priority );
		crop[i].y.SetPriority( priority );
	}
	*/

	return;
}

//=============================================================================
// SpriteAnim::Play()
//=============================================================================

void SpriteAnim::Play( bool Looped )
{
	texture.texture.Play( Looped );

	position.x.Play( Looped );
	position.y.Play( Looped );

	size.x.Play( Looped );
	size.y.Play( Looped );

	scale.x.Play( Looped );
	scale.y.Play( Looped );

	color.a.Play( Looped );
	color.r.Play( Looped );
	color.g.Play( Looped );
	color.b.Play( Looped );

	familycolor.a.Play( Looped );
	familycolor.r.Play( Looped );
	familycolor.g.Play( Looped );
	familycolor.b.Play( Looped );

	rotation.z.Play( Looped );

	origin.x.Play( Looped );
	origin.y.Play( Looped );

	offset.x.Play( Looped );
	offset.y.Play( Looped );

	/* chopped
	for( int i = 0; i < 4; i++ )
	{
		vertex[i].x.Play( Looped );
		vertex[i].y.Play( Looped );
		vertex[i].r.Play( Looped );
		vertex[i].g.Play( Looped );
		vertex[i].b.Play( Looped );
		vertex[i].a.Play( Looped );
		vertex[i].u.Play( Looped );
		vertex[i].v.Play( Looped );

		crop[i].x.Play( Looped );
		crop[i].y.Play( Looped );
	}
	*/

	return;
}

//=============================================================================
// SpriteAnim::Resume()
//=============================================================================

void SpriteAnim::Resume( bool Looped )
{
	texture.texture.Resume( Looped );

	position.x.Resume( Looped );
	position.y.Resume( Looped );

	size.x.Resume( Looped );
	size.y.Resume( Looped );

	scale.x.Resume( Looped );
	scale.y.Resume( Looped );

	color.a.Resume( Looped );
	color.r.Resume( Looped );
	color.g.Resume( Looped );
	color.b.Resume( Looped );

	familycolor.a.Resume( Looped );
	familycolor.r.Resume( Looped );
	familycolor.g.Resume( Looped );
	familycolor.b.Resume( Looped );

	rotation.z.Resume( Looped );

	origin.x.Resume( Looped );
	origin.y.Resume( Looped );

	offset.x.Resume( Looped );
	offset.y.Resume( Looped );

	/* chopped
	for( int i = 0; i < 4; i++ )
	{
		vertex[i].x.Resume( Looped );
		vertex[i].y.Resume( Looped );
		vertex[i].r.Resume( Looped );
		vertex[i].g.Resume( Looped );
		vertex[i].b.Resume( Looped );
		vertex[i].a.Resume( Looped );
		vertex[i].u.Resume( Looped );
		vertex[i].v.Resume( Looped );

		crop[i].x.Resume( Looped );
		crop[i].y.Resume( Looped );
	}
	*/

	return;
}

//=============================================================================
// SpriteAnim::Stop()
//=============================================================================

void SpriteAnim::Stop( void )
{
	texture.texture.Stop();

	position.x.Stop();
	position.y.Stop();

	size.x.Stop();
	size.y.Stop();

	scale.x.Stop();
	scale.y.Stop();

	color.a.Stop();
	color.r.Stop();
	color.g.Stop();
	color.b.Stop();

	familycolor.a.Stop();
	familycolor.r.Stop();
	familycolor.g.Stop();
	familycolor.b.Stop();

	rotation.z.Stop();

	origin.x.Stop();
	origin.y.Stop();

	offset.x.Stop();
	offset.y.Stop();

	/* chopped
	for( int i = 0; i < 4; i++ )
	{
		vertex[i].x.Stop();
		vertex[i].y.Stop();
		vertex[i].r.Stop();
		vertex[i].g.Stop();
		vertex[i].b.Stop();
		vertex[i].a.Stop();
		vertex[i].u.Stop();
		vertex[i].v.Stop();

		crop[i].x.Stop();
		crop[i].y.Stop();
	}
	*/

	return;
}

//=============================================================================
// SpriteAnim::Rewind()
//=============================================================================

void SpriteAnim::Rewind( void )
{
	texture.texture.Rewind();

	position.x.Rewind();
	position.y.Rewind();

	size.x.Rewind();
	size.y.Rewind();

	scale.x.Rewind();
	scale.y.Rewind();

	color.a.Rewind();
	color.r.Rewind();
	color.g.Rewind();
	color.b.Rewind();

	familycolor.a.Rewind();
	familycolor.r.Rewind();
	familycolor.g.Rewind();
	familycolor.b.Rewind();

	rotation.z.Rewind();

	origin.x.Rewind();
	origin.y.Rewind();

	offset.x.Rewind();
	offset.y.Rewind();

	/* chopped
	for( int i = 0; i < 4; i++ )
	{
		vertex[i].x.Rewind();
		vertex[i].y.Rewind();
		vertex[i].r.Rewind();
		vertex[i].g.Rewind();
		vertex[i].b.Rewind();
		vertex[i].a.Rewind();
		vertex[i].u.Rewind();
		vertex[i].v.Rewind();

		crop[i].x.Rewind();
		crop[i].y.Rewind();
	}
	*/

	return;
}

//=============================================================================
// SpriteAnim::Jump( Time )
//=============================================================================

void SpriteAnim::Jump( float Time )
{
	texture.texture.Jump( Time );

	position.x.Jump( Time );
	position.y.Jump( Time );

	size.x.Jump( Time );
	size.y.Jump( Time );

	scale.x.Jump( Time );
	scale.y.Jump( Time );

	color.a.Jump( Time );
	color.r.Jump( Time );
	color.g.Jump( Time );
	color.b.Jump( Time );

	familycolor.a.Jump( Time );
	familycolor.r.Jump( Time );
	familycolor.g.Jump( Time );
	familycolor.b.Jump( Time );

	rotation.z.Jump( Time );

	origin.x.Jump( Time );
	origin.y.Jump( Time );

	offset.x.Jump( Time );
	offset.y.Jump( Time );

	/* chopped
	for( int i = 0; i < 4; i++ )
	{
		vertex[i].x.Jump( Time );
		vertex[i].y.Jump( Time );
		vertex[i].r.Jump( Time );
		vertex[i].g.Jump( Time );
		vertex[i].b.Jump( Time );
		vertex[i].a.Jump( Time );
		vertex[i].u.Jump( Time );
		vertex[i].v.Jump( Time );

		crop[i].x.Jump( Time );
		crop[i].y.Jump( Time );
	}
	*/

	return;
}

//=============================================================================
// SpriteAnim::Clear()
//=============================================================================

void SpriteAnim::Clear( void )
{
	texture.texture.Clear();

	position.x.Clear();
	position.y.Clear();

	size.x.Clear();
	size.y.Clear();

	scale.x.Clear();
	scale.y.Clear();

	color.a.Clear();
	color.r.Clear();
	color.g.Clear();
	color.b.Clear();

	familycolor.a.Clear();
	familycolor.r.Clear();
	familycolor.g.Clear();
	familycolor.b.Clear();

	rotation.z.Clear();

	origin.x.Clear();
	origin.y.Clear();

	offset.x.Clear();
	offset.y.Clear();

	/* chopped
	for( int i = 0; i < 4; i++ )
	{
		vertex[i].x.Clear();
		vertex[i].y.Clear();
		vertex[i].r.Clear();
		vertex[i].g.Clear();
		vertex[i].b.Clear();
		vertex[i].a.Clear();
		vertex[i].u.Clear();
		vertex[i].v.Clear();

		crop[i].x.Clear();
		crop[i].y.Clear();
	}
	*/

	return;
}

//=============================================================================
// Sprite::Sprite() : The constructor just does some routine stuff, set
//						  : pointers, to zero, invalid handles, etc...
//=============================================================================

float Sprite::m_screenx = 0.0f;
float Sprite::m_screeny = 0.0f;
float Sprite::m_invscreenx = 1.0f / 640.0f;
float Sprite::m_invscreeny = 1.0f / 480.0f;
float Sprite::m_viewportx = 0.0f;
float Sprite::m_viewporty = 0.0f;

Sprite *Sprite::m_pDrawList;
Sprite *Sprite::m_pDrawTail;

#ifndef CDROM
int g_sprites = 0;
#endif

Sprite::Sprite( void )
{
#ifndef CDROM
	++g_sprites;
#endif

	// initialize texture
	m_status.texture.texturedatabase = 0;
	m_state.textureset.handle = INVALID_HANDLE;
	m_state.textureset.texture = 0;
	m_state.textureset.cell = 0;
	m_state.textureset.cols = 1;
	m_state.textureset.rows = 1;
	m_state.textureset.sizex = 0;
	m_state.textureset.sizey = 0;

	// initialize anim
	m_anim = 0;

	// initialize state
	m_status.texture.texturedatabase = 0;
	m_status.texture.handle = INVALID_HANDLE;
	m_status.texture.texture = 0;
	m_status.handles.sprite = 0;
	m_status.handles.shadow = 0;

	// initialize draw list
	m_pInterfaces = 0;
	m_pDrawNext = 0;
	m_pDrawPrev = 0;

	// TJC - we start without children, therefore, they are all released.
	SetChildHandlesAreReleased( true );
	SetDrawPending( false );

	Reset();
}

//=============================================================================
// Sprite::~Sprite() : The destructor frees up the sprite and removes it from
//							: the tree.
//=============================================================================

Sprite::~Sprite()
{
#ifndef CDROM
	--g_sprites;
#endif

	DeleteInterfaces();
	RemoveFromDrawList();
	Release();
	Remove();
}

//=============================================================================
// Sprite::AreHandlesReleased() - TJC
//
//		This routine need to return true if Release() would perform no actions.
//		Otherwise, return false.
//=============================================================================

bool Sprite::AreHandlesReleased(void)
{
	if(m_status.handles.sprite == INVALID_HANDLE &&
		m_status.handles.shadow == INVALID_HANDLE )
		return true;

	return false;
}

//=============================================================================
// Sprite::InformParentsWeAreNotReleased() - TJC
//
//		Let all the parents know that one of their children is not released.
//=============================================================================

void Sprite::InformParentsOurHandlesArentReleased(void)
{
	Sprite *pParent = Parent();

	if( !pParent )
		return;

	pParent->SetChildHandlesAreReleased(false);
	pParent->InformParentsOurHandlesArentReleased();
}

//=============================================================================
// Sprite::ChildHandlesAreReleased
//
//		Returns true if our child handles are release, false if not.
//=============================================================================

bool Sprite::ChildHandlesAreReleased( void )
{
	if( m_StateFlags & STATE_DEADCHILDREN )
		return true;

	return false;
}

//=============================================================================
// Sprite::SetChildHandlesAreReleased
//
//		Returns true if our child handles are release, false if not.
//=============================================================================

void Sprite::SetChildHandlesAreReleased( bool TrueOrFalse )
{
	if( TrueOrFalse )
		m_StateFlags |= STATE_DEADCHILDREN;
	else
		m_StateFlags &= ~STATE_DEADCHILDREN;

	return;
}

//=============================================================================
// Sprite::SimulateScreenResolution()
//=============================================================================

void Sprite::SimulateScreenResolution( float Width, float Height )
{
	m_screenx = Width;
	m_screeny = Height;
	m_invscreenx = 1.0f / SimulatedScreenWidth();
	m_invscreeny = 1.0f / SimulatedScreenHeight();
}

//=============================================================================
// Sprite::SimulatedScreenWidth()
//=============================================================================

float Sprite::SimulatedScreenWidth( void )
{
	if( m_screenx != 0.0f )
		return m_screenx;

	return (float)640.0f; // Display__GetScreenWidth();
}

//=============================================================================
// Sprite::SimulatedScreenHeight()
//=============================================================================

float Sprite::SimulatedScreenHeight( void )
{
	if( m_screeny != 0.0f )
		return m_screeny;

	return (float)480.0f;// Display__GetScreenHeight();
}

//=============================================================================
// Sprite::SetViewPortOrigin()
//=============================================================================

void Sprite::SetViewPortOrigin( float x, float y )
{
	m_viewportx = x;
	m_viewporty = y;
}

//=============================================================================
// Sprite::ViewPortX()
//=============================================================================

float Sprite::ViewPortX( void )
{
	return m_viewportx;
}

//=============================================================================
// Sprite::ViewPortY()
//=============================================================================

float Sprite::ViewPortY( void )
{
	return m_viewporty;
}

//=============================================================================
// Sprite::ViewPortOrigin()
//=============================================================================

void Sprite::ViewPortOrigin( float &x, float &y )
{
	x = m_viewportx;
	y = m_viewporty;
}

//=============================================================================
// Sprite::DrawAll()
//=============================================================================

#ifdef _IGNORE//_WIN32
static __int64 CpuTimer1 = 0;
static __int64 CpuTimer2 = 0;
static __int64 TimerLatency = 0;

inline void StartCpuTimer( void )
{
	for( int i = 0; i < 20; i++ )
	{
		__asm
		{
			xor	eax, eax;
			xor	ebx, ebx;
			xor	ecx, ecx;
			xor	edx, edx;
			cpuid;
			rdtsc;
			mov	dword ptr CpuTimer1+0, eax;
			mov	dword ptr CpuTimer1+4, edx;
		}

		__asm
		{
			xor	eax, eax;
			xor	ebx, ebx;
			xor	ecx, ecx;
			xor	edx, edx;
			cpuid;
			rdtsc;
			mov	dword ptr CpuTimer2+0, eax;
			mov	dword ptr CpuTimer2+4, edx;
		}
	}

	TimerLatency = CpuTimer2 - CpuTimer1;

	__asm
	{
		xor	eax, eax;
		xor	ebx, ebx;
		xor	ecx, ecx;
		xor	edx, edx;
		cpuid;
		rdtsc;
		mov	dword ptr CpuTimer1+0, eax;
		mov	dword ptr CpuTimer1+4, edx;
	}

	return;
}

inline void StopCpuTimer( void )
{
	__asm
	{
		xor	eax, eax;
		xor	ebx, ebx;
		xor	ecx, ecx;
		xor	edx, edx;
		cpuid;
		rdtsc;
		mov	dword ptr CpuTimer2+0, eax;
		mov	dword ptr CpuTimer2+4, edx;
	}

	return;
}

inline int GetCpuTimer( void )
{
	return (int) (CpuTimer2 - CpuTimer1 - TimerLatency);
}

#else
inline void StartCpuTimer( void ) {}
inline void StopCpuTimer( void ) {}
inline int GetCpuTimer( void ) { return 0; }
#endif

static int Performance[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void Sprite::DrawAll( void )
{
	int c;

	Sprite *pSprite = m_pDrawList;

	//=========================================================================
	// Draw all at once, helps to improve cache usage significantly
	//=========================================================================

	for( int i = 15; i > 0; i-- )
		Performance[i] = Performance[i-1];

	StartCpuTimer();

	for( c = 0; pSprite; c++ )
	{
		if( pSprite->DrawPending() )
			pSprite->Draw();

		pSprite = pSprite->m_pDrawNext;
	}

	StopCpuTimer();

	Performance[0] = GetCpuTimer();

	m_pDrawList = 0;
	m_pDrawTail = 0;
}

void Sprite::DrawCancel( void )
{
	Sprite* pSprite = m_pDrawList;

	while( pSprite )
	{
		pSprite->SetDrawPending( false );
		pSprite = pSprite->m_pDrawNext;
	}

	m_pDrawList = 0;
	m_pDrawTail = 0;
}

int Sprite::GetSpritePerformanceStatistics( void )
{
	int Result = 0;

	for( int i = 0; i < 16; i++ )
		Result += Performance[i];

	return Result >> 4;
}

//=============================================================================
// Sprite::Create()
//=============================================================================

void Sprite::Create( t_Handle textureset, int texture, int cell, int cols, int rows )
{
	SetTextureSet( textureset, texture, cell, cols, rows ), OnCreate();
}

//=============================================================================
// Sprite::Copy()
//=============================================================================

void Sprite::Copy( Sprite *sprite )
{
	if( sprite )
		memcpy( &m_state, &sprite->m_state, sizeof(m_state) );

	return;
}

//=============================================================================
// Sprite::Reset() : Resets the sprite to its default state.
//=============================================================================

void Sprite::Reset( void )
{
	//==========================================================================
	// Default position is the upper-left corner.
	//==========================================================================

	SetPosition( 0.0f, 0.0f );

	//==========================================================================
	// Default size is same as the source texture.
	//==========================================================================

	SetSize( m_state.textureset.sizex / (float) m_state.textureset.cols,
			   m_state.textureset.sizey / (float) m_state.textureset.rows );

	//==========================================================================
	// Default scale is normal.
	//==========================================================================

	SetScale( 1.0f, 1.0f );

	//==========================================================================
	// Default origin is the upper left corner.
	//==========================================================================

	SetOrigin( 0.0f, 0.0f );

	//==========================================================================
	// Default offset is none.
	//==========================================================================

	SetOffset( 0.0f, 0.0f );

	//==========================================================================
	// Default rotation is none.
	//==========================================================================

	SetRotation( 0.0f );

	//==========================================================================
	// Default base color is white with no tranparency.
	//==========================================================================

	SetColor( 1.0f, 1.0f, 1.0f, 1.0f );

	//==========================================================================
	// Default group color is white with no transparency.
	//==========================================================================

	SetFamilyColor( 1.0f, 1.0f, 1.0f, 1.0f );

	//==========================================================================
	// Default vertex colors are white with no transparency.
	//==========================================================================

	SetVertexColor( 0, 1.0f, 1.0f, 1.0f, 1.0f );
	SetVertexColor( 1, 1.0f, 1.0f, 1.0f, 1.0f );
	SetVertexColor( 2, 1.0f, 1.0f, 1.0f, 1.0f );
	SetVertexColor( 3, 1.0f, 1.0f, 1.0f, 1.0f );

	//==========================================================================
	// Set the default vertex position.
	//==========================================================================

	SetVertexPosition( 0, 0.0f, 0.0f );
	SetVertexPosition( 1, 0.0f, 0.0f );
	SetVertexPosition( 2, 0.0f, 0.0f );
	SetVertexPosition( 3, 0.0f, 0.0f );

	//==========================================================================
	// Set the default texture coordinates.
	//==========================================================================

	SetTextureCoordinates( 0, 0.0f, 0.0f );
	SetTextureCoordinates( 1, 1.0f, 0.0f );
	SetTextureCoordinates( 2, 1.0f, 1.0f );
	SetTextureCoordinates( 3, 0.0f, 1.0f );

	//==========================================================================
	// Disable cropping/clipping.
	//==========================================================================

	CropRect( 0.0f, 0.0f, 1.0f, 1.0f );

	//==========================================================================
	// By default, the shadow is disabled.
	//==========================================================================

	ShadowSettings( false );

	//==========================================================================
	// Default sort priority is normal.
	//==========================================================================

	SetPriority( 0 );

	//==========================================================================
	// Default blend mode
	//==========================================================================

	SetBlendMode( SPRITEBLEND_NORMAL );

	//==========================================================================
	// Make the sprite visible.
	//==========================================================================

	Show();
}

//=============================================================================
// Sprite::Release()
//=============================================================================

void Sprite::Release( void )
{
	//=========================================================================
	// Allow derived classes to take action.
	//=========================================================================

	OnRelease();

	//==========================================================================
	// Set the texture set to invalid, otherwise the sprite will be recreated
	// on the next draw.
	//==========================================================================

	SetTextureSet( INVALID_HANDLE, 0 );

	//==========================================================================
	// Free handles.
	//==========================================================================

	ReleaseHandles();

	//==========================================================================
	// Destroy the animation.
	//==========================================================================

	KillAnim();
}

//=============================================================================
// Sprite::ReleaseAll() : This releases the sprite and all its children.
//=============================================================================

void Sprite::ReleaseAll( void )
{
	//==========================================================================
	// Release itself.
	//==========================================================================

	Release();

	//==========================================================================
	// Release all child sprites.
	//==========================================================================

	Sprite *pChild = Child();

	while( pChild )
		pChild->ReleaseAll(), pChild = pChild->Next();

	return;
}

//=============================================================================
// Sprite::ReleaseHandles() : Frees up both sprite handles and shadow sprite
//									 : handles, usually in response to a sprite being
//									 : disabled (which can happen in the alpha channel
//									 : is reduced to zero), or the sprite has been
//									 : destroyed.
//=============================================================================

void Sprite::ReleaseHandles( void )
{
	ReleaseSprites();
	ReleaseShadows();
}

//=============================================================================
// Sprite::ReleaseHandlesRecursively() : Frees up handles throughout the tree,
//													: usually in response to the root being
//													: disabled or when the whole family has
//													: become transparent.
//=============================================================================

void Sprite::ReleaseHandlesRecursively( void )
{
	ReleaseHandles();

	// TJC - if all our children are already released, whats the point?
	if( ChildHandlesAreReleased() )
		return;

	Sprite *child = Child();

	while( child )
		child->ReleaseHandlesRecursively(), child = child->Next();

	// TJC - keep track of the fact that we have released all our children.
	SetChildHandlesAreReleased( true );
}

//=============================================================================
// Sprite::AddInterface()
//=============================================================================

SpriteInterface* Sprite::AddInterface( const char* pInterfaceName, void* Param )
{
	if( !m_pInterfaces )
	{
		m_pInterfaces = new SpriteInterface;

		if( !m_pInterfaces )
			return 0;
	}

	SpriteInterface* pInterface = SpriteInterface::NewObject( pInterfaceName );

	if( pInterface )
	{
		pInterface->SetSpriteHost( this );

		if( pInterface->Init( Param ) )
			m_pInterfaces->AddChild( pInterface );
		else
		{
			delete pInterface;
			pInterface = 0;
		}
	}

	return pInterface;
}

//=============================================================================
// Sprite::GetInterface()
//=============================================================================

SpriteInterface* Sprite::GetInterface( const char* pInterfaceName )
{
	SpriteInterface *pInterface = 0;

	if( m_pInterfaces )
	{
		pInterface = m_pInterfaces->Child();

		while( pInterface )
		{
			if( 0 == stricmp( pInterface->Classname(), pInterfaceName ) )
				break;

			pInterface = pInterface->Next();
		}
	}

	return pInterface;
}

//=============================================================================
// Sprite::ProcessInterfaces()
//=============================================================================

void Sprite::ProcessInterfaces( void )
{
	if( m_pInterfaces )
	{
		SpriteInterface *pInterface = m_pInterfaces->Child();

		while( pInterface )
		{
			pInterface->Process();
			pInterface = pInterface->Next();
		}
	}

	return;
}

//=============================================================================
// Sprite::DeleteInterfaces
//=============================================================================

void Sprite::DeleteInterfaces( void )
{
	if( m_pInterfaces )
	{
		while( m_pInterfaces->Child() )
			delete m_pInterfaces->Child();

		delete m_pInterfaces;
	}

	return;
}

//=============================================================================
// Sprite::Update()
//=============================================================================

void Sprite::Update( void )
{
	OnUpdate();

	//==========================================================================
	// Process the moduals attached to the sprite.
	//==========================================================================

	ProcessInterfaces();

	//==========================================================================
	// Being a little bit smarter about how sprites are computed, we have to
	// minimize processor time by not generating sprites that are invisible.
	//==========================================================================

	//==========================================================================
	// If the sprite is invisible, we can avoid processing it.
	//==========================================================================

	if( !FLOATTOINT(m_state.color.a) && !Child() )
		ReleaseHandles();
	else
	{
		//=======================================================================
		// However, if the group alpha is zero, none of the child sprites are
		// visible, therefor we can avoid processing the entire familycolor.
		//=======================================================================

		if( !m_state.sprite.visible || FLOATTOINT(m_state.familycolor.a) == 0 )
			ReleaseHandlesRecursively();
		else
		{
			//====================================================================
			// The sprite is added to the draw list.  Sprites in the draw list
			// we be processed and displayed at the end of a frame.
			//====================================================================

			AddToDrawList();

			HudObject::Update();
		}
	}

	return;
}

//=============================================================================
// Sprite::Draw() : Compute the appearance of the sprite and place it on screen.
//						: Parent sprites MUST be drawn before child sprites, because
//						: child sprites depend on the result of its parent's matrix
//						: calculation.
//=============================================================================

void Sprite::Draw( void )
{
	SetDrawPending( false );

	if( !m_state.sprite.visible || (m_state.color.a == 0 && !Child() ) )
		ReleaseHandles();
	else
	{
		OnDraw();

		//==========================================================================
		// Finally, calculate each of the sprites vertices, the texture coordinates,
		// and vertex colors.
		//==========================================================================

		ComputeSprite();
	}

	return;
}

//=============================================================================
// Sprite::AddToDrawList() : Add a sprite to the draw list.
//=============================================================================

void Sprite::AddToDrawList( void )
{
	if( !DrawPending() )
	{
		if( m_pDrawList == 0 )
			m_pDrawList = this;

		if( m_pDrawTail != 0 )
			m_pDrawTail->m_pDrawNext = this;

		m_pDrawTail = this;
		m_pDrawNext = 0;
		SetDrawPending( true );
	}

	return;
}

//=============================================================================
// Sprite::RemoveFromDrawList() : Pull a sprite from the draw list.
//										  : This is slow, don't do it frequently.
//=============================================================================

void Sprite::RemoveFromDrawList( void )
{
	//==========================================================================
	// First, check if a draw is pending.
	//==========================================================================

	if( DrawPending() )
	{
		//=======================================================================
		// See if this sprite is first item in the draw list, and if so, check to
		// see if it is also the only item in the draw list.
		//=======================================================================

		if( m_pDrawList == this )
		{
			if( m_pDrawTail == this )
				m_pDrawList = m_pDrawTail = 0;
			else
				m_pDrawList = m_pDrawNext;
		}
		else
		if( m_pDrawList )
		{
			//====================================================================
			// The sprite is not at the top of the draw list, so we have to search
			// for it.
			//====================================================================

			Sprite *pSprite = m_pDrawList;

			while( pSprite->m_pDrawNext != this )
			{
				pSprite = pSprite->m_pDrawNext;

				if( !pSprite )
					return;
			}

			if( m_pDrawTail == this )
				m_pDrawTail = pSprite;

			pSprite->m_pDrawNext = m_pDrawNext;
		}
	}

	return;
}

//=============================================================================
// Sprite::SetTextureSet() : Fetch the dimensions to the new texture.  It has
//									: been optimized to void unnecessary calls to the
//									: texture manager.
//=============================================================================

void Sprite::SetTextureSet( t_Handle textureset, int texture, int cell, int cols, int rows )
{
	//==========================================================================
	// Reinitialize everything when the texture set is invalid
	//==========================================================================

	if( textureset == INVALID_HANDLE )
	{
		m_status.texture.texturedatabase = NULL;
		m_state.textureset.handle = INVALID_HANDLE;
		m_state.textureset.texture = 0;
		m_state.textureset.cell = 0;
		m_state.textureset.cols = 1;
		m_state.textureset.rows = 1;
		m_state.textureset.sizex = 0;
		m_state.textureset.sizey = 0;
	}
	else
	{
		//==========================================================================
		// Call dblGetCheckData only when the texture set is valid AND it has changed.
		//==========================================================================

		m_status.texture.texturedatabase = (TextureDatabase *) dblGetChunkData( DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, textureset );

		if( !m_status.texture.texturedatabase )
			m_status.texture.texturedatabase = (TextureDatabase *) dblGetChunkData( DB_MERGED_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, textureset );

		//==========================================================================
		// Proceed only if the texture set is valid.
		//==========================================================================

		if( m_status.texture.texturedatabase )
		{
			if( texture != TS_NO_TEXTURE )
				texture %= m_status.texture.texturedatabase->TextureCount();

			m_state.textureset.cell = cell;
			m_state.textureset.cols = cols;
			m_state.textureset.rows = rows;

			if( m_state.textureset.handle != textureset || m_state.textureset.texture != texture )
			{
				m_state.textureset.handle = textureset;
				m_state.textureset.texture = texture;

				//====================================================================
				// Looks like either the texture set or texture index has changed, so
				// update the texture struct accordingly.
				//====================================================================

				s16 x, y;
				s16 sizex = 0;
				s16 sizey = 0;

				m_state.textureset.sizex = 0;
				m_state.textureset.sizey = 0;

				if( m_state.textureset.texture == TS_NO_TEXTURE )
				{
					m_state.textureset.sizex = 64;
					m_state.textureset.sizey = 64;
				}
				else
				{
					if( m_status.texture.texturedatabase->PieceCount() == 0 )
					{
						//=================================================================
						// Pretty easy for a non-merged texture set.
						//=================================================================

						Texture* texture = m_status.texture.texturedatabase->GetTexture( (u16) m_state.textureset.texture);
						ASSERT_PTR(texture);
						Image* img = texture->GetImage();
						ASSERT_PTR(img);


						//=================================================================
						// Check if the texture has been stored horizontally rather than
						// vertically.  This can happen when a texture is taller than it is
						// wide.
						//=================================================================

						if( img->GetFlags() & Image::UVREVERSED )
						{
							m_state.textureset.sizex = (int) img->GetHeight();
							m_state.textureset.sizey = (int) img->GetWidth();
						}
						else
						{
							m_state.textureset.sizex = (int) img->GetWidth();
							m_state.textureset.sizey = (int) img->GetHeight();
						}
					}
					else
					{
						//=================================================================
						// A bit more complicated for a merged texture set.
						//=================================================================

						//=================================================================
						// TODO: Merged texture sets are not checking for UVREVERSED flag.
						// Check if the texture has been stored horizontally rather than
						// vertically.  This can happen when a texture is taller than it is
						// wide.
						//=================================================================

						int Pieces = (int) m_status.texture.texturedatabase->GetPiece(texture)->u16Pieces;

						//=================================================================
						// Look at each piece in the whole texture to determine the overall
						// size of the texture.
						//=================================================================

						for( int t = 0; t <= Pieces; t++ )
						{
							x = m_status.texture.texturedatabase->GetPiece(texture+t)->s16HotSpotX + m_status.texture.texturedatabase->GetPiece(texture+t)->s16Width;
							y = m_status.texture.texturedatabase->GetPiece(texture+t)->s16HotSpotY + m_status.texture.texturedatabase->GetPiece(texture+t)->s16Height;

							if( sizex < x ) sizex = x;
							if( sizey < y ) sizey = y;
						}

						m_state.textureset.sizex = (int) sizex;
						m_state.textureset.sizey = (int) sizey;
					}
				}
			}
		}
	}


	return;
}

//=============================================================================
// Sprite::SetTexture()
//=============================================================================

void Sprite::SetTexture( int texture, int cell, int cols, int rows )
{
	SetTextureSet( m_state.textureset.handle, texture, cell, cols, rows );
}

//=============================================================================
// Sprite::SelectCell() : Selects the desired cell in an ordered sprite table.
//=============================================================================

void Sprite::SelectCell( int cell )
{
	m_state.textureset.cell = cell % (m_state.textureset.cols * m_state.textureset.rows);
}

//=============================================================================
// Sprite::GetTextureSet() : Returns a handle to the current texture set.
//=============================================================================

t_Handle Sprite::GetTextureSet( void )
{
	return m_state.textureset.handle;
}

//=============================================================================
// Sprite::GetTexture() : Returns the index into the current texture set.
//=============================================================================

int Sprite::GetTexture( void )
{
	return m_state.textureset.texture;
}

//=============================================================================
// Sprite::ClearTextureSet() : Clears the texture and the sprite becomes blank.
//=============================================================================

void Sprite::ClearTextureSet( bool force )
{
	SetTextureSet( INVALID_HANDLE, 0 );
	if (force)
		ComputeTexture();
}

//=============================================================================
// Sprite::SetPosition()
//=============================================================================

void Sprite::SetPosition( float x, float y )
{
	m_state.position.x = x;
	m_state.position.y = y;
}

//=============================================================================
// Sprite::GetPosition()
//=============================================================================

void Sprite::GetPosition( float &x, float &y )
{
	x = m_state.position.x;
	y = m_state.position.y;
}

//=============================================================================
// Sprite::SetPositionX
//=============================================================================

void Sprite::SetPositionX( float x )
{
	m_state.position.x = x;
}

//=============================================================================
// Sprite::SetPositionY
//=============================================================================

void Sprite::SetPositionY( float y )
{
	m_state.position.y = y;
}

//=============================================================================
// Sprite::GetPositionX
//=============================================================================

float Sprite::GetPositionX( void )
{
	return m_state.position.x;
}

//=============================================================================
// Sprite::GetPositionY
//=============================================================================

float Sprite::GetPositionY( void )
{
	return m_state.position.y;
}

//=============================================================================
// Sprite::SetScreenPosition()
//=============================================================================

void Sprite::SetScreenPosition( float x, float y )
{
	ScreenPositionToRelativePosition( x, y );

	SetPosition( x, y );
}

//=============================================================================
// Sprite::GetSpritePosition()
//=============================================================================

void Sprite::GetScreenPosition( float &x, float &y )
{
	x = m_state.position.x;
	y = m_state.position.y;

	RelativePositionToScreenPosition( x, y );
}

//=============================================================================
// Sprite::ScreenPosition()
//=============================================================================

void Sprite::ScreenPosition( float x, float y )
{
	SetPosition( x * SimulatedScreenWidth(), y * SimulatedScreenHeight() );
}

//=============================================================================
// Sprite::SetOrigin()
//=============================================================================

void Sprite::SetOrigin( float x, float y )
{
	m_state.origin.x = x;
	m_state.origin.y = y;
}

//=============================================================================
// Sprite::SetOffset()
//=============================================================================

void Sprite::SetOffset( float x, float y )
{
	m_state.offset.x = x;
	m_state.offset.y = y;
}

//=============================================================================
// Sprite::GetOffset()
//=============================================================================

void Sprite::GetOffset( float &x, float &y )
{
	x = m_state.offset.x;
	y = m_state.offset.y;
}

//=============================================================================
// Sprite::SetScale()
//=============================================================================

void Sprite::SetScale( float x, float y )
{
	m_state.scale.x = x;
	m_state.scale.y = y;
}

//=============================================================================
// Sprite::UseTextureSize()
//=============================================================================

void Sprite::UseTextureSize( void )
{
	m_state.size.x = (float) m_state.textureset.sizex;
	m_state.size.y = (float) m_state.textureset.sizey;
}

//=============================================================================
// Sprite::GetTextureSize()
//=============================================================================

void Sprite::GetTextureSize( float &width, float &height )
{
	width = (float) m_state.textureset.sizex;
	height = (float) m_state.textureset.sizey;
}

//=============================================================================
// Sprite::SetSize()
//=============================================================================

void Sprite::SetSize( float width, float height )
{
	SetWidth( width ), SetHeight( height );
}

//=============================================================================
// Sprite::SetWidth()
//=============================================================================

void Sprite::SetWidth( float width )
{
	m_state.size.x = width;
}

//=============================================================================
// Sprite::SetHeight()
//=============================================================================

void Sprite::SetHeight( float height )
{
	m_state.size.y = height;
}

//=============================================================================
// Sprite::Grow()
//=============================================================================

void Sprite::Grow( float width, float height )
{
	SetSize( m_state.size.x + width, m_state.size.y + height );
}

//=============================================================================
// Sprite::SetRotation()
//=============================================================================

void Sprite::SetRotation( float degrees )
{
	m_state.rotation.z = degrees;
}

//=============================================================================
// Sprite::SetPriority()
//=============================================================================

void Sprite::SetPriority( int priority )
{
	m_state.sprite.priority = priority;
}

//=============================================================================
// Sprite::Move()
//=============================================================================

void Sprite::Move( float x, float y )
{
	SetPosition( m_state.position.x + x, m_state.position.y + y );
}

//=============================================================================
// Sprite::Scale()
//=============================================================================

void Sprite::Scale( float x, float y )
{
	SetScale( m_state.scale.x * x, m_state.scale.y * y );
}

//=============================================================================
// Sprite::Rotate()
//=============================================================================

void Sprite::Rotate( float degrees )
{
	SetRotation( m_state.rotation.z + degrees );
}

//=============================================================================
// Sprite::CopyColor()
//=============================================================================

void Sprite::CopyColor( Sprite *pSource )
{
	if( pSource )
		m_state.color.color = pSource->m_state.color.color;

	return;
}

//=============================================================================
// Sprite::SetColor()
//=============================================================================

void Sprite::SetColor( float red, float green, float blue, float alpha )
{
	SetColor( (int) (  red * 255.0f),
				 (int) (green * 255.0f),
				 (int) ( blue * 255.0f),
				 (int) (alpha * 255.0f) );
}

//=============================================================================
// Sprite::SetColor()
//=============================================================================

void Sprite::SetColor( int red, int green, int blue, int alpha )
{
	u32 color32 = ((alpha & 0xff) << 24)|
					  ((  red & 0xff) << 16)|
					  ((green & 0xff) <<  8)|
					  (( blue & 0xff) <<  0);

	SetColor( color32 );
}

//=============================================================================
// Sprite::SetColor()
//=============================================================================

void Sprite::SetColor( int color32 )
{
	m_state.color.color = color32;
}

//=============================================================================
// Sprite::SetAlpha()
//=============================================================================

void Sprite::SetAlpha( float alpha )
{
	m_state.color.a = (u8) (alpha * 255.0f);
}

//=============================================================================
// Sprite::SetFamilyColor()
//=============================================================================

void Sprite::SetFamilyColor( float red, float green, float blue, float alpha )
{
	SetFamilyColor( (int) (  red * 255.0f),
						 (int) (green * 255.0f),
						 (int) ( blue * 255.0f),
						 (int) (alpha * 255.0f) );
}

//=============================================================================
// Sprite::SetFamilyColor()
//=============================================================================

void Sprite::SetFamilyColor( int red, int green, int blue, int alpha )
{
	u32 color32 = ((alpha & 0xff) << 24)|
					  ((  red & 0xff) << 16)|
					  ((green & 0xff) <<  8)|
					  (( blue & 0xff) <<  0);

	SetFamilyColor( color32 );
}

//=============================================================================
// Sprite::SetFamilyColor()
//=============================================================================

void Sprite::SetFamilyColor( u32 color32 )
{
	m_state.familycolor.color = color32;
}

//=============================================================================
// Sprite::SetVertexColor()
//=============================================================================

void Sprite::SetVertexColor( int vertex, float red, float green, float blue, float alpha )
{
	SetVertexColor( vertex, (int) (  red * 255.0f),
									(int) (green * 255.0f),
									(int) ( blue * 255.0f),
									(int) (alpha * 255.0f) );
}

//=============================================================================
// Sprite::SetVertexColor()
//=============================================================================

void Sprite::SetVertexColor( int vertex, int red, int green, int blue, int alpha )
{
	u32 color32 = ((alpha & 0xff) << 24)|
					  ((  red & 0xff) << 16)|
					  ((green & 0xff) <<  8)|
					  (( blue & 0xff) <<  0);

	SetVertexColor( vertex, color32 );
}

//=============================================================================
// Sprite::SetVertexColor()
//=============================================================================

void Sprite::SetVertexColor( int vertex, u32 color32 )
{
	m_state.vertexcolor[vertex&3].color = color32;
}

//=============================================================================
// Sprite::SetVertexPosition()
//=============================================================================

void Sprite::SetVertexPosition( int vertex, float x, float y )
{
	m_state.vertexposition[vertex&3].x = x;
	m_state.vertexposition[vertex&3].y = y;
}

//=============================================================================
// Sprite::SetTextureCoordinates()
//=============================================================================

void Sprite::SetTextureCoordinates( int vertex, float u, float v )
{
	m_state.texcoords[vertex&3].x = u;
	m_state.texcoords[vertex&3].y = v;
}

//=============================================================================
// Sprite::SetTextureRect()
//=============================================================================

void Sprite::SetTextureRect( float u1, float v1, float u2, float v2 )
{
	m_state.texcoords[0].x = u1, m_state.texcoords[0].y = v1;
	m_state.texcoords[1].x = u2, m_state.texcoords[1].y = v1;
	m_state.texcoords[2].x = u2, m_state.texcoords[2].y = v2;
	m_state.texcoords[3].x = u1, m_state.texcoords[3].y = v2;
}

//=============================================================================
// Sprite::SetBaseColor()
//=============================================================================

void Sprite::SetBaseColor( float r, float g, float b, float a )
{
	SetColor( r, g, b, a );
}

//=============================================================================
// Sprite::SetBaseColor()
//=============================================================================

void Sprite::SetBaseColor( int r, int g, int b, int a )
{
	SetColor( r, g, b, a );
}

//=============================================================================
// Sprite::SetBaseColor
//=============================================================================

void Sprite::SetBaseColor( u32 color )
{
	int r = (color>>16) & 0xff;
	int g = (color>> 8) & 0xff;
	int b = (color>> 0) & 0xff;
	int a = (color>>24) & 0xff;

	SetBaseColor( r, g, b, a );
}

//=============================================================================
// Sprite::SetBlendMode
//=============================================================================

void Sprite::SetBlendMode( u8 bm )
{
	m_state.sprite.blendmode = bm;
}

//=============================================================================
// Sprite::SetBlendMode
//=============================================================================

u8 Sprite::GetBlendMode( void )
{
	return m_state.sprite.blendmode;
}

//=============================================================================
// Sprite::Toggle()
//=============================================================================

void Sprite::Toggle( void )
{
	if( m_state.sprite.visible )
		Hide();
	else
		Show();

	return;
}

//=============================================================================
// Sprite::Show()
//=============================================================================

void Sprite::Show( bool show )
{
	m_state.sprite.visible = show;
}

//=============================================================================
// Sprite::ShadowOn()
//=============================================================================

void Sprite::ShadowOn( void )
{
	m_state.shadow.enabled = true;
}

//=============================================================================
// Sprite::ShadowOff()
//=============================================================================

void Sprite::ShadowOff( void )
{
	m_state.shadow.enabled = false;
}

//=============================================================================
// Sprite::ShadowPosition()
//=============================================================================

void Sprite::ShadowPosition( s8 x, s8 y )
{
	m_state.shadow.x = x;
	m_state.shadow.y = y;
}

//=============================================================================
// Sprite::ShadowIntensity()
//=============================================================================

void Sprite::ShadowIntensity( u8 intensity )
{
	m_state.shadow.intensity = intensity;
}

//=============================================================================
// Sprite::ShadowSettings()
//=============================================================================

void Sprite::ShadowSettings( bool Enabled, u8 Intensity /*= 255*/, s8 x /*= 2*/, s8 y /*= 2*/ )
{
	m_state.shadow.enabled = Enabled;
	m_state.shadow.intensity = Intensity;
	m_state.shadow.x = x;
	m_state.shadow.y = y;
}

//=============================================================================
// Sprite::InitAnim()
//=============================================================================

void Sprite::InitAnim( char priority )
{
//	ASSERT(m_anim==0);

	if( !m_anim )
	{
		m_anim = new SpriteAnim;

		if( m_anim )
		{
			m_anim->SetTarget( this );
			m_anim->SetPriority( priority );
		}
	}

	if( m_anim )
		m_anim->Clear();

	return;
}

//=============================================================================
// Sprite::KillAnim()
//=============================================================================

void Sprite::KillAnim( void )
{
	if( m_anim )
		delete m_anim;

	m_anim = 0;
}

//=============================================================================
// Sprite::IsVisible()
//=============================================================================

bool Sprite::IsVisible( void )
{
	if( !m_state.sprite.visible )
		return false;

	if( m_state.color.a == 0 )
		return false;

	if( m_state.familycolor.a == 0 )
		return false;

	return true;
}

//=============================================================================
// Sprite::IsHidden()
//=============================================================================

bool Sprite::IsHidden( void )
{
	return !IsVisible();
}

//=============================================================================
// Sprite::GetAlpha()
//=============================================================================

float Sprite::GetAlpha( void )
{
	return m_state.color.a * (1.0f/255.0f);
}

//=============================================================================
// Sprite::GetBaseColor
//=============================================================================

void Sprite::GetBaseColor( int &r, int &g, int &b, int &a )
{
	r = m_state.color.r;
	g = m_state.color.g;
	b = m_state.color.b;
	a = m_state.color.a;
}

//=============================================================================
// Sprite::GetBaseColor
//=============================================================================

void Sprite::GetBaseColor( float &r, float &g, float &b, float &a )
{
	const float n = 1.0f/255.0f;

	r = m_state.color.r * n;
	g = m_state.color.g * n;
	b = m_state.color.b * n;
	a = m_state.color.a * n;
}

//=============================================================================
// Sprite::GetFamilyColor
//=============================================================================

void Sprite::GetFamilyColor( int &r, int &g, int &b, int &a )
{
	r = m_state.familycolor.r;
	g = m_state.familycolor.g;
	b = m_state.familycolor.b;
	a = m_state.familycolor.a;
}

//=============================================================================
// Sprite::GetFamilyColor
//=============================================================================

void Sprite::GetFamilyColor( float &r, float &g, float &b, float &a )
{
	const float n = 1.0f/255.0f;

	r = m_state.familycolor.r * n;
	g = m_state.familycolor.g * n;
	b = m_state.familycolor.b * n;
	a = m_state.familycolor.a * n;
}

//=============================================================================
// Sprite::DrawPending()
//=============================================================================

bool Sprite::DrawPending( void )
{
	if( m_StateFlags & STATE_DRAWPENDING )
		return true;

	return false;
}

//=============================================================================
// Sprite::SetDrawPending()
//=============================================================================

void Sprite::SetDrawPending( bool TrueOrFalse )
{
	if( TrueOrFalse == true )
		m_StateFlags |= STATE_DRAWPENDING;
	else
		m_StateFlags &= ~STATE_DRAWPENDING;

	return;
}

//=============================================================================
// Sprite::GetHandle()
//=============================================================================

t_Handle Sprite::GetHandle( void )
{
	return m_status.handles.sprite;
}

//=============================================================================
// Sprite::GetWidth()
//=============================================================================

float Sprite::GetWidth( void )
{
	return m_state.size.x;
}

//=============================================================================
// Sprite::GetHeight()
//=============================================================================

float Sprite::GetHeight( void )
{
	return m_state.size.y;
}

//=============================================================================
// Sprite:GetRotation()
//=============================================================================

float Sprite::GetRotation( void )
{
	return m_state.rotation.z;
}

//=============================================================================
// Sprite::GetSize
//=============================================================================

void Sprite::GetSize( float &width, float &height )
{
	width = m_state.size.x;
	height = m_state.size.y;
}

//=============================================================================
// Sprite::GetOrigin
//=============================================================================

void Sprite::GetOrigin( float &x, float &y )
{
	x = m_state.origin.x;
	y = m_state.origin.y;
}

//=============================================================================
// Sprite::GetScale
//=============================================================================

void Sprite::GetScale( float &x, float &y )
{
	x = m_state.scale.x;
	y = m_state.scale.y;
}

//=============================================================================
// Sprite::ComputeTransform
//=============================================================================

void Sprite::ComputeTransform( Graphics4x4 &transform )
{
	//==========================================================================
	// Calculate the matrix transformations in this order:
	// Origin, scale, rotate, translate.
	//==========================================================================

	//==========================================================================
	// NOTE: Setting up the matrix manually is much faster than using matrix
	//			functions.
	//==========================================================================

	float w = m_state.size.x;
	float h = m_state.size.y;

	if( FLOATTOINT(w) == 0 ) w = (float) m_state.textureset.sizex;
	if( FLOATTOINT(h) == 0 ) h = (float) m_state.textureset.sizey;

	transform.r00 = m_state.scale.x;
	transform.r01 = 0.0f;
	transform.r02 = 0.0f;
	transform.u0  = 0.0f;
	transform.r10 = 0.0f;
	transform.r11 = m_state.scale.y;
	transform.r12 = 0.0f;
	transform.u1  = 0.0f;
	transform.r20 = 0.0f;
	transform.r21 = 0.0f;
	transform.r22 = 1.0f;
	transform.u2  = 0.0f;
	transform.tx  = w * -m_state.origin.x * m_state.scale.x;
	transform.ty  = h * -m_state.origin.y * m_state.scale.y;
	transform.tz  = 0.0f;
	transform.w	  = 1.0f;

	//==========================================================================
	// NOTE: The rotation matrix is skipped if the angle of rotation is zero.
	//==========================================================================

	if( FLOATTOINT(m_state.rotation.z) == 0 )
	{
		transform.tx += m_state.position.x + m_state.offset.x;
		transform.ty += m_state.position.y + m_state.offset.y;
	}
	else
	{
		transform.CatZRotation( Math::Deg2Rad( m_state.rotation.z ) );

		//==========================================================================
		// The translation matrix is appended AFTER the rotation.
		//==========================================================================

		transform.CatTranslation( m_state.position.x + m_state.offset.x,
										  m_state.position.y + m_state.offset.y,
										  0.0f );
	}

	return;
}

//=============================================================================
// Sprite::ComputeVertices
//=============================================================================

void Sprite::ComputeVertices( const Graphics4x4 &transform, Vector3 *vertices )
{
	float w = m_state.size.x;
	float h = m_state.size.y;

	if( FLOATTOINT(w) == 0 ) w = (float) m_state.textureset.sizex;
	if( FLOATTOINT(h) == 0 ) h = (float) m_state.textureset.sizey;

	for( int n = 0; n < 4; n++ )
		vertices[n] = transform.Transform( m_state.clipcoords[n].x * w + m_state.vertexposition[n].x,
													  m_state.clipcoords[n].y * h + m_state.vertexposition[n].y, 0.0f );

	return;
}

//=============================================================================
// Sprite::GetLocalTransform()
//=============================================================================

Graphics4x4 Sprite::GetLocalTransform( void )
{
	Graphics4x4 transform;

	ComputeTransform( transform );

	return transform;
}

//=============================================================================
// Sprite::GetParentTransform
//=============================================================================

Graphics4x4 Sprite::GetParentTransform( void )
{
	if( Parent() )
		return Parent()->GetFamilyTransform();

	Graphics4x4 identity;
	identity.Identity();

	return identity;
}

//=============================================================================
// Sprite::GetFamilyTransform
//=============================================================================

Graphics4x4 Sprite::GetFamilyTransform( void )
{
	Sprite *pParent = Parent();

	if( !pParent )
		return GetLocalTransform();

	return GetLocalTransform() * pParent->GetFamilyTransform();
}

//=============================================================================
// Sprite::GetFamilyTransformInverse()
//=============================================================================

Graphics4x4 Sprite::GetFamilyTransformInverse( void )
{
	Graphics4x4 transform;

	GetFamilyTransform().Invert( transform );

	return transform;
}

//=============================================================================
// Sprite::GetParentRotation()
//=============================================================================

float Sprite::GetParentRotation( void )
{
	if( !Parent() )
		return 0.0f;

	return Parent()->GetFamilyRotation();
}

//=============================================================================
// Sprite::GetFamilyRotation()
//=============================================================================

float Sprite::GetFamilyRotation( void )
{
	if( !Parent() )
		return GetRotation();

	return GetRotation() + Parent()->GetFamilyRotation();
}

//=============================================================================
// Sprite::GetSpriteVertices()
//=============================================================================

void Sprite::GetSpriteVertices( Vector3 *vertices )
{
	Graphics4x4 transform = GetFamilyTransform();

	if( vertices )
		ComputeVertices( transform, vertices );

	return;
}

//=============================================================================
// Sprite::GetSpriteRect()
//=============================================================================

void Sprite::GetSpriteRect( float &x1, float &y1, float &x2, float &y2 )
{
	Vector3 vertices[4];
	int n;

	GetSpriteVertices( vertices );

	x1 = x2 = vertices[0].x();
	y1 = y2 = vertices[0].y();

	for( n = 1; n < 4; n++ )
	{
		if( x1 > vertices[n].x() ) x1 = vertices[n].x();
		if( y1 > vertices[n].y() ) y1 = vertices[n].y();
		if( x2 < vertices[n].x() ) x2 = vertices[n].x();
		if( y2 < vertices[n].y() ) y2 = vertices[n].y();
	}

	return;
}

//=============================================================================
// Sprite::GetFamilyRect()
//=============================================================================

void Sprite::GetFamilyRect( float &x1, float &y1, float &x2, float &y2 )
{
	float x3, y3, x4, y4;

	GetSpriteRect( x1, y1, x2, y2 );

	Sprite *pChild = Child();

	while( pChild )
	{
		pChild->GetFamilyRect( x3, y3, x4, y4 );

		if( x1 > x3 ) x1 = x3;
		if( y1 > y3 ) y1 = y3;
		if( x2 < x4 ) x2 = x4;
		if( y2 < y4 ) y2 = y4;

		pChild = pChild->Next();
	}

	return;
}

//=============================================================================
// Sprite::GetSortedPriority()
//=============================================================================

int Sprite::GetSortedPriority( void )
{
	int priority = m_state.sprite.priority;

	Sprite *pParent = Parent();

	while( pParent )
	{
		priority = priority + 100 + pParent->m_state.sprite.priority;

		pParent = pParent->Parent();
	}

	if( priority > 65535 )
		priority = 65535;

	return priority;
}

//=============================================================================
// Sprite::GetSortPriority
//=============================================================================

int Sprite::GetSortPriority( void )
{
	return m_state.sprite.priority;
}

//=============================================================================
// Sprite::GetFamilyPriority
//=============================================================================

int Sprite::GetFamilyPriority( void )
{
	int priority1 = GetSortedPriority();
	int priority2 = 0;

	Sprite* child = Child();

	while( child )
	{
		priority2 = child->GetFamilyPriority();

		if( priority1 < priority2 )
			priority1 = priority2;

		child = child->Next();
	}

	return priority1;
}

//=============================================================================
// Sprite::ScreenPositionToRelativePosition
//=============================================================================

void Sprite::ScreenPositionToRelativePosition( float &x, float &y )
{
	Graphics4x4 transform = GetParentTransform();
	Graphics4x4 inverse;

	transform.Invert( inverse );

	Vector3 position = inverse.Transform( x, y, 0.0f );

	x = position.x();
	y = position.y();
}

//=============================================================================
// Sprite::RelativePositionToScreenPosition
//=============================================================================

void Sprite::RelativePositionToScreenPosition( float &x, float &y )
{
	Graphics4x4 transform = GetParentTransform();

	Vector3 position = transform.Transform( x, y, 0.0f );

	x = position.x();
	y = position.y();
}

//=============================================================================
// Sprite::PointInSprite
//=============================================================================

bool Sprite::PointInSprite( float x, float y  )
{
	Graphics4x4 transform = GetFamilyTransform();
	Vector3 vertices[4];

	ComputeVertices( transform, vertices );

	return false;
}

//=============================================================================
// Sprite::Crop()
//=============================================================================

void Sprite::Crop( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4 )
{
	m_state.clipcoords[0].x = x1;
	m_state.clipcoords[0].y = y1;
	m_state.clipcoords[1].x = x2;
	m_state.clipcoords[1].y = y2;
	m_state.clipcoords[2].x = x3;
	m_state.clipcoords[2].y = y3;
	m_state.clipcoords[3].x = x4;
	m_state.clipcoords[3].y = y4;
}

//=============================================================================
// Sprite::CropRect()
//=============================================================================

void Sprite::CropRect( float x1, float y1, float x2, float y2 )
{
	Crop( x1, y1, x2, y1, x2, y2, x1, y2 );
}

//=============================================================================
// Sprite::GetStateStruct
//=============================================================================

void Sprite::GetStateStruct( SPRITESTATESTRUCT &state )
{
	state = m_state;
}

//=============================================================================
// Sprite::GetSpriteFormat
//=============================================================================

#define SPRITE_LOADSAVE_VERSION1 (0x100)
#define SPRITE_LOADSAVE_VERSION2 (0x101)
#define SPRITE_LOADSAVE_VERSION3 (0x102)
#define SPRITE_LOADSAVE_VERSION4 (0x103)
#define SPRITE_LOADSAVE_VERSION5 (0x104)
#define SPRITE_LOADSAVE_VERSION6 (0x105)
#define SPRITE_LOADSAVE_VERSION7 (0x106)
#define LOADSAVE_LATEST_VERSION (SPRITE_LOADSAVE_VERSION7)

#define SPRITEFORMAT_TEXTURESET		(1<<0)
#define SPRITEFORMAT_POSITION			(1<<1)
#define SPRITEFORMAT_SIZE				(1<<2)
#define SPRITEFORMAT_SCALE				(1<<3)
#define SPRITEFORMAT_ORIGIN			(1<<4)
#define SPRITEFORMAT_OFFSET			(1<<5)
#define SPRITEFORMAT_ROTATION			(1<<6)
#define SPRITEFORMAT_BASECOLOR		(1<<7)
#define SPRITEFORMAT_FAMILYCOLOR		(1<<8)
#define SPRITEFORMAT_VERTEXCOLOR		(1<<9)
#define SPRITEFORMAT_VERTEXPOSITION	(1<<10)
#define SPRITEFORMAT_TEXCOORDS		(1<<11)
#define SPRITEFORMAT_CLIPCOORDS		(1<<12)
#define SPRITEFORMAT_MISC				(1<<13)
#define SPRITEFORMAT_SHADOW			(1<<14)
#define SPRITEFORMAT_TRANSFORM		(1<<15)

int Sprite::GetSpriteFormat( SPRITESTATESTRUCT &state )
{
	int Format = 0;

	if(state.textureset.texture != 0 ||
		state.textureset.cell    != 0 ||
		state.textureset.cols    != 1 ||
		state.textureset.rows    != 1 ||
		state.textureset.sizex   != 0 ||
		state.textureset.sizey   != 0 )
	{
		Format |= SPRITEFORMAT_TEXTURESET;
	}

	if(state.position.x != 0.0f || state.position.y != 0.0f )
		Format |= SPRITEFORMAT_POSITION;

	if( state.size.x != 0.0f || state.size.y != 0.0f )
		Format |= SPRITEFORMAT_SIZE;

	if( state.scale.x != 1.0f || state.scale.y != 1.0f )
		Format |= SPRITEFORMAT_SCALE;

	if( state.origin.x != 0.0f || state.origin.y != 0.0f )
		Format |= SPRITEFORMAT_ORIGIN;

	if( state.offset.x != 0.0f || state.offset.y != 0.0f )
		Format |= SPRITEFORMAT_OFFSET;

	if( state.rotation.z != 0.0f )
		Format |= SPRITEFORMAT_ROTATION;

	// the default base color is white fully opaque (all 255)

	if(state.color.a != 255 ||
		state.color.r != 255 ||
		state.color.g != 255 ||
		state.color.b != 255 )
	{
		Format |= SPRITEFORMAT_BASECOLOR;
	}

	// the default family color is white fully opaque (all 255)

	if(state.familycolor.a != 255 ||
		state.familycolor.r != 255 ||
		state.familycolor.g != 255 ||
		state.familycolor.b != 255 )
	{
		Format |= SPRITEFORMAT_FAMILYCOLOR;
	}

	// the default vertex color is white fully opaque (all 255)

	static const float texcoords_x[4] = {0.0f, 1.0f, 1.0f, 0.0f};
	static const float texcoords_y[4] = {0.0f, 0.0f, 1.0f, 1.0f};
	static const float clipcoords_x[4] = {0.0f, 1.0f, 1.0f, 0.0f};
	static const float clipcoords_y[4] = {0.0f, 0.0f, 1.0f, 1.0f};

	for( int v = 0; v < 4; v++ )
	{
		if( state.vertexcolor[v].a != 255 || state.vertexcolor[v].r != 255 || state.vertexcolor[v].g != 255 || state.vertexcolor[v].b != 255 )
			Format |= SPRITEFORMAT_VERTEXCOLOR;

		if( state.vertexposition[v].x != 0.0f || state.vertexposition[v].y != 0.0f )
			Format |= SPRITEFORMAT_VERTEXPOSITION;

		if( state.texcoords[v].x != texcoords_x[v] || state.texcoords[v].y != texcoords_y[v] )
			Format |= SPRITEFORMAT_TEXCOORDS;

		if( state.clipcoords[v].x != clipcoords_x[v] || state.clipcoords[v].y != clipcoords_y[v] )
			Format |= SPRITEFORMAT_CLIPCOORDS;
	}

	if( state.sprite.priority != 0 || state.sprite.visible == 0 || state.sprite.blendmode != SPRITEBLEND_NORMAL )
		Format |= SPRITEFORMAT_MISC;

	if( state.shadow.enabled != 0 || state.shadow.intensity != 255 || state.shadow.x != 2 || state.shadow.y != 2 )
		Format |= SPRITEFORMAT_SHADOW;

	return Format;
}

//=============================================================================
// Sprite::SaveObject
//=============================================================================

bool Sprite::SaveObject( MemoryBank &m, char *texture )
{
	SPRITESTATESTRUCT s;
	memcpy( &s, &m_state, sizeof(s) );

	// clear out the members we don't want to save

	s.textureset.handle = INVALID_HANDLE;

	if( texture == 0 )
		texture = dblGetFileName( m_state.textureset.handle );

	u32 Version = LOADSAVE_LATEST_VERSION;

	if( !m.Write( &Version, sizeof(Version) ) )
		return false;

	if( !m.WriteString( texture ) )
		return false;

	if( !m.Write( &m_ObjectId, sizeof(m_ObjectId) ) )
		return false;

	u32 c = 0;

	if( m_pInterfaces )
		c = m_pInterfaces->GetNumChildren();

	if( !m.Write( &c, sizeof(c) ) )
		return false;

	SpriteInterface *pInterface = 0;

	if( m_pInterfaces )
		pInterface = m_pInterfaces->Child();

	for( u32 i = 0; i < c; i++ )
	{
		if( pInterface )
		{
			if( !m.WriteString( pInterface->Classname() ) )
				return false;

			pInterface->Save(m);
			pInterface = pInterface->Next();
		}
	}

	u32 Format = GetSpriteFormat(s);

	if( !m.Write( &Format, sizeof(Format) ) )
		return false;

	if( Format & SPRITEFORMAT_TEXTURESET )
		if( !m.Write( &s.textureset, sizeof(s.textureset) ) )
			return false;

	if( Format & SPRITEFORMAT_POSITION )
		if( !m.Write( &s.position, sizeof(s.position) ) )
			return false;

	if( Format & SPRITEFORMAT_SIZE )
		if( !m.Write( &s.size, sizeof(s.size) ) )
			return false;

	if( Format & SPRITEFORMAT_SCALE )
		if( !m.Write( &s.scale, sizeof(s.scale) ) )
			return false;

	if( Format & SPRITEFORMAT_ORIGIN )
		if( !m.Write( &s.origin, sizeof(s.origin) ) )
			return false;

	if( Format & SPRITEFORMAT_OFFSET )
		if( !m.Write( &s.offset, sizeof(s.offset) ) )
			return false;

	if( Format & SPRITEFORMAT_ROTATION )
		if( !m.Write( &s.rotation, sizeof(s.rotation) ) )
			return false;

	if( Format & SPRITEFORMAT_BASECOLOR )
		if( !m.Write( &s.color, sizeof(s.color) ) )
			return false;

	if( Format & SPRITEFORMAT_FAMILYCOLOR )
		if( !m.Write( &s.familycolor, sizeof(s.familycolor) ) )
			return false;

	if( Format & SPRITEFORMAT_VERTEXCOLOR )
		if( !m.Write( &s.vertexcolor, sizeof(s.vertexcolor) ) )
			return false;

	if( Format & SPRITEFORMAT_VERTEXPOSITION )
		if( !m.Write( &s.vertexposition, sizeof(s.vertexposition) ) )
			return false;

	if( Format & SPRITEFORMAT_TEXCOORDS )
		if( !m.Write( &s.texcoords, sizeof(s.texcoords) ) )
			return false;

	if( Format & SPRITEFORMAT_CLIPCOORDS )
		if( !m.Write( &s.clipcoords, sizeof(s.clipcoords) ) )
			return false;

	if( Format & SPRITEFORMAT_MISC )
		if( !m.Write( &s.sprite, sizeof(s.sprite) ) )
			return false;

	if( Format & SPRITEFORMAT_SHADOW )
		if( !m.Write( &s.shadow, sizeof(s.shadow) ) )
			return false;

	return true;
}

//=============================================================================
// Sprite::LoadObject
//=============================================================================

bool Sprite::LoadObject( MemoryBank &m, char *texture )
{
	SpriteInterface* pInterface;
	xstring TextureName;
	xstring InterfaceName;
	u32 i, c, Version;
	u8 junk[256];

	if( !m.Read( &Version, sizeof(Version) ) )
		return false;

	if( !m.ReadString( TextureName ) )
		return false;

	if( !m.Read( &m_ObjectId, sizeof(m_ObjectId) ) )
		return false;

	if( Version >= SPRITE_LOADSAVE_VERSION1 )
	{
		if( !m.Read( &c, sizeof(c) ) )
			return false;

		for( i = 0; i < c; i++ )
		{
			if( !m.ReadString( InterfaceName ) )
				return false;

			pInterface = AddInterface( InterfaceName.get(), 0 );

			if( pInterface && Version >= SPRITE_LOADSAVE_VERSION6 )
				pInterface->Load( m );
		}
	}

	if( Version == SPRITE_LOADSAVE_VERSION1 )
	{
		//=======================================================================
		// version1 used to store the m_state structure as a whole chunk.
		// over time, the m_state structure changed so now we have to read it
		// back in piece by piece and toss the old shit out
		//=======================================================================

		//	if( !m.Read( &m_state, sizeof(m_state) ) )
		//		return false; old-1

		m.Read( junk, sizeof(u32) ); // was m_state.textureset.texturedatabase

		if( !m.Read( &m_state.textureset, sizeof(m_state.textureset) ) )
			return false;

		if( !m.Read( &m_state.position.x, sizeof(m_state.position.x) ) )
			return false;

		if( !m.Read( &m_state.position.y, sizeof(m_state.position.y) ) )
			return false;

		if( !m.Read( &m_state.size, sizeof(m_state.size) ) )
			return false;

		if( !m.Read( &m_state.scale, sizeof(m_state.scale) ) )
			return false;

		if( !m.Read( &m_state.origin, sizeof(m_state.origin) ) )
			return false;

		if( !m.Read( &m_state.offset, sizeof(m_state.offset) ) )
			return false;

		if( !m.Read( &m_state.rotation, sizeof(m_state.rotation) ) )
			return false;

		m.Read( junk, sizeof(u32) );  // was m_state.rotation.pad

		if( !m.Read( &m_state.color, sizeof(m_state.color) ) )
			return false;

		if( !m.Read( &m_state.familycolor, sizeof(m_state.familycolor) ) )
			return false;

		if( !m.Read( &m_state.vertexcolor, sizeof(m_state.vertexcolor) ) )
			return false;

		if( !m.Read( &m_state.vertexposition, sizeof(m_state.vertexposition) ) )
			return false;

		if( !m.Read( &m_state.texcoords, sizeof(m_state.texcoords) ) )
			return false;

		if( !m.Read( &m_state.clipcoords, sizeof(m_state.clipcoords) ) )
			return false;

		if( !m.Read( &m_state.sprite, sizeof(m_state.sprite) ) )
			return false;

		m_state.sprite.blendmode = SPRITEBLEND_NORMAL;

		if( !m.Read( &m_state.shadow, sizeof(m_state.shadow) ) )
			return false;

		m.Read( junk, sizeof(u32) ); // was m_state.shadow.pad

		Graphics4x4 not_used_anymore;
		if( !m.Read( &not_used_anymore, sizeof(not_used_anymore) ) )
			return false;
	}
	else
	if( Version == SPRITE_LOADSAVE_VERSION2 )
	{
		//=======================================================================
		// this version compresses the m_state structure by removing the bits we don't need.
		// the leading 32-bit word identifies which bits are present.
		// the previous version saved every structure all the time.
		//=======================================================================

		u32 Format = 0;

		if( !m.Read( &Format, sizeof(Format) ) )
			return false;

		if( Format & SPRITEFORMAT_TEXTURESET )
		{
			m.Read( junk, sizeof(u32) ); // was m_state.textureset.texturedatabase

			if( !m.Read( &m_state.textureset, sizeof(m_state.textureset) ) )
				return false;
		}

		if( Format & SPRITEFORMAT_POSITION )
		{
			if( !m.Read( &m_state.position.x, sizeof(m_state.position.x) ) )
				return false;

			if( !m.Read( &m_state.position.y, sizeof(m_state.position.y) ) )
				return false;
		}

		if( Format & SPRITEFORMAT_SIZE )
			if( !m.Read( &m_state.size, sizeof(m_state.size) ) )
				return false;

		if( Format & SPRITEFORMAT_SCALE )
			if( !m.Read( &m_state.scale, sizeof(m_state.scale) ) )
				return false;

		if( Format & SPRITEFORMAT_ORIGIN )
			if( !m.Read( &m_state.origin, sizeof(m_state.origin) ) )
				return false;

		if( Format & SPRITEFORMAT_OFFSET )
			if( !m.Read( &m_state.offset, sizeof(m_state.offset) ) )
				return false;

		if( Format & SPRITEFORMAT_ROTATION )
		{
			if( !m.Read( &m_state.rotation, sizeof(m_state.rotation) ) )
				return false;

			m.Read( junk, sizeof(u32) ); // was m_state.rotation.pad
		}

		if( Format & SPRITEFORMAT_BASECOLOR )
			if( !m.Read( &m_state.color, sizeof(m_state.color) ) )
				return false;

		if( Format & SPRITEFORMAT_FAMILYCOLOR )
			if( !m.Read( &m_state.familycolor, sizeof(m_state.familycolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXCOLOR )
			if( !m.Read( &m_state.vertexcolor, sizeof(m_state.vertexcolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXPOSITION )
			if( !m.Read( &m_state.vertexposition, sizeof(m_state.vertexposition) ) )
				return false;

		if( Format & SPRITEFORMAT_TEXCOORDS )
			if( !m.Read( &m_state.texcoords, sizeof(m_state.texcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_CLIPCOORDS )
			if( !m.Read( &m_state.clipcoords, sizeof(m_state.clipcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_MISC )
		{
			if( !m.Read( &m_state.sprite, sizeof(m_state.sprite) ) )
				return false;

			m_state.sprite.blendmode = SPRITEBLEND_NORMAL;
			m_state.sprite.reserved0 = 0;
			m_state.sprite.reserved1 = 0;
		}

		if( Format & SPRITEFORMAT_SHADOW )
		{
			if( !m.Read( &m_state.shadow, sizeof(m_state.shadow) ) )
				return false;

			m.Read( junk, sizeof(u32) ); // was m_state.shadow.pad
		}
	}
	else
	if( Version == SPRITE_LOADSAVE_VERSION3 )
	{
		//=======================================================================
		// this version compresses the m_state structure by removing the bits we don't need.
		// the leading 32-bit word identifies which bits are present.
		//	changes from version2 to version are the removal of
		//		m_state.rotation.pad
		//		m_state.shadow.pad
		//=======================================================================

		u32 Format = 0;

		if( !m.Read( &Format, sizeof(Format) ) )
			return false;

		if( Format & SPRITEFORMAT_TEXTURESET )
		{
			m.Read( junk, sizeof(u32) ); // was m_state.textureset.texturedatabase

			if( !m.Read( &m_state.textureset, sizeof(m_state.textureset) ) )
				return false;
		}

		if( Format & SPRITEFORMAT_POSITION )
			if( !m.Read( &m_state.position, sizeof(m_state.position) ) )
				return false;

		if( Format & SPRITEFORMAT_SIZE )
			if( !m.Read( &m_state.size, sizeof(m_state.size) ) )
				return false;

		if( Format & SPRITEFORMAT_SCALE )
			if( !m.Read( &m_state.scale, sizeof(m_state.scale) ) )
				return false;

		if( Format & SPRITEFORMAT_ORIGIN )
			if( !m.Read( &m_state.origin, sizeof(m_state.origin) ) )
				return false;

		if( Format & SPRITEFORMAT_OFFSET )
			if( !m.Read( &m_state.offset, sizeof(m_state.offset) ) )
				return false;

		if( Format & SPRITEFORMAT_ROTATION )
			if( !m.Read( &m_state.rotation, sizeof(m_state.rotation) ) )
				return false;

		if( Format & SPRITEFORMAT_BASECOLOR )
			if( !m.Read( &m_state.color, sizeof(m_state.color) ) )
				return false;

		if( Format & SPRITEFORMAT_FAMILYCOLOR )
			if( !m.Read( &m_state.familycolor, sizeof(m_state.familycolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXCOLOR )
			if( !m.Read( &m_state.vertexcolor, sizeof(m_state.vertexcolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXPOSITION )
			if( !m.Read( &m_state.vertexposition, sizeof(m_state.vertexposition) ) )
				return false;

		if( Format & SPRITEFORMAT_TEXCOORDS )
			if( !m.Read( &m_state.texcoords, sizeof(m_state.texcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_CLIPCOORDS )
			if( !m.Read( &m_state.clipcoords, sizeof(m_state.clipcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_MISC )
		{
			if( !m.Read( &m_state.sprite, sizeof(m_state.sprite) ) )
				return false;

			m_state.sprite.blendmode = SPRITEBLEND_NORMAL;
			m_state.sprite.reserved0 = 0;
			m_state.sprite.reserved1 = 0;
		}

		if( Format & SPRITEFORMAT_TRANSFORM )
		{
			Graphics4x4 not_used_anymore;
			if( !m.Read( &not_used_anymore, sizeof(not_used_anymore) ) )
				return false;
		}
	}
	else
	if( Version == SPRITE_LOADSAVE_VERSION4 )
	{
		//=======================================================================
		// this version compresses the m_state structure by removing the bits we don't need.
		// the leading 32-bit word identifies which bits are present.
		// changes from version3 to version4 are removal of
		//		m_state.textureset.texturedatabase
		//		m_state.transform
		//=======================================================================

		u32 Format = 0;

		if( !m.Read( &Format, sizeof(Format) ) )
			return false;

		if( Format & SPRITEFORMAT_TEXTURESET )
			if( !m.Read( &m_state.textureset, sizeof(m_state.textureset) ) )
				return false;

		if( Format & SPRITEFORMAT_POSITION )
		{
			if( !m.Read( &m_state.position, sizeof(m_state.position) ) )
				return false;

			if( !m.Read( junk, sizeof(float) ) ) // m_state.position.z is gone now
				return false;
		}

		if( Format & SPRITEFORMAT_SIZE )
			if( !m.Read( &m_state.size, sizeof(m_state.size) ) )
				return false;

		if( Format & SPRITEFORMAT_SCALE )
			if( !m.Read( &m_state.scale, sizeof(m_state.scale) ) )
				return false;

		if( Format & SPRITEFORMAT_ORIGIN )
			if( !m.Read( &m_state.origin, sizeof(m_state.origin) ) )
				return false;

		if( Format & SPRITEFORMAT_OFFSET )
			if( !m.Read( &m_state.offset, sizeof(m_state.offset) ) )
				return false;

		if( Format & SPRITEFORMAT_ROTATION )
			if( !m.Read( &m_state.rotation, sizeof(m_state.rotation) ) )
				return false;

		if( Format & SPRITEFORMAT_BASECOLOR )
			if( !m.Read( &m_state.color, sizeof(m_state.color) ) )
				return false;

		if( Format & SPRITEFORMAT_FAMILYCOLOR )
			if( !m.Read( &m_state.familycolor, sizeof(m_state.familycolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXCOLOR )
			if( !m.Read( &m_state.vertexcolor, sizeof(m_state.vertexcolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXPOSITION )
			if( !m.Read( &m_state.vertexposition, sizeof(m_state.vertexposition) ) )
				return false;

		if( Format & SPRITEFORMAT_TEXCOORDS )
			if( !m.Read( &m_state.texcoords, sizeof(m_state.texcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_CLIPCOORDS )
			if( !m.Read( &m_state.clipcoords, sizeof(m_state.clipcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_MISC )
		{
			if( !m.Read( &m_state.sprite, sizeof(m_state.sprite) ) )
				return false;

			m_state.sprite.blendmode = SPRITEBLEND_NORMAL;
			m_state.sprite.reserved0 = 0;
			m_state.sprite.reserved1 = 0;
		}

		if( Format & SPRITEFORMAT_SHADOW )
			if( !m.Read( &m_state.shadow, sizeof(m_state.shadow) ) )
				return false;
	}
	else
	if( Version == SPRITE_LOADSAVE_VERSION5 || Version == SPRITE_LOADSAVE_VERSION6 )
	{
		//=======================================================================
		// this version is basically the same as version 4 but the position.z
		// member has been removed.
		//=======================================================================

		u32 Format = 0;

		if( !m.Read( &Format, sizeof(Format) ) )
			return false;

		if( Format & SPRITEFORMAT_TEXTURESET )
			if( !m.Read( &m_state.textureset, sizeof(m_state.textureset) ) )
				return false;

		if( Format & SPRITEFORMAT_POSITION )
			if( !m.Read( &m_state.position, sizeof(m_state.position) ) )
				return false;

		if( Format & SPRITEFORMAT_SIZE )
			if( !m.Read( &m_state.size, sizeof(m_state.size) ) )
				return false;

		if( Format & SPRITEFORMAT_SCALE )
			if( !m.Read( &m_state.scale, sizeof(m_state.scale) ) )
				return false;

		if( Format & SPRITEFORMAT_ORIGIN )
			if( !m.Read( &m_state.origin, sizeof(m_state.origin) ) )
				return false;

		if( Format & SPRITEFORMAT_OFFSET )
			if( !m.Read( &m_state.offset, sizeof(m_state.offset) ) )
				return false;

		if( Format & SPRITEFORMAT_ROTATION )
			if( !m.Read( &m_state.rotation, sizeof(m_state.rotation) ) )
				return false;

		if( Format & SPRITEFORMAT_BASECOLOR )
			if( !m.Read( &m_state.color, sizeof(m_state.color) ) )
				return false;

		if( Format & SPRITEFORMAT_FAMILYCOLOR )
			if( !m.Read( &m_state.familycolor, sizeof(m_state.familycolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXCOLOR )
			if( !m.Read( &m_state.vertexcolor, sizeof(m_state.vertexcolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXPOSITION )
			if( !m.Read( &m_state.vertexposition, sizeof(m_state.vertexposition) ) )
				return false;

		if( Format & SPRITEFORMAT_TEXCOORDS )
			if( !m.Read( &m_state.texcoords, sizeof(m_state.texcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_CLIPCOORDS )
			if( !m.Read( &m_state.clipcoords, sizeof(m_state.clipcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_MISC )
		{
			if( !m.Read( &m_state.sprite, sizeof(m_state.sprite) ) )
				return false;

			m_state.sprite.blendmode = SPRITEBLEND_NORMAL;
			m_state.sprite.reserved0 = 0;
			m_state.sprite.reserved1 = 0;
		}

		if( Format & SPRITEFORMAT_SHADOW )
			if( !m.Read( &m_state.shadow, sizeof(m_state.shadow) ) )
				return false;
	}
	if( Version == SPRITE_LOADSAVE_VERSION7 )
	{
		//=======================================================================
		// this version is basically the same as version 4 but the position.z
		// member has been removed.
		//=======================================================================

		u32 Format = 0;

		if( !m.Read( &Format, sizeof(Format) ) )
			return false;

		if( Format & SPRITEFORMAT_TEXTURESET )
			if( !m.Read( &m_state.textureset, sizeof(m_state.textureset) ) )
				return false;

		if( Format & SPRITEFORMAT_POSITION )
			if( !m.Read( &m_state.position, sizeof(m_state.position) ) )
				return false;

		if( Format & SPRITEFORMAT_SIZE )
			if( !m.Read( &m_state.size, sizeof(m_state.size) ) )
				return false;

		if( Format & SPRITEFORMAT_SCALE )
			if( !m.Read( &m_state.scale, sizeof(m_state.scale) ) )
				return false;

		if( Format & SPRITEFORMAT_ORIGIN )
			if( !m.Read( &m_state.origin, sizeof(m_state.origin) ) )
				return false;

		if( Format & SPRITEFORMAT_OFFSET )
			if( !m.Read( &m_state.offset, sizeof(m_state.offset) ) )
				return false;

		if( Format & SPRITEFORMAT_ROTATION )
			if( !m.Read( &m_state.rotation, sizeof(m_state.rotation) ) )
				return false;

		if( Format & SPRITEFORMAT_BASECOLOR )
			if( !m.Read( &m_state.color, sizeof(m_state.color) ) )
				return false;

		if( Format & SPRITEFORMAT_FAMILYCOLOR )
			if( !m.Read( &m_state.familycolor, sizeof(m_state.familycolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXCOLOR )
			if( !m.Read( &m_state.vertexcolor, sizeof(m_state.vertexcolor) ) )
				return false;

		if( Format & SPRITEFORMAT_VERTEXPOSITION )
			if( !m.Read( &m_state.vertexposition, sizeof(m_state.vertexposition) ) )
				return false;

		if( Format & SPRITEFORMAT_TEXCOORDS )
			if( !m.Read( &m_state.texcoords, sizeof(m_state.texcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_CLIPCOORDS )
			if( !m.Read( &m_state.clipcoords, sizeof(m_state.clipcoords) ) )
				return false;

		if( Format & SPRITEFORMAT_MISC )
			if( !m.Read( &m_state.sprite, sizeof(m_state.sprite) ) )
				return false;

		if( Format & SPRITEFORMAT_SHADOW )
			if( !m.Read( &m_state.shadow, sizeof(m_state.shadow) ) )
				return false;
	}


	if( texture != 0 )
		TextureName.set( texture );

	t_Handle TextureSet = INVALID_HANDLE;

	if( TextureName.length() > 0 )
	{
		// rhn HACK, some screens don't have a path
		char filename[ 256 ];
		sprintf( filename, "%s%s", ( strchr( TextureName.get(), '/' ) || strchr( TextureName.get(), '\\' ) ) ?
			"" : "Screens\\", TextureName.get() );

		TextureSet = dblLoad(filename);

		/*
		ASSERT( TextureSet != INVALID_HANDLE );

		if( TextureSet == INVALID_HANDLE ) // do it again so we can step through and see why it failed to load.
			TextureSet = dblLoad(TextureName.get());
		*/
	}

	SetTextureSet( TextureSet, m_state.textureset.texture );

	return true;
}

//=============================================================================
// Sprite::ComputeTexture()
//=============================================================================

void Sprite::ComputeTexture()
{
	//==========================================================================
	// Register the sprite if it hasn't been done already, or re-register the
	// sprite if there is a texture change (allows animating textures)
	//==========================================================================

	//==========================================================================
	// If this hits, either this is the first time the sprite has been displayed
	// or the texture set or texture has been changed.
	//==========================================================================

	if( m_state.textureset.handle != INVALID_HANDLE && m_status.texture.texturedatabase == 0 )
	{
		//==========================================================================
		// This can happen when the m_state.textureset.handle is injected into the
		// data structure, for example during a memcpy of one sprite to another.
		// This will force the m_status.texture.texturedatabase back into action!
		//==========================================================================

		SetTextureSet( m_state.textureset.handle,
			            m_state.textureset.texture,
							m_state.textureset.cell,
							m_state.textureset.cols,
							m_state.textureset.rows );
	}

	if( m_status.handles.sprite == INVALID_HANDLE || m_status.texture.handle != m_state.textureset.handle || m_status.texture.texture != m_state.textureset.texture )
	{
		static Vector3 vertices[4];
		static Vector2 texuvs[4];
		static ts_bRGBA colors[4];

		if( m_state.textureset.handle == INVALID_HANDLE )
			ReleaseSprites();
		else
		{
			if( m_state.textureset.handle != m_status.texture.handle )
				SetTextureSet( m_state.textureset.handle, m_state.textureset.texture, m_state.textureset.cell, m_state.textureset.cols, m_state.textureset.rows );

			//====================================================================
			// Since we've gotten this far, we know either the texture handle or
			// the texture index changed, or the sprite has just been made visible
			// or (unlikely) a previous attempt to create the sprite failed.
			//====================================================================

			SpriteMgr__SetTextureDbl( m_state.textureset.handle );

			if( m_status.texture.texturedatabase )
			{
				//====================================================================
				// How many sprites do we need?
				//====================================================================

				if( !m_status.texture.texturedatabase->PieceCount() || m_state.textureset.texture == TS_NO_TEXTURE )
				{
					//=================================================================
					// Pretty easy for a non-merged texture set.
					//=================================================================

					if( m_status.handles.sprite != INVALID_HANDLE )
						SpriteMgr__KillSprite( m_status.handles.sprite );

					m_status.handles.sprite = SpriteMgr__RegisterSprite( m_state.textureset.texture,
																						  0,
																						  0,
																						  0,
																						  vertices,
																						  texuvs,
																						  colors );

					if( m_status.handles.sprite != INVALID_HANDLE )
						SpriteMgr__ChangeFlag( m_status.handles.sprite, 0 );
				}
				else
				{
					//=================================================================
					// A merged texture set is a bit more complicated.
					//=================================================================

					if( m_status.handles.sprite != INVALID_HANDLE )
						SpriteMgr__KillSprite( m_status.handles.sprite );

					//==========================================================================
					// Each piece of the whole texture has a different texture id.
					//==========================================================================

					m_status.handles.sprite = SpriteMgr__RegisterSprite( m_status.texture.texturedatabase->GetPiece((u16)m_state.textureset.texture)->u16TexIndex,
																						  0,
																						  0,
																						  0,
																						  vertices,
																						  texuvs,
																						  colors );

					if( m_status.handles.sprite != INVALID_HANDLE )
						SpriteMgr__ChangeFlag( m_status.handles.sprite, 0 );
				}

				if( m_status.handles.sprite != INVALID_HANDLE )
					InformParentsOurHandlesArentReleased();

				m_status.texture.texturedatabase = m_status.texture.texturedatabase;
				m_status.texture.handle = m_state.textureset.handle;
				m_status.texture.texture = m_state.textureset.texture;
			}
		}

		//==========================================================================
		// Existing shadow sprites are deleted here because the texture set has
		// changed.  The shadow sprites are recreated later on.
		//==========================================================================

		Sprite::ReleaseShadows(); // Oops, don't call subclassed "ReleaseShadows()" (i.e. Text::ReleaseShadows())
	}

	return;
}

//=============================================================================
// Sprite::ComputeSprite() - Because a sprite is state driven, this function
//									  detects changes and updates the sprite as
//									  necessary.  Unfortnately, some of the work during
//									  an update is wasted because not all the paramters
//									  change from to frame.  However, it has been
//									  optimized to help reduce wasted work.  For example,
//									  when there is no rotation on the z-axis, a matrix
//									  multiplication (CatZRotation) is eliminated.
//=============================================================================

void Sprite::ComputeSprite( void )
{
	//==========================================================================
	// Early check for visibility.
	//==========================================================================

	if( !m_state.sprite.visible || (FLOATTOINT(m_state.color.a) == 0 && !Child() ) )
		ReleaseHandles();
	else
	{
		Sprite *parent = this;

		struct {
			float a;
			float r;
			float g;
			float b;
		} color;

		struct {
			float a;
			float r;
			float g;
			float b;
		} familycolor;

		struct {
			float a;
			float r;
			float g;
			float b;
		} vertexcolor;

		bool visible = m_state.sprite.visible;

		//=======================================================================
		// Determine the visibility of a sprite by checking its alpha component.
		// If the alpha is zero, or the parent sprite has a group alpha of zero,
		// then the sprite will be disabled and the handle will be returned to
		// the sprite manager.
		//=======================================================================

		if( visible )
		{
			color.a = (1.0f/255.0f) * m_state.color.a;
			color.r = (1.0f/255.0f) * m_state.color.r;
			color.g = (1.0f/255.0f) * m_state.color.g;
			color.b = (1.0f/255.0f) * m_state.color.b;

			if( FLOATTOINT(color.a) == 0 )
				visible = false;

			while( parent && visible )
			{
				visible = parent->m_state.sprite.visible;

				familycolor.a = (1.0f/255.0f) * parent->m_state.familycolor.a;
				familycolor.r = (1.0f/255.0f) * parent->m_state.familycolor.r;
				familycolor.g = (1.0f/255.0f) * parent->m_state.familycolor.g;
				familycolor.b = (1.0f/255.0f) * parent->m_state.familycolor.b;

				if( FLOATTOINT(familycolor.a) == 0 )
					visible = false;
				else
				{
					color.a = color.a * familycolor.a;
					color.r = color.r * familycolor.r;
					color.g = color.g * familycolor.g;
					color.b = color.b * familycolor.b;

					parent = parent->Parent();
				}
			}
		}

		if( !visible )
			ReleaseHandles();
		else
		{
			//==========================================================================
			// This code path is only executed if the sprite is visible.  Sprites that
			// have been disabled or have had their alpha component set to zero will
			// never get this far.
			//==========================================================================

			const float viewportx = m_viewportx * m_invscreenx;
			const float viewporty = m_viewporty * m_invscreeny;

			//==========================================================================
			// Register the sprite if it hasn't been done already, or re-register the
			// sprite if there is a texture change (allows animating textures)
			//==========================================================================

			ComputeTexture();

			//==========================================================================
			// Calculate each of the sprites vertices, the texture coordinates, and
			// vertex colors. We must take into consideration the clipping region.
			//==========================================================================

			if( m_status.handles.sprite != INVALID_HANDLE )
			{
				//=======================================================================
				// Compute the sort priority relative to our parent.
				//=======================================================================

				u32 flags = ComputeFlags();
				u16 priority = (u16) GetSortedPriority();
				u8 blendmode = m_state.sprite.blendmode;

				SpriteMgr__ChangeSortPriority( m_status.handles.sprite, priority );

				//=======================================================================
				// Transform the vertices.
				//=======================================================================

				Vector3 v[4];

				// FIXME: DON'T COMPUTE THE TRANSFORM EVERY TIME!  USE A MATRIX STACK :)
				ComputeVertices( GetFamilyTransform(), v );

				//=======================================================================
				// Store the REAL computed vertices here.
				//=======================================================================

				Vector3 vertices[4];
				Vector2 texuvs[4];
				ts_bRGBA colors[4];
				float r, g, b, a;
				float x, y;

				//=======================================================================
				// Now we check if the sprite is clipped.  There are two different code
				// paths we can take, one that assumes no clipping is performed and
				// another that assumes clipping might be performed and executes more
				// slowly.
				//
				// We could just stick to one code path since the clipping algorithm
				// works whether or not the sprite really is clipped, but it would cause
				// a significant performance hit because so much work is required.  By
				// having two different code paths, we can optimize for the best case
				// scenereo and realize a moddest performance gain.  This is due to the
				// fact that few sprites (if any) will ever use the clipping feature.
				//
				// To determine whether a sprite is clipped or not, all we need to do
				// is make sure the extreme corners of the clipping region match that of
				// the sprite itself.
				//
				// However, for performance reasons, I've reduced the test from eight
				// floating point unit (FPU) compares to a single integer (ALU) compare.
				//
				// In the following code, I take the four values of the clipping region
				// that should be equal to zero, OR them with each other, and compare
				// the result to zero.
				//
				// On the other end, I take the four values of the clipping region that
				// must be equal to one, AND them together, then XOR the result with the
				// floating point value of one, which in integer format, is 0x3f800000.
				//
				// Finally, the results are OR'd and if all bits are zero, the faster
				// code path is executed.
				//=======================================================================


				// wtf?  don't do this kind of bullshit unless you absolutely need the extra bit
				// of speed.  This doesn't work, and I'm not gonna fucking take the time to figure
				// out WHY it doesn't work, although if I had to guess, the fact that the code depends
				// on a particular binary representation of a float is probably the cause.  Why the fuck
				// would you do something like that in code meant to work across 4 platforms?  Sorry if I'm
				// hurting anybody's feelings here, but shit like this pisses me off.  This code is unreadable
				// not to mention BROKEN all to save a measly few hundred cycles every FRAME.  wtf?  -TN

				bool cropped = false;

				if( m_state.clipcoords[0].x > 0.0f || m_state.clipcoords[3].x > 0.0f ||
						m_state.clipcoords[0].y > 0.0f || m_state.clipcoords[1].y > 0.0f ||
						m_state.clipcoords[1].x < 1.0f || m_state.clipcoords[2].x < 1.0f ||
						m_state.clipcoords[2].y < 1.0f || m_state.clipcoords[3].y < 1.0f )
					cropped = true;

				#ifdef STUPID_ASS_CODE
					int zeros = FLOATTOINT(m_state.clipcoords[0].x)|
									FLOATTOINT(m_state.clipcoords[3].x)|
									FLOATTOINT(m_state.clipcoords[0].y)|
									FLOATTOINT(m_state.clipcoords[1].y);

					int ones  = FLOATTOINT(m_state.clipcoords[1].x)&
									FLOATTOINT(m_state.clipcoords[2].x)&
									FLOATTOINT(m_state.clipcoords[2].y)&
									FLOATTOINT(m_state.clipcoords[3].y);
				#endif //STUPID_ASS_CODE

				#ifdef PS2
					color.r *= 128.0f / (256.0f*256.0f*128.0f);
					color.g *= 128.0f / (256.0f*256.0f*128.0f);
					color.b *= 128.0f / (256.0f*256.0f*128.0f);
					color.a *= 128.0f / (256.0f*256.0f*128.0f);
				#else
					color.r *= 255.0f / (256.0f*256.0f*128.0f);
					color.g *= 255.0f / (256.0f*256.0f*128.0f);
					color.b *= 255.0f / (256.0f*256.0f*128.0f);
					color.a *= 255.0f / (256.0f*256.0f*128.0f);
				#endif

				//=======================================================================
				// This test tells us quickly whether the sprite is cropped or not.
				//=======================================================================

				// No it doesn't.  It doesn't do shit except tell you that cropped sprites
				// aren't cropped.  Useful, that.
				#ifdef STUPID_ASS_CODE
					if( zeros|(ones&~0x3f800000) )
				#endif // STUPID_ASS_CODE

				if( cropped )
				{
					float A, B, C, D;

					//=======================================================================
					// Test failed, which means the sprite is partially clipped.
					//=======================================================================

					for( int i = 0; i < 4; i++ )
					{
						x = m_state.clipcoords[i].x;
						y = m_state.clipcoords[i].y;

						A = (1.0f - x) * (1.0f - y);
						B = (0.0f + x) * (1.0f - y);
						C = (0.0f + x) * (0.0f + y);
						D = (1.0f - x) * (0.0f + y);

						vertices[i].Set(v[i].x() * m_invscreenx - viewportx,
											 v[i].y() * m_invscreeny - viewporty,
											 v[i].z());

						//====================================================================
						// This is where we clip the sprite.  I hope the FPU is fast =)
						//====================================================================

						texuvs[i].x( m_state.texcoords[0].x * A + m_state.texcoords[1].x * B + m_state.texcoords[2].x * C + m_state.texcoords[3].x * D);
						texuvs[i].y( m_state.texcoords[0].y * A + m_state.texcoords[1].y * B + m_state.texcoords[2].y * C + m_state.texcoords[3].y * D);

						vertexcolor.a = m_state.vertexcolor[0].a * A + m_state.vertexcolor[1].a * B + m_state.vertexcolor[2].a * C + m_state.vertexcolor[3].a * D;
						vertexcolor.r = m_state.vertexcolor[0].r * A + m_state.vertexcolor[1].r * B + m_state.vertexcolor[2].r * C + m_state.vertexcolor[3].r * D;
						vertexcolor.g = m_state.vertexcolor[0].g * A + m_state.vertexcolor[1].g * B + m_state.vertexcolor[2].g * C + m_state.vertexcolor[3].g * D;
						vertexcolor.b = m_state.vertexcolor[0].b * A + m_state.vertexcolor[1].b * B + m_state.vertexcolor[2].b * C + m_state.vertexcolor[3].b * D;

						a = 1.0f + color.a * (1.0f/255.0f) * vertexcolor.a;
						r = 1.0f + color.r * (1.0f/255.0f) * vertexcolor.r;
						g = 1.0f + color.g * (1.0f/255.0f) * vertexcolor.g;
						b = 1.0f + color.b * (1.0f/255.0f) * vertexcolor.b;

						colors[i].R = (u8) FLOATTOINT(r);
						colors[i].G = (u8) FLOATTOINT(g);
						colors[i].B = (u8) FLOATTOINT(b);
						colors[i].A = (u8) FLOATTOINT(a);
					}
				}
				else
				{
					//=======================================================================
					// The whole sprite is visible, much easier and faster this way!
					//=======================================================================

					for( int i = 0; i < 4; i++ )
					{
						vertices[i].Set(v[i].x() * m_invscreenx - viewportx,
											 v[i].y() * m_invscreeny - viewporty,
											 v[i].z());

						a = 1.0f + color.a * (1.0f/255.0f) * m_state.vertexcolor[i].a;
						r = 1.0f + color.r * (1.0f/255.0f) * m_state.vertexcolor[i].r;
						g = 1.0f + color.g * (1.0f/255.0f) * m_state.vertexcolor[i].g;
						b = 1.0f + color.b * (1.0f/255.0f) * m_state.vertexcolor[i].b;

						texuvs[i].x( m_state.texcoords[i].x);
						texuvs[i].y( m_state.texcoords[i].y);

						colors[i].R = (u8) FLOATTOINT(r);
						colors[i].G = (u8) FLOATTOINT(g);
						colors[i].B = (u8) FLOATTOINT(b);
						colors[i].A = (u8) FLOATTOINT(a);
					}
				}

				//=======================================================================
				// When a texture contains multiple image cells, the texture coordinates
				// specified by the user refer to the extents of an individual cell, not
				// the whole texture.  The real texture coordinates must be scaled so
				// that the sprite is displayed correctly.
				//=======================================================================
				// NOTE: This code path is rarely taken.
				//=======================================================================

				if( m_state.textureset.cols > 1 || m_state.textureset.rows > 1 )
				{
					float cellw = 1.0f  / (float) (m_state.textureset.cols);
					float cellh = 1.0f  / (float) (m_state.textureset.rows);
					float baseu = cellw * (float) (m_state.textureset.cell % m_state.textureset.cols);
					float basev = cellh * (float) (m_state.textureset.cell / m_state.textureset.cols % m_state.textureset.rows);

					for( int i = 0; i < 4; i++ )
					{
						texuvs[i].x( baseu + cellw * texuvs[i].x());
						texuvs[i].y( basev + cellh * texuvs[i].y());
					}
				}

				//=======================================================================
				// When a merged texture set is used, the REAL sprite is actually
				// composed of several smaller sprites, each using a different texture.
				// In this case, the vertices that have been computed up to this point
				// represent only a virtual sprite, and not the real thing.  The code
				// below is responsible for sub-dividing the sprite into peices small
				// enough for the engine to handle.
				//=======================================================================
				// NOTE: This limitation only exists because of the PS2.
				//=======================================================================

				if( m_status.handles.sprite != INVALID_HANDLE )
				{
					//=================================================================
					// NOTICE: AT THIS TIME, THE SUB-DIVISION ALGORITHM IS NOT WORKING,
					//			  SO MERGED TEXTURE SETS ARE NOT SUPPORTED, SORRY.
					//=================================================================

					if( m_status.texture.texture == TS_NO_TEXTURE )
					{
						SpriteMgr__UpdateSprite( m_status.handles.sprite, 0, priority, flags, vertices, 0, colors );
					}
					else
					{
						Texture* texture = m_status.texture.texturedatabase->GetTexture(m_status.texture.texture);

						//==============================================================
						// Check if the texture has been stored horizontally rather than
						// vertically.  This can happen when a texture is taller than it
						// is wide.
						//==============================================================

						if(!(texture->GetImage()->GetFlags() & Image::UVREVERSED))
						{
							SpriteMgr__UpdateSprite( m_status.handles.sprite, 0, priority, flags, vertices, texuvs, colors );
						}
						else
						{
							Vector2 texuvs_r[4];

							texuvs_r[0].x( texuvs[0].y());
							texuvs_r[0].y( texuvs[0].x());
							texuvs_r[1].x( texuvs[1].y());
							texuvs_r[1].y( texuvs[1].x());
							texuvs_r[2].x( texuvs[2].y());
							texuvs_r[2].y( texuvs[2].x());
							texuvs_r[3].x( texuvs[3].y());
							texuvs_r[3].y( texuvs[3].x());

							SpriteMgr__UpdateSprite( m_status.handles.sprite, 0, priority, flags, vertices, texuvs_r, colors );
						}
					}

					SpriteMgr__SetBlendMode( m_status.handles.sprite, m_state.sprite.blendmode );
					SpriteMgr__EnableSprite( m_status.handles.sprite );
				}

				//=======================================================================
				// Enable or disable the drop shadow
				//=======================================================================

				ComputeShadow();
			}
		}
	}

	return;
}

//=============================================================================
// Sprite::ComputeShadow() - This function generates, assigns and computes
//									  sprites necessary to display a drop shadow under
//									  the sprite.  The drop shadow is a sprite itself,
//									  cloned from the original sprite and modified so
//									  that each vertex is black with variable opacity.
//=============================================================================

void Sprite::ComputeShadow( void )
{
	if( !m_state.shadow.enabled )
		ReleaseShadows();
	else
	{
		if( m_status.handles.shadow == INVALID_HANDLE )
			CreateShadows();

		struct {
			Vector3 *vertices;
			Vector2 *texuvs;
			ts_bRGBA *colors;
		} sprite, shadow;

		float oneoverx = 1.0f / SimulatedScreenWidth();
		float oneovery = 1.0f / SimulatedScreenHeight();

		SpriteMgr__SetTextureDbl( m_state.textureset.handle );

		if(m_status.handles.sprite != INVALID_HANDLE &&
			m_status.handles.shadow != INVALID_HANDLE )
		{
			SpriteMgr__GetDataPointers( m_status.handles.sprite, 0, &sprite.vertices, &sprite.texuvs, &sprite.colors );
			SpriteMgr__GetDataPointers( m_status.handles.shadow, 0, &shadow.vertices, &shadow.texuvs, &shadow.colors );

			memcpy( shadow.texuvs, sprite.texuvs, sizeof(Vector2) * 4 );

			//=======================================================================
			// The color for all shadow sprites should already be black.
			//=======================================================================

			for( int c = 0; c < 4; c++ )
			{
				shadow.vertices[c].Set(sprite.vertices[c].x() + m_state.shadow.x * oneoverx,
								           sprite.vertices[c].y() + m_state.shadow.y * oneovery,
											  sprite.vertices[c].z());

				shadow.colors[c].A = (u8) ((int) sprite.colors[c].A * (int) m_state.shadow.intensity / 255);	// intensity is from 0..255
			}

			SpriteMgr__ChangeSortPriority( m_status.handles.shadow, SpriteMgr__GetSortPriority( m_status.handles.sprite ) - 1);
			SpriteMgr__EnableSprite( m_status.handles.shadow );
		}
	}

	return;
}

//=============================================================================
// Sprite::ComputeFlags() - Computes the appropriate flags value to pass to the
//									 sprite manager for rendering and blend modes.
//=============================================================================

u32 Sprite::ComputeFlags( void )
{
	u32 flags = 0;
	u32 blend = 0;

	if( m_state.sprite.blendmode != SPRITEBLEND_NORMAL )
		blend = SPRITE_BLENDMODE_ENABLED;

	return flags | blend;
}


//=============================================================================
// Sprite::ReleaseSprites() - Frees up the sprite handles, usually in response
//										to disabling the sprite (which can happen when
//										sprite's alpha channel is completely transparent)
//										or when the sprite is being deleted.
//=============================================================================

void Sprite::ReleaseSprites( void )
{
	SetDrawPending( false );

	if( m_status.handles.sprite != INVALID_HANDLE )
	{
		SpriteMgr__KillSprite( m_status.handles.sprite );
		m_status.handles.sprite = INVALID_HANDLE;
	}

	return;
}

//=============================================================================
// Sprite::CreateShadows() - The drop shadow uses a sprite(s) to create the
//									  dark, transparent shadow that appears below a
//									  sprite.  The shadow sprite is cloned from the
//									  original sprite and its vertex colors and
//									  position are modified to give the illusion of a
//									  shadow.  Please keep in mind that drop shadows
//									  increase processing time for each sprite, and
//									  should be used sparingly.  When texture memory is
//									  not an issue and dynamic drop shadows are not
//									  required, it would be smarter to embed the shadow
//									  directly in the texture.
//=============================================================================

bool Sprite::CreateShadows( void )
{
	ReleaseShadows();

	//=============================================================================
	// Can't create a drop shadow when there is no sprite.
	//=============================================================================

	if( m_status.handles.sprite == INVALID_HANDLE )
		return false;

	Vector3 *vertices;
	Vector2 *texuvs;
	ts_bRGBA *colors;

	//=============================================================================
	// The drop shadow uses the same texture as the sprite, the only difference is
	// that the vertex colors on the drop shadow are set to black and its alpha
	// channel is reduced to 25% (this is an arbitrary value.)
	//=============================================================================

	SpriteMgr__SetTextureDbl( m_state.textureset.handle );
	SpriteMgr__GetDataPointers( m_status.handles.sprite, 0, &vertices, &texuvs, &colors );

	u32 indexButReallyTheTexture = (u32)SpriteMgr__GetTexture( m_status.handles.sprite );

	m_status.handles.shadow = SpriteMgr__RegisterSprite( indexButReallyTheTexture,
																		  0,
																		  0,
																		  DIRECT_BIND,
																		  vertices,
																		  texuvs,
																		  colors );

	//=============================================================================
	// Under heavy sprite usage, its quite possible that allocation of the shadow
	// sprite may fail.  We must be able to tolerate this condition.
	//=============================================================================

	if( m_status.handles.shadow != INVALID_HANDLE )
	{
		SpriteMgr__GetDataPointers( m_status.handles.shadow, 0, 0, 0, &colors );

		for( int c = 0; c < 4; c++ )
		{
			colors[c].R = 0;
			colors[c].G = 0;
			colors[c].B = 0;
			colors[c].A = 0;
		}
	}

	// TJC - let our parents (recursively) know that we have handles that
	// must be ReleaseHandles()-ed.
	InformParentsOurHandlesArentReleased();

	return true;
}

//=============================================================================
// Sprite::ReleaseShadows() - Frees up the sprites used by the drop shadow,
//										usually in response to disabling the drop shadow
//										or deleting the sprite.
//=============================================================================

void Sprite::ReleaseShadows( void )
{
	if( m_status.handles.shadow != INVALID_HANDLE )
	{
		SpriteMgr__KillSprite( m_status.handles.shadow );
		m_status.handles.shadow = INVALID_HANDLE;
	}

	return;
}

//=============================================================================
// Sprite::OnCreate() : This virtual function is called in response to creating
//							 : a sprite, and is only used by derived classes that need
//							 : customized functionality.
//=============================================================================


void Sprite::OnCreate( void )
{
	Reset();
}

//=============================================================================
// Sprite::OnRelease()  : This virtual function is called in response to
//								: destroying a sprite, and is only used by derived
//								: classes that need customized functionality.
//=============================================================================

void Sprite::OnRelease( void )
{
}

//=============================================================================
// Sprite::OnShow() : This virtual function is called during the update process
//						  : when a sprite becomes visible, and is only used by 
//						  : derived classes that need customized functionality.
//=============================================================================

void Sprite::OnShow( void )
{
}

//=============================================================================
// Sprite::OnHide() : This virtual function is called during the update process
//						  : when a sprite becomes invisible, and is only used by
//						  : derived classes that need customized functionality.
//=============================================================================

void Sprite::OnHide( void )
{
}

//=============================================================================
// Sprite::OnUpdate() : This virtual function is called PRIOR to updating the
//							 : the sprite, and is only used by derived classes that
//							 : need customized funcionality.
//=============================================================================

void Sprite::OnUpdate( void )
{
}

//=============================================================================
// Sprite::OnDraw() : This virtual function is called during the update
//						  : process when a sprite is being drawn, and is only
//						  : used by derived classes that need customized
//						  : functionality.
//=============================================================================

void Sprite::OnDraw( void )
{
}
