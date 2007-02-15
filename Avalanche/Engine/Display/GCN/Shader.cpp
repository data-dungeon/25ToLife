/***************************************************************************/
// Shader system
/***************************************************************************/

/******************* includes ****************************/

#include "Display/DisplayPCH.h"

#include <Dolphin/gd.h>
#include "Display/GCN/FrameData.h"

// The singleton
ShaderMgr				*g_ShaderMgrPtr = NULL;

static GXTexMtx		agxTexMtx[] = 
{
	GX_TEXMTX0,
	GX_TEXMTX1,
	GX_TEXMTX2,
	GX_TEXMTX3,
	GX_TEXMTX4,
	GX_TEXMTX5,
	GX_TEXMTX6,
	GX_TEXMTX7,
	GX_TEXMTX8,
	GX_TEXMTX9
};

/***************************************************************************/
// Create an empty shader
/***************************************************************************/
ShaderInst::ShaderInst( )
{
	Clear( );
}

/***************************************************************************/
// Create a shader from a ts_TextureContext
/***************************************************************************/
ShaderInst::ShaderInst( ts_TextureContext *pTextureContext, bool bTEVGenEnvUVs)
{

	GXTevColorArg			gxtcaFragment,
								gxtcaTexture;

	GXTevAlphaArg			gxtaaFragment;

	ts_TEVStageParams		*pTSP;

	ts_TextureLayer		*pLayer;

	int						nTextureContexts,
								iStage,
								iTexMap,
								iTexCoord,
								iUVSet;

	Clear( );

	iStage = 0;
	iUVSet = 0;
	iTexMap = 0;
	iTexCoord = 0;
	pTSP = m_aTevStageParams;
	pLayer = pTextureContext->TextureLayer;

	nTextureContexts = pTextureContext->TextureCount;

	gxtcaFragment = GX_CC_RASC;
	gxtaaFragment = GX_CA_RASA;

	//if no textures in this context, just use the vertex color
	if( nTextureContexts == 0)
	{
		pTSP->tevOrder.texCoordID = GX_TEXCOORD_NULL;
		pTSP->tevOrder.texMapID = GX_TEXMAP_NULL;
		pTSP->tevOrder.channelID = GX_COLOR0A0;

		pTSP->tevCoordGen.texCoordID = GX_TEXCOORD0;
		pTSP->tevCoordGen.texGenType = GX_TG_MTX2x4;
		pTSP->tevCoordGen.texGenSrc = GX_TG_TEX0;

		pTSP->tevKonstSelect.kColorSel = GX_TEV_KCSEL_1;
		pTSP->tevKonstSelect.kAlphaSel = GX_TEV_KASEL_1;

		//we want C=Cf
		pTSP->tevColorIn.colorArgA = GX_CC_ZERO;
		pTSP->tevColorIn.colorArgB = GX_CC_ZERO;
		pTSP->tevColorIn.colorArgC = GX_CC_ZERO;
		pTSP->tevColorIn.colorArgD = gxtcaFragment;

		pTSP->tevColorOp.tevOP = GX_TEV_ADD;
		pTSP->tevColorOp.tevBias = GX_TB_ZERO;
		pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
		pTSP->tevColorOp.tevClamp = GX_TRUE;
		pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

		//we want A=Af
		pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
		pTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
		pTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
		pTSP->tevAlphaIn.alphaArgD = gxtaaFragment;

		pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
		pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
		pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
		pTSP->tevAlphaOp.tevClamp = GX_TRUE;
		pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;

		pTSP++;
		iStage++;
	}
	else
	{
		while( nTextureContexts)
		{
			if( pLayer->u8Flags & TEXTURE_IS_ENVIRONMENT_MAP && bTEVGenEnvUVs)
			{
				pTSP->tevCoordGen.texCoordID = (GXTexCoordID)( GX_TEXCOORD0 + iTexCoord);
				pTSP->tevCoordGen.texGenType = GX_TG_MTX3x4;
				pTSP->tevCoordGen.texGenSrc = GX_TG_NRM;
			}
			else
			{
				pTSP->tevCoordGen.texCoordID = (GXTexCoordID)( GX_TEXCOORD0 + iTexCoord);
				pTSP->tevCoordGen.texGenType = GX_TG_MTX2x4;
				pTSP->tevCoordGen.texGenSrc = (GXTexGenSrc)( GX_TG_TEX0 + iUVSet);
				iUVSet++;
			}

			gxtcaTexture = GX_CC_TEXC;
			if( TextureBind__SeparateAlpha( pLayer->s16TextureBind))
			{
				
				ts_TEVStageParams		*pColorTSP = pTSP;

				pTSP++;

				pTSP->tevCoordGen = pColorTSP->tevCoordGen;

				pColorTSP->tevOrder.texCoordID = (GXTexCoordID)( GX_TEXCOORD0 + iTexCoord);
				pColorTSP->tevOrder.texMapID = (GXTexMapID)( GX_TEXMAP0 + iTexMap);
				pColorTSP->tevOrder.channelID = GX_COLOR0A0;

				pColorTSP->tevKonstSelect.kColorSel = GX_TEV_KCSEL_1;
				pColorTSP->tevKonstSelect.kAlphaSel = GX_TEV_KASEL_1;

				//we want C=Ct
				pColorTSP->tevColorIn.colorArgA = GX_CC_ZERO;
				pColorTSP->tevColorIn.colorArgB = GX_CC_ZERO;
				pColorTSP->tevColorIn.colorArgC = GX_CC_ZERO;
				pColorTSP->tevColorIn.colorArgD = gxtcaTexture;

				pColorTSP->tevColorOp.tevOP = GX_TEV_ADD;
				pColorTSP->tevColorOp.tevBias = GX_TB_ZERO;
				pColorTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
				pColorTSP->tevColorOp.tevClamp = GX_TRUE;
				pColorTSP->tevColorOp.tevOutReg = GX_TEVREG0;

				//we want A=Af
				pColorTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
				pColorTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
				pColorTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
				pColorTSP->tevAlphaIn.alphaArgD = gxtaaFragment;

				pColorTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
				pColorTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
				pColorTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
				pColorTSP->tevAlphaOp.tevClamp = GX_TRUE;
				pColorTSP->tevAlphaOp.tevOutReg = GX_TEVREG0;

				iStage++;
				iTexMap++;

				gxtcaTexture = GX_CC_C0;
				gxtaaFragment = GX_CA_A0;
			}

			pTSP->tevOrder.texCoordID = (GXTexCoordID)( GX_TEXCOORD0 + iTexCoord);
			pTSP->tevOrder.texMapID = (GXTexMapID)( GX_TEXMAP0 + iTexMap);
			pTSP->tevOrder.channelID = GX_COLOR0A0;

			pTSP->tevKonstSelect.kColorSel = GX_TEV_KCSEL_1;
			pTSP->tevKonstSelect.kAlphaSel = GX_TEV_KASEL_1;

			switch( pLayer->u8Method)
			{
				case TS_METHOD_DECAL:
					//we want C=CtAt+Cf(1-At)
					pTSP->tevColorIn.colorArgA = gxtcaFragment;
					pTSP->tevColorIn.colorArgB = gxtcaTexture;
					pTSP->tevColorIn.colorArgC = GX_CC_TEXA;
					pTSP->tevColorIn.colorArgD = GX_CC_ZERO;

					pTSP->tevColorOp.tevOP = GX_TEV_ADD;
					pTSP->tevColorOp.tevBias = GX_TB_ZERO;
					pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevColorOp.tevClamp = GX_TRUE;
					pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

					//we want A=Af
					pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgD = gxtaaFragment;

					pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
					pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
					pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevAlphaOp.tevClamp = GX_TRUE;
					pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;
					break;

				case TS_METHOD_REPLACE:
					//we want C=Ct
					pTSP->tevColorIn.colorArgA = GX_CC_ZERO;
					pTSP->tevColorIn.colorArgB = GX_CC_ZERO;
					pTSP->tevColorIn.colorArgC = GX_CC_ZERO;
					pTSP->tevColorIn.colorArgD = gxtcaTexture;

					pTSP->tevColorOp.tevOP = GX_TEV_ADD;
					pTSP->tevColorOp.tevBias = GX_TB_ZERO;
					pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevColorOp.tevClamp = GX_TRUE;
					pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

					//we want A=At
					pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgB = GX_CA_TEXA;

					pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
					pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
					pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevAlphaOp.tevClamp = GX_TRUE;
					pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;
					break;

				case TS_METHOD_MODULATE:
					//we want C=CtCf
					pTSP->tevColorIn.colorArgA = GX_CC_ZERO;
					pTSP->tevColorIn.colorArgB = gxtcaTexture;
					pTSP->tevColorIn.colorArgC = gxtcaFragment;
					pTSP->tevColorIn.colorArgD = GX_CC_ZERO;

					pTSP->tevColorOp.tevOP = GX_TEV_ADD;
					pTSP->tevColorOp.tevBias = GX_TB_ZERO;
					pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevColorOp.tevClamp = GX_TRUE;
					pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

					//we want A=AtAf
					pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgB = GX_CA_TEXA;
					pTSP->tevAlphaIn.alphaArgC = gxtaaFragment;
					pTSP->tevAlphaIn.alphaArgD = GX_CA_ZERO;

					pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
					pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
					pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevAlphaOp.tevClamp = GX_TRUE;
					pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;
					break;

				case TS_METHOD_INTERPOLATE:
					//we want C=CtAc+Cf(1-Ac)
					pTSP->tevKonstSelect.kColorSel = GX_TEV_KCSEL_K1_A;

					pTSP->tevColorIn.colorArgA = gxtcaFragment;
					pTSP->tevColorIn.colorArgB = gxtcaTexture;
					pTSP->tevColorIn.colorArgC = GX_CC_KONST;
					pTSP->tevColorIn.colorArgD = GX_CC_ZERO;

					pTSP->tevColorOp.tevOP = GX_TEV_ADD;
					pTSP->tevColorOp.tevBias = GX_TB_ZERO;
					pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevColorOp.tevClamp = GX_TRUE;
					pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

					//we want A=Af
					pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgD = gxtaaFragment;

					pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
					pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
					pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevAlphaOp.tevClamp = GX_TRUE;
					pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;
					break;

				case TS_METHOD_CONSTANTALPHA:
					//we want C=Ct
					pTSP->tevColorIn.colorArgA = GX_CC_ZERO;
					pTSP->tevColorIn.colorArgB = GX_CC_ZERO;
					pTSP->tevColorIn.colorArgC = GX_CC_ZERO;
					pTSP->tevColorIn.colorArgD = gxtcaTexture;

					pTSP->tevColorOp.tevOP = GX_TEV_ADD;
					pTSP->tevColorOp.tevBias = GX_TB_ZERO;
					pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevColorOp.tevClamp = GX_TRUE;
					pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

					//we want A=Ac
					pTSP->tevKonstSelect.kAlphaSel = GX_TEV_KASEL_K1_A;
					pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
					pTSP->tevAlphaIn.alphaArgD = GX_CA_KONST;

					pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
					pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
					pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
					pTSP->tevAlphaOp.tevClamp = GX_TRUE;
					pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;
					break;

				default:
				case TS_METHOD_PASSTHRU:
					ASSERT( 0);
			}

			iStage++;
			iTexMap++;
			iTexCoord++;
			pTSP++;
			pLayer++;
			nTextureContexts--;

			gxtcaFragment = GX_CC_CPREV;
			gxtaaFragment = GX_CA_APREV;
		}
	}

	//now set up one more stage for object constant alpha
	pTSP->tevOrder.texCoordID = GX_TEXCOORD_NULL;
	pTSP->tevOrder.texMapID = GX_TEXMAP_NULL;
	pTSP->tevOrder.channelID = GX_COLOR0A0;

	pTSP->tevCoordGen.texCoordID = (GXTexCoordID)( GX_TEXCOORD0 + iTexCoord);
	pTSP->tevCoordGen.texGenType = GX_TG_MTX2x4;
	pTSP->tevCoordGen.texGenSrc = GX_TG_TEX0;

	//we want C=Cf
	pTSP->tevKonstSelect.kColorSel = GX_TEV_KCSEL_1;
	pTSP->tevColorIn.colorArgA = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgB = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgC = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgD = GX_CC_CPREV;

	pTSP->tevColorOp.tevOP = GX_TEV_ADD;
	pTSP->tevColorOp.tevBias = GX_TB_ZERO;
	pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
	pTSP->tevColorOp.tevClamp = GX_TRUE;
	pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

	//we want A=AfAc
	pTSP->tevKonstSelect.kAlphaSel = GX_TEV_KASEL_K0_A;
	pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
	pTSP->tevAlphaIn.alphaArgB = GX_CA_APREV;
	pTSP->tevAlphaIn.alphaArgC = GX_CA_KONST;
	pTSP->tevAlphaIn.alphaArgD = GX_CA_ZERO;

	pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
	pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
	pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
	pTSP->tevAlphaOp.tevClamp = GX_TRUE;
	pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;

	iStage++;

	//record number of stages and number of texgens defined
	m_nTevStages = iStage;
	m_nTexGens = iTexCoord;
	m_nChans = 1;

	m_bPermanent = g_ShaderMgr.GetShadersPermanent( );

	CreateDisplayList( );
}

/***************************************************************************/
// Reset the ShaderInst to default configuration
/***************************************************************************/
void ShaderInst::Clear( void)
{

	ts_TEVStageParams		*pTSP;

	int						iStage;

	m_nChans = 0;
	m_nTevStages = 1;
	m_nTexGens = 0;

	pTSP = m_aTevStageParams;

	for( iStage = 0; iStage < GX_MAX_TEVSTAGE; iStage++, pTSP++)
	{
		pTSP->tevOrder.texCoordID = GX_TEXCOORD_NULL;
		pTSP->tevOrder.texMapID = GX_TEXMAP_NULL;
		pTSP->tevOrder.channelID = GX_COLOR_NULL;

		pTSP->tevCoordGen.texCoordID = GX_TEXCOORD0;
		pTSP->tevCoordGen.texGenType = GX_TG_MTX2x4;
		pTSP->tevCoordGen.texGenSrc = GX_TG_TEX0;
		pTSP->tevCoordGen.texNormalize = GX_FALSE;
		pTSP->tevCoordGen.texPostMtx = GX_PTIDENTITY;

		//we want C=Cf
		pTSP->tevColorIn.colorArgA = GX_CC_ZERO;
		pTSP->tevColorIn.colorArgB = GX_CC_ZERO;
		pTSP->tevColorIn.colorArgC = GX_CC_ZERO;
		pTSP->tevColorIn.colorArgD = GX_CC_RASC;

		pTSP->tevColorOp.tevOP = GX_TEV_ADD;
		pTSP->tevColorOp.tevBias = GX_TB_ZERO;
		pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
		pTSP->tevColorOp.tevClamp = GX_TRUE;
		pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

		//we want A=Af
		pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
		pTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
		pTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
		pTSP->tevAlphaIn.alphaArgD = GX_CA_RASA;

		pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
		pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
		pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
		pTSP->tevAlphaOp.tevClamp = GX_TRUE;
		pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;

		pTSP->tevKonstSelect.kColorSel = GX_TEV_KCSEL_1;
		pTSP->tevKonstSelect.kAlphaSel = GX_TEV_KASEL_1;
	}

	m_bPermanent = false;

	ASSERT( ( (u32)m_DLBuffer & 0x1f) == 0);
	m_DLSize = 0;
}

static void Shader__Callback( void)
{
	LOCKUP( );
}

/***************************************************************************/
// CreateSend the Shader to the hardware
/***************************************************************************/
void ShaderInst::CreateDisplayList( void)
{

	ts_TEVStageParams		*pTSPEven,
								*pTSPOdd;

	GDLObj					gdlObj;

	g_ShaderMgr.LockDLMutex( );

	CHECK_ALIGN_32B( m_pDL);

	GDInitGDLObj( &gdlObj, m_pDL, GCN_SHADER_DL_SIZE);
	GDSetCurrent( &gdlObj);
	GDSetOverflowCallback( Shader__Callback);

	pTSPEven = m_aTevStageParams;
	pTSPOdd = pTSPEven + 1;

	for( int iStage = 0; iStage < m_nTevStages; iStage++, pTSPEven++, pTSPOdd++)
	{
		if( ( iStage & 0x01) == 0)
		{
			GDSetTevOrder(	(GXTevStageID)( GX_TEVSTAGE0 + iStage),
								pTSPEven->tevOrder.texCoordID,
								pTSPEven->tevOrder.texMapID,
								pTSPEven->tevOrder.channelID,
								pTSPOdd->tevOrder.texCoordID,
								pTSPOdd->tevOrder.texMapID,
								pTSPOdd->tevOrder.channelID);
		}

		if( iStage < GX_MAX_TEXCOORD)
		{
			GDSetTexCoordGen(	pTSPEven->tevCoordGen.texCoordID,
									pTSPEven->tevCoordGen.texGenType,
									pTSPEven->tevCoordGen.texGenSrc,
									pTSPEven->tevCoordGen.texNormalize,
									pTSPEven->tevCoordGen.texPostMtx);
		}

		if( ( iStage & 0x01) == 0)
		{
			GDSetTevKonstantSel(	(GXTevStageID)( GX_TEVSTAGE0 + iStage),
										pTSPEven->tevKonstSelect.kColorSel,
										pTSPEven->tevKonstSelect.kAlphaSel,
										pTSPOdd->tevKonstSelect.kColorSel,
										pTSPOdd->tevKonstSelect.kAlphaSel);
		}

		GDSetTevColorCalc(	(GXTevStageID)( GX_TEVSTAGE0 + iStage),
									pTSPEven->tevColorIn.colorArgA,
									pTSPEven->tevColorIn.colorArgB,
									pTSPEven->tevColorIn.colorArgC,
									pTSPEven->tevColorIn.colorArgD,
									pTSPEven->tevColorOp.tevOP,
									pTSPEven->tevColorOp.tevBias,
									pTSPEven->tevColorOp.tevScale,
									pTSPEven->tevColorOp.tevClamp,
									pTSPEven->tevColorOp.tevOutReg);

		GDSetTevAlphaCalcAndSwap(	(GXTevStageID)( GX_TEVSTAGE0 + iStage),
											pTSPEven->tevAlphaIn.alphaArgA,
											pTSPEven->tevAlphaIn.alphaArgB,
											pTSPEven->tevAlphaIn.alphaArgC,
											pTSPEven->tevAlphaIn.alphaArgD,
											pTSPEven->tevAlphaOp.tevOP,
											pTSPEven->tevAlphaOp.tevBias,
											pTSPEven->tevAlphaOp.tevScale,
											pTSPEven->tevAlphaOp.tevClamp,
											pTSPEven->tevAlphaOp.tevOutReg,
											GX_TEV_SWAP0,
											GX_TEV_SWAP0);
	}

	GDSetGenMode( m_nTexGens, m_nChans, m_nTevStages);

	GDPadCurr32( );
	GDFlushCurrToMem( );
	
	m_DLSize = GDGetCurrOffset( );

	g_ShaderMgr.UnlockDLMutex( );
}

/***************************************************************************/
// Create the ShaderMgr
/***************************************************************************/
ShaderMgr::ShaderMgr( void)
{

	ts_TEVStageParams		*pTSP;

	g_ShaderMgrPtr = this;

	OSInitMutex( &m_DLMutex);

	m_pCurrShaderInst = NULL;
	m_bPermanent = false;
	m_bLocked = false;
	m_LastTextureBrightness = 0.0f;

	m_bLightingLocked = false;
	m_ChannelFlags = -1;
	m_MaterialColor.r = 0;
	m_MaterialColor.g = 0;
	m_MaterialColor.b = 0;
	m_MaterialColor.a = 0;
	m_gxLightIDs = -1;

	memSetOneShotNewAlign( 32);
	MEM_SET_ONE_SHOT_NEW_NAME( "ShaderInst");
	m_pRTShadowShader = new ShaderInst;

	m_pRTShadowShader->m_nChans = 0;
	m_pRTShadowShader->m_nTevStages = 1;
	m_pRTShadowShader->m_nTexGens = 1;

	pTSP = m_pRTShadowShader->m_aTevStageParams;

	pTSP->tevOrder.texCoordID = GX_TEXCOORD0;
	pTSP->tevOrder.texMapID = GX_TEXMAP0;
	pTSP->tevOrder.channelID = GX_COLOR_NULL;

	pTSP->tevCoordGen.texCoordID = GX_TEXCOORD0;
	pTSP->tevCoordGen.texGenType = GX_TG_MTX3x4;
	pTSP->tevCoordGen.texGenSrc = GX_TG_POS;

	//we want C=Ct
	pTSP->tevColorIn.colorArgA = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgB = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgC = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgD = GX_CC_TEXC;

	pTSP->tevColorOp.tevOP = GX_TEV_ADD;
	pTSP->tevColorOp.tevBias = GX_TB_ZERO;
	pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
	pTSP->tevColorOp.tevClamp = GX_TRUE;
	pTSP->tevColorOp.tevOutReg = GX_TEVPREV;

	//we want A=At
	pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
	pTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
	pTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
	pTSP->tevAlphaIn.alphaArgD = GX_CA_TEXA;

	pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
	pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
	pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
	pTSP->tevAlphaOp.tevClamp = GX_TRUE;
	pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;

	m_pRTShadowShader->CreateDisplayList( );

	memSetOneShotNewAlign( 32);
	MEM_SET_ONE_SHOT_NEW_NAME("ShaderInst");
	m_pRTShadowCreate = new ShaderInst;

	m_pRTShadowCreate->m_nChans = 1;
	m_pRTShadowCreate->m_nTevStages = 1;
	m_pRTShadowCreate->m_nTexGens = 0;
	
	pTSP = m_pRTShadowCreate->m_aTevStageParams;
	
	//we want C=Cc
	pTSP->tevColorIn.colorArgA = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgB = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgC = GX_CC_ZERO;
	pTSP->tevColorIn.colorArgD = GX_CC_C2;
	
	pTSP->tevColorOp.tevOP = GX_TEV_ADD;
	pTSP->tevColorOp.tevBias = GX_TB_ZERO;
	pTSP->tevColorOp.tevScale = GX_CS_SCALE_1;
	pTSP->tevColorOp.tevClamp = GX_TRUE;
	pTSP->tevColorOp.tevOutReg = GX_TEVPREV;
	
	//we want A=Ac
	pTSP->tevAlphaIn.alphaArgA = GX_CA_ZERO;
	pTSP->tevAlphaIn.alphaArgB = GX_CA_ZERO;
	pTSP->tevAlphaIn.alphaArgC = GX_CA_ZERO;
	pTSP->tevAlphaIn.alphaArgD = GX_CA_A2;
	
	pTSP->tevAlphaOp.tevOP = GX_TEV_ADD;
	pTSP->tevAlphaOp.tevBias = GX_TB_ZERO;
	pTSP->tevAlphaOp.tevScale = GX_CS_SCALE_1;
	pTSP->tevAlphaOp.tevClamp = GX_TRUE;
	pTSP->tevAlphaOp.tevOutReg = GX_TEVPREV;

	m_pRTShadowCreate->CreateDisplayList( );

	GDLObj					gdlObj;
	
	MEM_SET_ALLOC_NAME( "Shader DL");
	m_MatrixResetDL = (void *)new char[32 * 5];
	
	GDInitGDLObj( &gdlObj, m_MatrixResetDL, GCN_SHADER_DL_SIZE);
	GDSetCurrent( &gdlObj);
	GDSetOverflowCallback( Shader__Callback);
	
	u32	u32BaseOffset;

	u32BaseOffset = GDGetCurrOffset( );
	GDSetCurrentMtx(	GX_PNMTX0,
							GX_TEXMTX0,
							GX_IDENTITY,
							GX_IDENTITY,
							GX_IDENTITY,
							GX_IDENTITY,
							GX_IDENTITY,
							GX_IDENTITY,
							GX_IDENTITY);
	GDPadCurr32( );
	m_MatrixResetDLSize = GDGetCurrOffset( ) - u32BaseOffset;
	ASSERT( m_MatrixResetDLSize == 32);

	u32BaseOffset = GDGetCurrOffset( );
	m_CullFrontDL = GDGetCurrPointer( );
	GDSetCullMode( GX_CULL_FRONT);
	GDPadCurr32( );
	m_CullFrontDLSize = GDGetCurrOffset( ) - u32BaseOffset;
	ASSERT( m_CullFrontDLSize == 32);
	
	u32BaseOffset = GDGetCurrOffset( );
	m_CullBackDL = GDGetCurrPointer( );
	GDSetCullMode( GX_CULL_BACK);
	GDPadCurr32( );
	m_CullBackDLSize = GDGetCurrOffset( ) - u32BaseOffset;
	ASSERT( m_CullBackDLSize == 32);
	
	u32BaseOffset = GDGetCurrOffset( );
	m_CullNoneDL = GDGetCurrPointer( );
	GDSetCullMode( GX_CULL_NONE);
	GDPadCurr32( );
	m_CullNoneDLSize = GDGetCurrOffset( ) - u32BaseOffset;
	ASSERT( m_CullNoneDLSize == 32);
	
	GDFlushCurrToMem( );
}

/***************************************************************************/
// Get a shader from a texture_context. if it doesn't exist, create it
/***************************************************************************/
ShaderInst * ShaderMgr::GetShader( ts_TextureContext *pTextureContext, bool bTEVGenEnvUVs)
{
	
	ShaderInst		*pNewShader,
						*pCurrShader;

	MEM_SET_ONE_SHOT_NEW_NAME("ShaderInst");
	memSetOneShotNewFlags( MEM_FLG_PERMANENT);
	memSetOneShotNewAlign( 32);
	pNewShader = new ShaderInst( pTextureContext, bTEVGenEnvUVs);
	ASSERT( pNewShader);

	pCurrShader = m_ShaderInstList.Head( );

	while( pCurrShader)
	{
		if( *pCurrShader == *pNewShader)
		{
			delete pNewShader;
			return( pCurrShader);
		}
		pCurrShader = pCurrShader->next;
	}

	m_ShaderInstList.AddHead( pNewShader);

	return( pNewShader);
}

/***************************************************************************/
// Reset the current shader to none
/***************************************************************************/
void ShaderMgr::Reset( int mask)
{
	if( mask & SHADER_RESET_SHADER)
	{
		m_pCurrShaderInst = NULL;
		m_LastTextureBrightness = 0.0f;
		m_bLocked = false;
	}

	if( mask & SHADER_RESET_LIGHTING)
	{
		m_ChannelFlags = -1;
		m_bLightingLocked = false;
		m_gxLightIDs = -1;
	}

	if( mask & SHADER_RESET_MATCOLOR)
	{
		m_MaterialColor.r = 0;
		m_MaterialColor.g = 0;
		m_MaterialColor.b = 0;
		m_MaterialColor.a = 0;
	}

	if( mask & SHADER_RESET_MATRIX)
	{
		ResetCurrentMatrixIndices( );
	}
}

/***************************************************************************/
// Reset the current shader to none
/***************************************************************************/
void ShaderMgr::Cleanup( void)
{

	ShaderInst		*pShader;

	Reset( );

	pShader = m_ShaderInstList.Head( );

	while( pShader)
	{
		if( pShader->m_bPermanent)
		{
			pShader = pShader->next;
		}
		else
		{
			ShaderInst		*pRemove = pShader;
			pShader = pShader->next;

			m_ShaderInstList.Remove( pRemove);
		}
	}
}

/***************************************************************************/
// Setup the lighting channel parameters if changed
/***************************************************************************/
void ShaderMgr::SetupLightingChannel( u32 channelFlags, GXColor &materialColor)
{
	
	GXColor	gxMatColor;

	if( m_bLightingLocked)
	{
		return;
	}

	//we always need to do this because the
	//light color could have changed
	if( channelFlags & GCN_CHANNEL_HAS_LIGHTS)
	{
		RenderState__Request( RS_SEND_LIGHTS, RS_TRUE );
	}

	if(	( materialColor.r != m_MaterialColor.r) ||
			( materialColor.g != m_MaterialColor.g) ||
			( materialColor.b != m_MaterialColor.b) ||
			( materialColor.a != m_MaterialColor.a))
	{
		m_MaterialColor = materialColor;
 
		GXSetChanMatColor( GX_COLOR0A0, materialColor);
	}

	if( channelFlags != m_ChannelFlags || gxLightIDs != m_gxLightIDs)
	{
		m_ChannelFlags = channelFlags;
		m_gxLightIDs = gxLightIDs;

		if( channelFlags & GCN_CHANNEL_HAS_LIGHTS)
		{
			//			RenderState__Request( RS_TEXTURE_BRIGHTNESS, RenderState__ftoi( 2.0f ));
			
			GXBool	gxLightEnable = GX_TRUE;

			if( gxLightIDs == GX_LIGHT_NULL)
			{
				gxLightEnable = GX_FALSE;
			}

			if( channelFlags & GCN_CHANNEL_HAS_VTXCOLOR)
			{
				//use the vertex colors as ambient
				GXSetChanCtrl(	GX_COLOR0,
									gxLightEnable,
									GX_SRC_VTX,
									GX_SRC_REG,
									(GXLightID)gxLightIDs,
									GX_DF_CLAMP,
									GX_AF_SPOT);
				GXSetChanCtrl(	GX_ALPHA0,
									GX_FALSE,
									GX_SRC_REG,
									GX_SRC_VTX,
									GX_LIGHT_NULL,
									GX_DF_CLAMP,
									GX_AF_SPOT);
			}
			else
			{
				GXSetChanCtrl(	GX_COLOR0,
									gxLightEnable,
									GX_SRC_REG,
									GX_SRC_REG,
									(GXLightID)gxLightIDs,
									GX_DF_CLAMP,
									GX_AF_SPOT);
				GXSetChanCtrl(	GX_ALPHA0,
									GX_FALSE,
									GX_SRC_REG,
									GX_SRC_REG,
									GX_LIGHT_NULL,
									GX_DF_CLAMP,
									GX_AF_SPOT);
			}
		}
		else
		{
//			RenderState__Request( RS_TEXTURE_BRIGHTNESS, RenderState__ftoi( 1.0f));
			if( channelFlags & GCN_CHANNEL_HAS_VTXCOLOR)
			{
				GXSetChanCtrl(	GX_COLOR0A0,
									GX_FALSE,
									GX_SRC_REG,
									GX_SRC_VTX,
									GX_LIGHT_NULL,
									GX_DF_CLAMP,
									GX_AF_SPOT);
			}
			else
			{
				GXSetChanCtrl(	GX_COLOR0A0,
									GX_FALSE,
									GX_SRC_REG,
									GX_SRC_REG,
									GX_LIGHT_NULL,
									GX_DF_CLAMP,
									GX_AF_SPOT);
			}
		}
	}
}

