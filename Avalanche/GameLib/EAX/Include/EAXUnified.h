/*******************************************************************\
*																	*
*  EAXUnified.h - Defines IEAXUnified interface.					*
*				  (Added to version 3.06 of EAX.DLL)				*
*																	*
********************************************************************/

#ifndef EAX_UNIFIED_H_INCLUDED
#define EAX_UNIFIED_H_INCLUDED

//
// IEAXUnifed
//

#ifdef __cplusplus
struct IEAXUnified;
#endif // __cplusplus

typedef struct IEAXUnified	*LPEAXUNIFIED;

#define EAX_VERSION_1	0x01
#define EAX_VERSION_2	0x02
#define EAX_VERSION_25	0x04
#define EAX_VERSION_3	0x08
#define EAX_VERSION_4	0x10

typedef struct
{
	unsigned long ulSize;
	unsigned long ulEAXVersionsSupported;
} EAXUNIFIEDCAPS, *LPEAXUNIFIEDCAPS;

DEFINE_GUID(IID_IEAXUnified, 0xfa82931b, 0xde98, 0x4a68, 0xa7, 0xfe, 0xd6, 0x83, 0xe3, 0x8a, 0x65, 0x2);

#undef INTERFACE
#define INTERFACE IEAXUnified

DECLARE_INTERFACE_(IEAXUnified, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID *) PURE;
	STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
	STDMETHOD_(ULONG,Release)	(THIS) PURE;

	// IEAXUnified methods
	STDMETHOD(GetEmulated)		(THIS_ ULONG *lpulEmulated) PURE;
	STDMETHOD(GetNativeSupport)	(THIS_ ULONG *lpulNativeSupport) PURE;
	STDMETHOD(GetCaps)			(THIS_ LPEAXUNIFIEDCAPS lpEAXUnifiedCaps) PURE;
	STDMETHOD(GetEAXDSBuffer)	(THIS_ LPDIRECTSOUNDBUFFER lpDSB, LPDIRECTSOUNDBUFFER *lplpEAXDSB) PURE;
};

#define IEAXUnified_QueryInterface(p,a,b)		IUnknown_QueryInterface(p,a,b)
#define IEAXUnified_AddRef(p)					IUnknown_AddRef(p)
#define IEAXUnified_Release(p)					IUnknown_Release(p)

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IEAXUnified_GetEmulated(p,a)			(p)->lpVtbl->GetEmulated(p,a)
#define IEAXUnified_GetNativeSupport(p,a)		(p)->lpVtbl->GetNativeSupport(p,a)
#define IEAXUnified_GetCaps(p,a)				(p)->lpVtbl->GetCaps(p,a)
#define IEAXUnified_GetEAXDSBuffer(p,a,b)		(p)->lpVtbl->GetEAXDSBuffer(p,a,b)
#else // !defined(__cplusplus) || defined(CINTERFACE)
#define IEAXUnified_GetEmulated(p,a)			(p)->GetEmulated(a)
#define IEAXUnified_GetNativeSupport(p,a)		(p)->GetNativeSupport(a)
#define IEAXUnified_GetCaps(p,a)				(p)->GetCaps(a)
#define IEAXUnified_GetEAXDSBuffer(p,a,b)		(p)->GetEAXDSBuffer(a,b)
#endif // !defined(__cplusplus) || defined(CINTERFACE)

#endif // EAX_UNIFIED_H_INCLUDED