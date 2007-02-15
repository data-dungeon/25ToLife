#ifndef __GCN_SHADER__
#define __GCN_SHADER__

#include "Game/Database/Som.h"
#include <dolphin/gx.h>

#include "container/dllist.h"

class ShaderMgr;

#define GCN_SHADER_DL_SIZE		( 1 * 1024)

struct ts_TEVCoordGen
{
	GXTexCoordID	texCoordID;
	GXTexGenType	texGenType;
	GXTexGenSrc		texGenSrc;
	GXBool			texNormalize;
	GXPTTexMtx		texPostMtx;

	int operator==( const ts_TEVCoordGen &comp) const
	{
		return(	texCoordID == comp.texCoordID &&
					texGenType == comp.texGenType &&
					texGenSrc == comp.texGenSrc &&
					texNormalize == comp.texNormalize &&
					texPostMtx == comp.texPostMtx);
	};

	int operator!=( const ts_TEVCoordGen &comp) const
	{
		return( !( *this == comp));
	};
};

struct ts_TEVOrder
{
	GXTexCoordID	texCoordID;
	GXTexMapID		texMapID;
	GXChannelID		channelID;

	int operator==( const ts_TEVOrder &comp) const
	{
		return(	texCoordID == comp.texCoordID &&
					texMapID == comp.texMapID &&
					channelID == comp.channelID);
	};

	int operator!=( const ts_TEVOrder &comp) const
	{
		return( !( *this == comp));
	};
} ;

struct ts_TEVColorIn
{
	GXTevColorArg	colorArgA,
						colorArgB,
						colorArgC,
						colorArgD;

	int operator==( const ts_TEVColorIn &comp) const
	{
		return(	colorArgA == comp.colorArgA &&
					colorArgB == comp.colorArgB &&
					colorArgC == comp.colorArgC &&
					colorArgD == comp.colorArgD);
	};

	int operator!=( const ts_TEVColorIn &comp) const
	{
		return( !( *this == comp));
	};
};

struct ts_TEVAlphaIn
{
	GXTevAlphaArg	alphaArgA,
						alphaArgB,
						alphaArgC,
						alphaArgD;

	int operator==( const ts_TEVAlphaIn &comp) const
	{
		return(	alphaArgA == comp.alphaArgA &&
					alphaArgB == comp.alphaArgB &&
					alphaArgC == comp.alphaArgC &&
					alphaArgD == comp.alphaArgD);
	};

	int operator!=( const ts_TEVAlphaIn &comp) const
	{
		return( !( *this == comp));
	};
};

struct ts_TEVColorAlphaOp
{
	GXTevOp			tevOP;
	GXTevBias		tevBias;
	GXTevScale		tevScale;
	GXBool			tevClamp;
	GXTevRegID     tevOutReg;

	int operator==( const ts_TEVColorAlphaOp &comp) const
	{
		return(	tevOP == comp.tevOP &&
					tevBias == comp.tevBias &&
					tevScale == comp.tevScale &&
					tevClamp == comp.tevClamp &&
					tevOutReg == comp.tevOutReg);
	};

	int operator!=( const ts_TEVColorAlphaOp &comp) const
	{
		return( !( *this == comp));
	};
};

struct ts_TEVKonstSelect
{
	GXTevKColorSel		kColorSel;
	GXTevKAlphaSel		kAlphaSel;
	int operator==( const ts_TEVKonstSelect &comp) const
	{
		return(	kColorSel == comp.kColorSel &&
					kAlphaSel == comp.kAlphaSel);
	};

	int operator!=( const ts_TEVKonstSelect &comp) const
	{
		return( !( *this == comp));
	};
};

struct ts_TEVStageParams
{
	ts_TEVOrder					tevOrder;
	ts_TEVCoordGen				tevCoordGen;
	ts_TEVColorIn				tevColorIn;
	ts_TEVColorAlphaOp		tevColorOp;
	ts_TEVAlphaIn				tevAlphaIn;
	ts_TEVColorAlphaOp		tevAlphaOp;
	ts_TEVKonstSelect			tevKonstSelect;

	int operator==( const ts_TEVStageParams &comp) const
	{
		return(	tevOrder == comp.tevOrder &&
					tevCoordGen == comp.tevCoordGen &&
					tevColorIn == comp.tevColorIn &&
					tevColorOp == comp.tevColorOp &&
					tevAlphaIn == comp.tevAlphaIn &&
					tevAlphaOp == comp.tevAlphaOp &&
					tevKonstSelect == comp.tevKonstSelect);
	};

	int operator!=( const ts_TEVStageParams &comp) const
	{
		return( !( *this == comp));
	};
};

class ShaderInst
{
	#define m_pDL ( (void *)( m_DLBuffer))
	private:
		u8							m_DLBuffer[GCN_SHADER_DL_SIZE];

	public:
		ShaderInst				*next,
									*prev;

		inline int operator==( const ShaderInst &compShaderInst) const;		//does this shader match the supplied texture context

	protected:
		ShaderInst( );
		ShaderInst( ts_TextureContext *pTextureContext, bool bTEVGenEnvUVs);

		void Clear( void);

		inline void Apply( void);		//sends the shader to the hardware

		void CreateDisplayList( void);

	private:
		u8							m_nTevStages;
		u8							m_nTexGens;
		u8							m_nChans;
		
		ts_TEVStageParams		m_aTevStageParams[GX_MAX_TEVSTAGE];
		bool						m_bPermanent;

		u32						m_DLSize;

	friend class ShaderMgr;
};

class ShaderMgr
{
	public:
		enum
		{
			SHADER_RESET_LIGHTING = 1,
			SHADER_RESET_SHADER = 2,
			SHADER_RESET_MATRIX = 4,
			SHADER_RESET_MATCOLOR = 8
		};
		ShaderMgr( void);
		ShaderInst * GetShader( ts_TextureContext *pTextureContext, bool bTEVGenEnvUVs);
		inline void UseShader( ShaderInst *pShaderInst);
		void Lock( void)
		{
			m_bLocked = true;
		}
		void Unlock( void)
		{
			m_bLocked = false;
		}

		ShaderInst * GetRTShadowShader( void)
		{
			return( m_pRTShadowShader);
		}

		ShaderInst * GetRTShadowCreateShader( void)
		{
			return( m_pRTShadowCreate);
		}

		void Reset( int mask = ( SHADER_RESET_LIGHTING | SHADER_RESET_SHADER | SHADER_RESET_MATRIX | SHADER_RESET_MATCOLOR));
		void Cleanup( void);

		void SetShadersPermanent( bool bPerm)
		{
			m_bPermanent = bPerm;
		}

		bool GetShadersPermanent( void)
		{
			return( m_bPermanent);
		}

		void ResetCurrentMatrixIndices( void)
		{
			GXCallDisplayList( m_MatrixResetDL, m_MatrixResetDLSize);
		}

		void SetupLightingChannel( u32 channelFlags, GXColor &materialColor);

		void LightingChannelLock( void)
		{
			m_bLightingLocked = true;
		}

		void LightingChannelUnlock( void)
		{
			m_bLightingLocked = false;
		}
		
		enum
		{
			GCN_CHANNEL_HAS_VTXCOLOR = 0x01,
			GCN_CHANNEL_HAS_LIGHTS
		};

		inline void SetCullMode( GXCullMode gxCullMode);

		void LockDLMutex( void)
		{
			OSLockMutex( &m_DLMutex);
		}

		void UnlockDLMutex( void)
		{
			OSUnlockMutex( &m_DLMutex);
		}
		
	private:
		
		DoublyLinkedList<ShaderInst>	m_ShaderInstList;

		ShaderInst							*m_pCurrShaderInst;

		ShaderInst							*m_pRTShadowShader,
												*m_pRTShadowCreate;

		float									m_LastTextureBrightness;

		bool									m_bLocked;

		//lighting channel data
		bool									m_bLightingLocked;
		u32									m_ChannelFlags;
		GXColor								m_MaterialColor;
		u32									m_gxLightIDs;

		bool									m_bPermanent;

		void									*m_MatrixResetDL;
		u32									m_MatrixResetDLSize;

		void									*m_CullFrontDL;
		u32									m_CullFrontDLSize;

		void									*m_CullBackDL;
		u32									m_CullBackDLSize;

		void									*m_CullNoneDL;
		u32									m_CullNoneDLSize;

		OSMutex								m_DLMutex;
};

extern ShaderMgr *g_ShaderMgrPtr;
inline ShaderMgr & SafePtr( ShaderMgr *ptr)
{
	ASSERT( ptr);
	return( *ptr);
}
#define g_ShaderMgr SafePtr( g_ShaderMgrPtr)

#include "Display/GCN/Shader.hpp"

#endif

