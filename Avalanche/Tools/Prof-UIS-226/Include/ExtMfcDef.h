// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2004 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// http://www.fossware.com
// mailto:support@fossware.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#if (!defined __EXT_MFC_DEF_H)
#define __EXT_MFC_DEF_H

#define __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES (0x0000FFFFL)

// disable warning 4201
#pragma warning( push )
#pragma warning ( disable : 4201 )
	#include <mmsystem.h>
// rollback warning 4201
#pragma warning( pop )

// __PROF_UIS_VERSION_DWORD is required for static library builds
#define __PROF_UIS_VERSION_DWORD DWORD( 0x02020601 )

// the multiple monitor stub can be included into the library
// builds (used in ExtPaintManager.cpp to define
// COMPILE_MULTIMON_STUBS symbol before include multimon.h)
#if (defined _AFXDLL)
	#define __PROF_UIS_BUILTIN_MULTIMON_STUB
#endif // _AFXDLL
#pragma message("   Prof-UIS multiple monitor support:")
#if (defined __PROF_UIS_BUILTIN_MULTIMON_STUB)
	#pragma message("      built-in")
#else
	#pragma message("      external")
#endif // __PROF_UIS_BUILTIN_MULTIMON_STUB

// Prof-UIS basic library
#if (defined _AFXDLL)
	#if (defined __STATPROFUIS_WITH_DLLMFC__)
		#define __PROF_UIS_MANAGE_STATE
		#if (defined _DEBUG)
			#if (defined _UNICODE)
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226yud.lib"
			#elif (defined _MBCS)
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226ymd.lib"
			#else
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226ynd.lib"
			#endif // _UNICODE
		#else
			#if (defined _UNICODE)
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226yu.lib"
			#elif (defined _MBCS)
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226ym.lib"
			#else
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226yn.lib"
			#endif // _UNICODE
		#endif // _DEBUG
	#else

		#define __PROF_UIS_PREFORMS_STATE_MANAGING

		struct AFX_MAINTAIN_STATE2;
		class AFX_MODULE_STATE;

		class
#if (defined __PROF_UIS_IMPL__)
			_declspec(dllexport)
#else
			_declspec(dllimport)
#endif // (defined __PROF_UIS_IMPL__)
			CExt_ProfUIS_ModuleState
		{
			static AFX_MODULE_STATE * g_pExternalModuleState;
			AFX_MAINTAIN_STATE2 * m_pAfxState;
		public:
			CExt_ProfUIS_ModuleState();
			~CExt_ProfUIS_ModuleState();
			static void InitExtension(
				AFX_MODULE_STATE * pExternalModuleState = NULL
				);
		};
		#define __PROF_UIS_MANAGE_STATE CExt_ProfUIS_ModuleState the_ProfUIS_ModuleState

		#if (defined __PROF_UIS_FOR_REGULAR_DLL)
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226ud-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226ud-RDE.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226md-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226md-RDE.lib"
				#else
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226nd-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226nd-RDE.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226u-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226u-RDE.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226m-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226m-RDE.lib"
				#else
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226n-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226n-RDE.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#else
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226ud.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226ud.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226md.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226md.lib"
				#else
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226nd.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226nd.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226u.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226u.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226m.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226m.lib"
				#else
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS226n.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS226n.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#endif // else from (defined __PROF_UIS_FOR_REGULAR_DLL)
	#endif // __STATPROFUIS_WITH_DLLMFC__
#else
		#define __PROF_UIS_MANAGE_STATE
		#if (defined _DEBUG)
			#if (defined _UNICODE)
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226sud.lib"
			#elif (defined _MBCS)
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226smd.lib"
			#else
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226snd.lib"
			#endif // _UNICODE
		#else
			#if (defined _UNICODE)
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226su.lib"
			#elif (defined _MBCS)
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226sm.lib"
			#else
				#define __PROF_UIS_LIB_NAME		   "ProfUIS226sn.lib"
			#endif // _UNICODE
		#endif // _DEBUG
#endif // _AFXDLL

#if (defined __PROF_UIS_IMPL__)
	#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		#define __PROF_UIS_API _declspec(dllexport)
	#else
		#define __PROF_UIS_API
	#endif // _AFXDLL
#else 
	#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		#define __PROF_UIS_API _declspec(dllimport)
	#else
		#define __PROF_UIS_API
	#endif // _AFXDLL

	#pragma message("   Automatically linking with ProfUIS226 library")
	#pragma message("      (Professional User Interface Suite)")
	#pragma comment( lib, __PROF_UIS_LIB_NAME ) 
#endif

#if (defined __PROF_UIS_ADV_IMPL__)
	#if (!defined __ENABLED_PROF_UIS_ADV__)
		#define __ENABLED_PROF_UIS_ADV__
	#endif
#endif // #if (defined __PROF_UIS_ADV_IMPL__)

#if (defined __ENABLED_PROF_UIS_ADV__)

	// __PROF_UIS_VERSION_DWORD is required for static library builds
	#define __PROF_UIS_ADV_VERSION_DWORD DWORD( 0x02010501 )

	// Prof-UIS advanced library
	// (now for internal use only)

	#if (defined _AFXDLL)
		#if (defined __STATPROFUIS_WITH_DLLMFC__)
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226yud.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226ymd.lib"
				#else
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226ynd.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226yu.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226ym.lib"
				#else
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226yn.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#else
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_ADV_MODULE_NAME	_T("ProfUISAdv226ud.dll")
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226ud.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_ADV_MODULE_NAME	_T("ProfUISAdv226md.dll")
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226md.lib"
				#else
					#define __PROF_UIS_ADV_MODULE_NAME	_T("ProfUISAdv226nd.dll")
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226nd.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_ADV_MODULE_NAME	_T("ProfUISAdv226u.dll")
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226u.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_ADV_MODULE_NAME	_T("ProfUISAdv226m.dll")
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226m.lib"
				#else
					#define __PROF_UIS_ADV_MODULE_NAME	_T("ProfUISAdv226n.dll")
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226n.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#endif // __STATPROFUIS_WITH_DLLMFC__
	#else
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226sud.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226smd.lib"
				#else
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226snd.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226su.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226sm.lib"
				#else
					#define __PROF_UIS_ADV_LIB_NAME		   "ProfUISAdv226sn.lib"
				#endif // _UNICODE
			#endif // _DEBUG
	#endif // _AFXDLL

	#if defined __PROF_UIS_ADV_IMPL__
		#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			#define __PROF_UIS_ADV_API _declspec(dllexport)
		#else
			#define __PROF_UIS_ADV_API
		#endif
	#else 
		#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			#define __PROF_UIS_ADV_API _declspec(dllimport)
		#else
			#define __PROF_UIS_ADV_API
		#endif

		#pragma message("   Automatically linking with ProfUISAdv226 library")
		#pragma message("      (Professional User Interface Suite / Advanced Stuff)")
		#pragma comment( lib, __PROF_UIS_ADV_LIB_NAME ) 
	#endif
#endif // __ENABLED_PROF_UIS_ADV__

// Prof-UIS registry names
#define __PROF_UIS_REG_SECTION			_T("ProfUIS226")
#define __PROF_UIS_REG_PROFILES			_T("Profiles")
#define __PROF_UIS_REG_COMMAND_MANAGER	_T("CommandManager")
#define __PROF_UIS_REG_CONTROL_BAR		_T("ControlBar")
#define __PROF_UIS_REG_CUSTOMIZATION	_T("Customization")

// Prof-UIS native unicode character type support for VC++ .NET 2002 or later
#if (!defined __ExtMfcSafeNativeTCHAR_H)
	#include <ExtMfcSafeNativeTCHAR.h>
#endif

#ifndef _AFX_NO_OLE_SUPPORT
	#ifndef __AFXCTL_H__
		#include <AfxCtl.h>     // MFC OLE Control support
	#endif
	#define DECLARE_PROF_UIS_WINDOW_METHODS \
		virtual bool OnQueryWindowEnabledState() const \
		{ \
			ASSERT_VALID( this ); \
			ASSERT_KINDOF( CWnd, this ); \
			ASSERT( GetSafeHwnd() != NULL && ::IsWindow(GetSafeHwnd()) ); \
			bool bEnabled = IsWindowEnabled() ? true : false; \
			CWnd * pWndParent = GetParent(); \
			if( pWndParent != NULL ) \
			{ \
				COleControl * pOleControl = \
					DYNAMIC_DOWNCAST( COleControl, pWndParent ); \
				if( pOleControl != NULL ) \
				{ \
					if( pOleControl->AmbientUserMode() == FALSE ) \
						return true; \
				} \
			} \
			return bEnabled; \
		}
#else
	#define DECLARE_PROF_UIS_WINDOW_METHODS \
		virtual bool OnQueryWindowEnabledState() const \
		{ \
			ASSERT_VALID( this ); \
			ASSERT_KINDOF( CWnd, this ); \
			ASSERT( GetSafeHwnd() != NULL && ::IsWindow(GetSafeHwnd()) ); \
			bool bEnabled = IsWindowEnabled() ? true : false; \
			return bEnabled; \
		}
#endif // _AFX_NO_OLE_SUPPORT

#endif // __EXT_MFC_DEF_H
