/***********************************/
/*                                 */
/* ParticleSwoosh.h                */
/* Pigpen particle tool  11/05/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* combined particle/swoosh class  */
/*                                 */
/***********************************/

/* some of the page dialogs are used for both particles and swooshes. this makes the code easier to read by defining the
common routines as an if/else */

#ifndef __PARTICLESWOOSH_H
#define __PARTICLESWOOSH_H

/******************* includes ****************************/

#include "Particle.h"
#include "Swoosh.h"

/* game engine includes */

/******************* defines *****************************/

/******************* structures **************************/

/******************* forward declarations ****************/

/******************* class *******************************/

class CParticleSwoosh : public CObject
{
public:

// Constructor/destructor

	CParticleSwoosh()		{ m_pParticle = NULL; m_pSwoosh = NULL; }
	~CParticleSwoosh()	{}

// Implementation

protected:

	/* at least one of these must be non-NULL */

	CParticle *m_pParticle;
	CSwoosh *m_pSwoosh;

public:

	/* set pointers */

	void Set(CParticle *pParticle, CSwoosh *pSwoosh)	{ m_pParticle = pParticle; m_pSwoosh = pSwoosh; ASSERT((pParticle == NULL) || (pSwoosh == NULL)); }
	bool IsValid()											{ return((m_pParticle) || (m_pSwoosh)); }
	CParticle *GetParticle()							{ return(m_pParticle); }
	CSwoosh *GetSwoosh()									{ return(m_pSwoosh); }

	/* routines-- whatever is common between CParticle and CSwoosh is valid. commented-out routine are candidates for */
	/* future expansion, if needed (but not necessarily an exhaustive list) */

//	CString& GetName(void)								{ if (m_pParticle) return(m_pParticle->GetName()); return(m_pSwoosh->GetName()); }
	bool IsMaintainAspectRatio(void)					{ if (m_pParticle) return(m_pParticle->IsMaintainAspectRatio()); return(true); }
	bool IsColorPair(void)								{ if (m_pParticle) return(m_pParticle->GetColorControl() == PART_COLOR_PAIR); return(m_pSwoosh->GetColorControl() == SWOO_COLOR_PAIR); }
	bool IsColorRGB(void)								{ if (m_pParticle) return(m_pParticle->GetColorControl() == PART_COLOR_RGB); return(m_pSwoosh->GetColorControl() == SWOO_COLOR_RGB); }
//	void SetName(char *pName)							{ m_Name = pName; }
//	bool IsCommon(void)									{ return((m_u32Flags & SWOO_COMMON) != 0); }
//	void SetGraphic(CGraphic *pGraphic)				{ m_pGraphic = pGraphic; }
//	CGraphic *GetGraphic(void)							{ return(m_pGraphic); }
//	void SetGraphicIndex(int nIndex)					{ m_nGraphicIndex = nIndex; }
//	int GetGraphicIndex(void)							{ return(m_nGraphicIndex); }
//	int GetHeight(void)									{ if (m_pGraphic) return(m_pGraphic->GetHeight()); else return(1); }
//	int GetWidth(void)									{ if (m_pGraphic) return(m_pGraphic->GetWidth()); else return(1); }
//	void SetFPS(float fFPS)								{ m_fFPS = fFPS; }
//	void SetTempIndex(int nIndex)						{ m_nTempIndex = nIndex; }
//	int GetTempIndex(void)								{ return(m_nTempIndex); }
//	void SetExportStatus(int nStatus)				{ m_nExportStatus = nStatus; }
//	int GetExportStatus(void)							{ return(m_nExportStatus); }
//	void SetExportDataSize(int nSize)				{ m_nExportDataSize = nSize; }
//	int GetExportDataSize(void)						{ return(m_nExportDataSize); }
//	void SetSelected(bool bSelected)					{ if (bSelected) m_u32Flags |= SWOO_SELECTED; else m_u32Flags &= ~SWOO_SELECTED; }
//	bool GetSelected(void)								{ return((m_u32Flags & SWOO_SELECTED) != 0); }
	CEnvelope *GetCurrentEnvelope(void)				{ if (m_pParticle) return(m_pParticle->GetCurrentEnvelope()); return(m_pSwoosh->GetCurrentEnvelope()); }
	void SetCurrentEnvelopeIndex(int nIndex)		{ if (m_pParticle) m_pParticle->SetCurrentEnvelopeIndex(nIndex); else m_pSwoosh->SetCurrentEnvelopeIndex(nIndex); }
	int GetCurrentEnvelopeIndex(void)				{ if (m_pParticle) return(m_pParticle->GetCurrentEnvelopeIndex()); return(m_pSwoosh->GetCurrentEnvelopeIndex()); }
	CEnvelope *GetEnvelope(int nIndex)				{ if (m_pParticle) return(m_pParticle->GetEnvelope(nIndex)); return(m_pSwoosh->GetEnvelope(nIndex)); }
	bool EnvelopeHidden(int nIndex)					{ if (m_pParticle) return(m_pParticle->EnvelopeHidden(nIndex)); return(m_pSwoosh->EnvelopeHidden(nIndex)); }
//	void SetEngineData(void *pData)					{ m_pEngineData = pData; }
//	void *GetEngineData(void)							{ return(m_pEngineData); }
	void SetColor(int nIndex, COLORREF Color)		{ if (m_pParticle) m_pParticle->SetColor(nIndex, Color); else m_pSwoosh->SetColor(nIndex, Color); }
	COLORREF GetColor(int nIndex)						{ if (m_pParticle) return(m_pParticle->GetColor(nIndex)); return(m_pSwoosh->GetColor(nIndex)); }
//	void SetUsedCount(int nCount)						{ m_nUsedCount = nCount; }
//	int GetUsedCount(void)								{ return(m_nUsedCount); }
//	void IncUsedCount(void)								{ m_nUsedCount++; }
//	void SetFlag(u32 u32FlagBit)						{ m_u32Flags |= u32FlagBit; }
//	void ClearFlag(u32 u32FlagBit)					{ m_u32Flags &= ~u32FlagBit; }
//	bool IsFlagSet(u32 u32FlagBit)					{ return((m_u32Flags & u32FlagBit) == u32FlagBit); }	// can be more than one bit passed, will check for all bits set
//	unsigned long GetLoadVersion(void)				{ return(m_ulLoadVersion); }
	void *GetDataPointer(int nDataID)				{ if (m_pParticle) return(m_pParticle->GetDataPointer(nDataID)); return(m_pSwoosh->GetDataPointer(nDataID)); }
};

#endif // __PARTICLESWOOSH_H
