
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Thu Apr 11 10:17:28 2002
 */
/* Compiler settings for PaintX.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __PaintX_h__
#define __PaintX_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPictureDecoder_FWD_DEFINED__
#define __IPictureDecoder_FWD_DEFINED__
typedef interface IPictureDecoder IPictureDecoder;
#endif 	/* __IPictureDecoder_FWD_DEFINED__ */


#ifndef __PictureDecoder_FWD_DEFINED__
#define __PictureDecoder_FWD_DEFINED__

#ifdef __cplusplus
typedef class PictureDecoder PictureDecoder;
#else
typedef struct PictureDecoder PictureDecoder;
#endif /* __cplusplus */

#endif 	/* __PictureDecoder_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IPictureDecoder_INTERFACE_DEFINED__
#define __IPictureDecoder_INTERFACE_DEFINED__

/* interface IPictureDecoder */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPictureDecoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B69FE5D2-9F53-11D2-B13F-444553540000")
    IPictureDecoder : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadPicture( 
            /* [in] */ BSTR Filename,
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadResPicture( 
            /* [in] */ int ResourceID,
            /* [retval][out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPictureDecoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPictureDecoder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPictureDecoder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPictureDecoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPictureDecoder * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPictureDecoder * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPictureDecoder * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPictureDecoder * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadPicture )( 
            IPictureDecoder * This,
            /* [in] */ BSTR Filename,
            /* [retval][out] */ IUnknown **ppUnk);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadResPicture )( 
            IPictureDecoder * This,
            /* [in] */ int ResourceID,
            /* [retval][out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } IPictureDecoderVtbl;

    interface IPictureDecoder
    {
        CONST_VTBL struct IPictureDecoderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPictureDecoder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPictureDecoder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPictureDecoder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPictureDecoder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPictureDecoder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPictureDecoder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPictureDecoder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPictureDecoder_LoadPicture(This,Filename,ppUnk)	\
    (This)->lpVtbl -> LoadPicture(This,Filename,ppUnk)

#define IPictureDecoder_LoadResPicture(This,ResourceID,ppUnk)	\
    (This)->lpVtbl -> LoadResPicture(This,ResourceID,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPictureDecoder_LoadPicture_Proxy( 
    IPictureDecoder * This,
    /* [in] */ BSTR Filename,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB IPictureDecoder_LoadPicture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPictureDecoder_LoadResPicture_Proxy( 
    IPictureDecoder * This,
    /* [in] */ int ResourceID,
    /* [retval][out] */ IUnknown **ppUnk);


void __RPC_STUB IPictureDecoder_LoadResPicture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPictureDecoder_INTERFACE_DEFINED__ */



#ifndef __PAINTXLib_LIBRARY_DEFINED__
#define __PAINTXLib_LIBRARY_DEFINED__

/* library PAINTXLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PAINTXLib;

EXTERN_C const CLSID CLSID_PictureDecoder;

#ifdef __cplusplus

class DECLSPEC_UUID("B69FE5D3-9F53-11D2-B13F-444553540000")
PictureDecoder;
#endif
#endif /* __PAINTXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


