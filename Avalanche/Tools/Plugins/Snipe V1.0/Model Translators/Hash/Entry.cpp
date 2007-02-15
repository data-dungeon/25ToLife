//
#include "stdafx.h"
#include "HashPlugin.h"
#include "HSnipePlugin.h"
#include "HModelTranslatorPlugin.h"
#include "Ply.h"
#include "Av2.h"
#include "MDL/Mdl.h"
#include "KFM/KFM.h"

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

extern CSnipePluginApp g_theApp;

extern "C" __declspec(dllexport) void InitSnipePlugin( HSnipePlugin *pSnipePlugin )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   pSnipePlugin->SetName( "HASHIO" );

   ADD_SNIPE_MODEL_TRANSLATOR_PLUGIN(pSnipePlugin, PlyTranslator);
   ADD_SNIPE_MODEL_TRANSLATOR_PLUGIN(pSnipePlugin, Av2Translator);
   ADD_SNIPE_MODEL_TRANSLATOR_PLUGIN(pSnipePlugin, MdlTranslator);
   ADD_SNIPE_MOTION_TRANSLATOR_PLUGIN(pSnipePlugin, KFMTranslator);
}