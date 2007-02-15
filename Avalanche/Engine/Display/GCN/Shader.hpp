#include "Display/Common/RenderState.h"

/***************************************************************************/
// Compare two shaders to see if they are equal
/***************************************************************************/
inline int ShaderInst::operator==( const ShaderInst &compShaderInst) const
{
	if(	m_nTevStages != compShaderInst.m_nTevStages ||
			m_nTexGens != compShaderInst.m_nTexGens ||
			m_nChans != compShaderInst.m_nChans)
	{
		return( false);
	}
	
	for( int i = 0; i < m_nTevStages; i++)
	{
		if( m_aTevStageParams[i] != compShaderInst.m_aTevStageParams[i])
		{
			return( false);
		}
	}
	
	return( true);
}

/***************************************************************************/
// Send the Shader to the hardware
/***************************************************************************/
inline void ShaderInst::Apply( void)
{
	
#if 0
#ifndef CDROM
	extern u16 u16CurrToken;
	GXSetDrawSync( u16CurrToken);
	OSReport( "Applying shader 0x%08x as token %d\n", this, u16CurrToken++);
#endif
#endif
	
	GXCallDisplayList( m_pDL, m_DLSize);
}

/***************************************************************************/
// Set the hardware to use the passed ShaderInst
/***************************************************************************/
inline void ShaderMgr::UseShader( ShaderInst *pShaderInst)
{
	if( m_bLocked)
	{
		return;
	}
	
	if(	m_pCurrShaderInst != pShaderInst)
	{
		pShaderInst->Apply( );
		m_pCurrShaderInst = pShaderInst;
	}
}

/***************************************************************************/
// Set the hardware to the requested culling mode
/***************************************************************************/
inline void ShaderMgr::SetCullMode( GXCullMode gxCullMode)
{
	switch( gxCullMode)
	{
		case GX_CULL_FRONT:
			GXCallDisplayList( m_CullFrontDL, m_CullFrontDLSize);
			break;

		case GX_CULL_BACK:
			GXCallDisplayList( m_CullBackDL, m_CullBackDLSize);
			break;

		case GX_CULL_NONE:
		default:
			GXCallDisplayList( m_CullNoneDL, m_CullNoneDLSize);
			break;
	}
}
