//*****************************************************************************
/*!	\file DotXSIImportExport.cpp
\brief Defines the entry point for the DLL application.

by: Martin Barrette

(c) Copyright 1996 - 2003 Softimage/Avid Corporation

*/
//*****************************************************************************
#include "stdafx.h"
#include <xsi_value.h>
#include <xsi_status.h>
#include <xsi_application.h>
#include <xsi_plugin.h>
#include <xsi_pluginitem.h>
#include <xsi_pluginregistrar.h>
#include <xsi_pluginitem.h>
#include <xsi_command.h>
#include <xsi_argument.h>
#include <xsi_context.h>
#include <xsi_menuitem.h>
#include <xsi_menu.h>
#include <xsi_customproperty.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgeventcontext.h>
#include <xsi_selection.h>
#include <xsi_comapihandler.h>
#include "cexporttup.h"
#include "cimporttup.h"
#include "helper.h"


using namespace XSI;

CString GetUserSelectedObject();
CValue InspectObj( const CString& in_inputobjs, const CString& in_keywords, const CString& in_title, const CValue& in_mode, bool in_throw );
void DeleteObj( const CValue& in_inputobj );

#ifdef unix
extern "C"
#endif
CStatus XSILoadPlugin( XSI::PluginRegistrar& in_reg )
{
   in_reg.PutAuthor( L"Avalanche Software" );
   in_reg.PutName( L"C++ Import Export Plug-in" );	in_reg.PutVersion( 1, 0 );

   // register the commands plugin item.
   in_reg.RegisterCommand( L"ExportTupFunction", L"ExportTupFunction" );
   in_reg.RegisterCommand( L"ImportTupFunction", L"ImportTupFunction" );

   // register the menu for the import and export
   in_reg.RegisterMenu(siMenuMainFileExportID, L"CPPExportTupCommand", false, false);
   in_reg.RegisterMenu(siMenuMainFileImportID, L"CPPImportTupCommand", false, false);

   // register the properties for Export and Import
   in_reg.RegisterProperty( L"ExportTupOptions" );
   in_reg.RegisterProperty( L"ImportTupOptions" );

   return XSI::CStatus::OK;	
}

#ifdef unix
extern "C"
#endif
XSI::CStatus XSIUnloadPlugin( const XSI::PluginRegistrar& in_reg )
{
   Application app;
#ifdef _DEBUG
   app.LogMessage( in_reg.GetName() + L" has been unloaded.");
#endif
   return XSI::CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus ExportTupFunction_Init( const XSI::CRef& in_context )
{
   Context ctx(in_context);
   Command cmd(ctx.GetSource());

   Application app;
   app.LogMessage( L"Defining: " + cmd.GetName() );

   ArgumentArray args = cmd.GetArguments();

   args.Add( L"arg0", L"c:/scene.tup" );
   args.Add( L"arg1", L"" );
   return XSI::CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus ExportTupFunction_Execute( XSI::CRef& in_context )
{
   Application app;
   Context ctxt(in_context);
   CValueArray args = ctxt.GetAttribute( L"Arguments" );

#ifdef _DEBUG
   for (long i=0; i<args.GetCount(); i++)
   {
      app.LogMessage( L"Arg" + CValue(i).GetAsText() + L": " + 
         args[i].GetAsText() );			
   }
#endif

   if ( args.GetCount() != 2 ) 
   {
      // Arguments of the command might not be properly registered
      return CStatus::InvalidArgument ;
   }

   CExportTup exportTup;

   CStatus st = exportTup.ExportFile(args[0].GetAsText(), args[1].GetAsText());
   return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus ImportTupFunction_Init( const XSI::CRef& in_context )
{
   Context ctx(in_context);
   Command cmd(ctx.GetSource());

   Application app;
   app.LogMessage( L"Defining: " + cmd.GetName() );

   ArgumentArray args = cmd.GetArguments();

   args.Add( L"arg0", L"c:/scene.tup" );
   return XSI::CStatus::OK;

}

#ifdef unix
extern "C"
#endif
XSI::CStatus ImportTupFunction_Execute( XSI::CRef& in_context )
{
   Application app;
   Context ctxt(in_context);
   CValueArray args = ctxt.GetAttribute( L"Arguments" );

#ifdef _DEBUG
   for (long i=0; i<args.GetCount(); i++)
   {
      app.LogMessage( L"Arg" + CValue(i).GetAsText() + L": " + 
         args[i].GetAsText() );			
   }
#endif

   CImportTup importTup;

   CStatus st = importTup.ImportFile(args[0].GetAsText());
   return st;

}

#ifdef unix
extern "C"
#endif
XSI::CStatus CPPExportTupCommand_Init( XSI::CRef& in_ref )
{
   Context ctxt = in_ref;
   Menu menu = ctxt.GetSource();

   CStatus st;
   MenuItem item;
   menu.AddCallbackItem(L"Export Tup...", L"OnTupExportMenu", item);

   return CStatus::OK;	
}
#ifdef unix
extern "C"
#endif
XSI::CStatus OnTupExportMenu( XSI::CRef& in_ref )
{	
   Application app;
   CStatus st(CStatus::OK);
   Property prop = app.GetActiveSceneRoot().AddProperty( L"ExportTupOptions" ) ;

   // InspectObj Returns true if the command was cancelled otherwise it returns false. 
   bool ret = InspectObj(L"ExportTupOptions",CValue(),L"Tup Export (example)",(long)4,true);
   if (!ret)
   {
      Parameter exportObjectParam = prop.GetParameters().GetItem( L"ExportObject" );
      CString strObjectToExport = exportObjectParam.GetValue();
      Parameter exportSceneParam = prop.GetParameters().GetItem( L"ExportScene" );
      bool exportSceneBool = exportSceneParam.GetValue();
      Parameter exportFileParam = prop.GetParameters().GetItem( L"ExportFile" );
      CString strFilePath = exportFileParam.GetValue();

      if ( strFilePath.Length() > 0)
      {
         if ( exportSceneBool )
            strObjectToExport = L"";
      }
      else
      {
         DeleteObj( L"ExportTupOptions" );
         return CStatus::InvalidArgument;
      }

      //Launch the command 
      CExportTup exportTup;

      st = exportTup.ExportFile(strFilePath, strObjectToExport);

   }
   DeleteObj( L"ExportTupOptions" );
   return st;	
}

#ifdef unix
extern "C"
#endif
XSI::CStatus CPPImportTupCommand_Init( XSI::CRef& in_ref )
{
   Context ctxt = in_ref;
   Menu menu = ctxt.GetSource();

   CStatus st;
   MenuItem item;
   menu.AddCallbackItem(L"Import Tup...", L"OnTupImportMenu", item);

   return CStatus::OK;	
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnTupImportMenu( XSI::CRef& in_ref )
{	
   Application app;
   CStatus st(CStatus::OK);
   Property prop = app.GetActiveSceneRoot().AddProperty( L"ImportTupOptions" ) ;
   bool ret = InspectObj(L"ImportTupOptions",CValue(),L"Tup Import (example)",(long)4,true);
   if (!ret)
   {
      Parameter importFileParam = prop.GetParameters().GetItem( L"ImportFile" );
      CString strFilePath = importFileParam.GetValue();

      if ( strFilePath.Length() <= 0)
      {
         DeleteObj( L"ImportTupOptions" );
         return CStatus::InvalidArgument;
      }

      //Launch the import command
      CImportTup importTup;
      st = importTup.ImportFile(strFilePath);

   }

   DeleteObj( L"ImportTupOptions" );
   return st;	
}

#ifdef unix
extern "C"
#endif
CStatus ExportTupOptions_Define( const CRef & in_Ctx )
{
   // Here is where we add all the parameters to the 
   // Custom Property.  This will be called each time 
   // an new instance of the Custom Property is called.
   // It is not called when an persisted Custom Property is loaded.

   //
   // Spdl equivalent of what is defined here.
   //
   /*
   Parameter "Name"
   {
   Name	= "Name";
   Description	= "Name";
   Guid	= "{2B91C031-677A-11D1-B0CF-00A024C79287}";
   Type	= VT_LPWSTR;
   Caps	= Persistable;
   Class	= MetaData;
   UIType	= "String";
   }

   Parameter "Custom parameter instance ID"
   {
   Name	= "Custom parameter instance ID";
   Description	= "Custom parameter instance ID";
   Guid	= "{29347CF0-1EA1-11D3-A612-00A0C9EC5EF7}";
   Type	= VT_CLSID;
   Caps	= Persistable, UnInspectable;
   Class	= Unknown;
   }

   Parameter "ExportScene"
   {
   Name	= "Export Entire Scene";
   Guid	= "{B285B7D3-236D-4194-84DB-4A44B52F9FED}";
   Type	= VT_BOOL;
   Caps	= Persistable;
   Class	= Unknown;
   UIType	= "Check";
   Value = True ;
   }

   Parameter "ExportObject"
   {
   Name	= "ExportObject";

   Guid	= "{629A70BA-C25F-4013-8452-0154737FCF78}";
   Type	= VT_BSTR;
   Caps	= Persistable;
   Class	= Unknown;
   UIType	= "String";
   }

   Parameter "ExportFile"
   {
   Name	= "ExportFile";

   Guid	= "{F8592492-88D9-445F-8224-44F35C7E1701}";
   Type	= VT_BSTR;
   Caps	= Persistable;
   Class	= Unknown;
   UIType	= "dscontrol"
   {
   class = "Edit";
   Caption = ";file=1;initialdir=project;filefilter=DotXSI format (*.tup)|*.xsi|All Files (*.*)|*.*||";
   }
   }
   */
   Application app ;
   CustomProperty prop = Context(in_Ctx).GetSource() ;
   Parameter param ;

   // Default capabilities for most of these parameters
   int caps = siPersistable  ;
   CValue dft ;	// Used for arguments we don't want to set

   prop.AddParameter(	L"ExportScene",CValue::siBool, caps, 
      L"Export Entire Scene", L"", 
      dft, param ) ;	

   prop.AddParameter(	L"ExportObject",CValue::siString, caps, 
      L"ExportObject", L"", 
      dft, param ) ;	

   prop.AddParameter(	L"ExportFile",CValue::siString, caps, 
      L"ExportFile", L"", 
      dft, param ) ;	

   return CStatus::OK;	
}

#ifdef unix
extern "C"
#endif
CStatus ExportTupOptions_DefineLayout( const CRef & in_Ctx )
{
   // XSI will call this to define the visual appearance of the CustomProperty
   // The layout is shared between all instances of the CustomProperty
   // and is cached.  You can force the code to re-execute by using the 
   // XSIUtils.Refresh feature.

   //
   // Spdl equivalent 
   //
   /*
   Layout "Default"
   {
   "ExportScene";

   Group "Object to Export"
   {
   row
   {
   "ExportObject", NoLabel, 80%;
   Button Pick, 1%; 
   }
   }


   Group "Export File"
   {
   "ExportFile", NoLabel;
   }
   }
   */

   PPGLayout oLayout = Context( in_Ctx ).GetSource() ;
   PPGItem item ;

   oLayout.Clear() ;


   item = oLayout.AddItem( L"ExportScene" ) ;

   oLayout.AddGroup(L"Object to Export") ;
   oLayout.AddRow() ;

   item = oLayout.AddItem( L"ExportObject" ) ;
   item.PutAttribute( siUINoLabel, true ) ;
   item.PutWidthPercentage( 80l ) ;

   item = oLayout.AddButton( L"Pick", L"Pick" ) ;
   item.PutWidthPercentage( 1l ) ;

   oLayout.EndRow() ;
   oLayout.EndGroup() ;


   oLayout.AddGroup( L"Export File" ) ;
   item = oLayout.AddItem( L"ExportFile",L"Export File",siControlFilePath ) ;
   item.PutAttribute( siUINoLabel, true ) ;
   item.PutAttribute( siUIFileFilter, L"Tup format  (*.tup)|*.tup|All Files (*.*)|*.*||" ) ;


   oLayout.EndGroup() ;


   return CStatus::OK;	
}

#ifdef unix
extern "C"
#endif
CStatus ExportTupOptions_PPGEvent( const CRef& io_Ctx )
{
   // This callback is called when events happen in the user interface
   // This is where you implement the "logic" code.

   Application app ;

   PPGEventContext ctx( io_Ctx ) ;

   PPGEventContext::PPGEvent eventID = ctx.GetEventID() ;

   CustomProperty prop = ctx.GetSource() ;	
   Parameter exportObjectParam = prop.GetParameters().GetItem( L"ExportObject" ) ;

   //
   // Spdl equivalent
   //
   /*
   sub OnInit
   if ( Selection.Count > 0 ) then
   Pset.ExportObject.Value = Selection(0)
   end if 

   PSet.ExportObject.Enable(false)
   end sub
   */
   if ( eventID == PPGEventContext::siOnInit )
   {

      Selection sel(app.GetSelection());
      if ( sel.GetCount() > 0 )
         prop.PutParameterValue( L"ExportObject", SIObject(sel[0]).GetName() ) ; 

      exportObjectParam.PutCapabilityFlag( siReadOnly, true );
   }

   //
   // Spdl equivalent
   //
   /*
   sub Pick_OnClicked
   'If they want to pick an object then automatically
   'disable exporting the entire scene
   PSet.ExportScene.Value = false 
   Pset.ExportObject.Enable( true )
   PSet.ExportObject.Value = GetUserSelectedObject
   end sub

   */
   else if ( eventID == PPGEventContext::siButtonClicked )
   {
      CValue buttonPressed = ctx.GetAttribute( L"Button" );	
      if ( buttonPressed.GetAsText() == L"Pick" )
      {
         prop.PutParameterValue( L"ExportScene", false ); 
         exportObjectParam.PutCapabilityFlag( siReadOnly, false );

         prop.PutParameterValue( L"ExportObject", GetUserSelectedObject() ) ; 

      }
   }
   //
   // Spdl equivalent
   //
   /*
   sub ExportScene_OnChanged
   Pset.ExportObject.Enable( NOT Pset.ExportScene.Value )	
   end sub
   */
   else if ( eventID == PPGEventContext::siParameterChange )
   {
      Parameter changed = ctx.GetSource() ;	

      CustomProperty prop = changed.GetParent() ;	

      CString   paramName = changed.GetScriptName() ; 

      if ( paramName == L"ExportScene" )
      {
         exportObjectParam.PutCapabilityFlag( siReadOnly, prop.GetParameterValue( L"ExportScene") );
      }

   }


   return CStatus::OK;	

}

int compare (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}


CString GetUserSelectedObject()
{
   //
   // Spdl equivalent
   //
   /*
   function GetUserSelectedObject		
   dim oDial, oItems, index

   'Simply find all the X3DObjects recursively under 
   'the scene root, but many other custom filtering 
   'of objects in the scene could be implemented.
   set oItems = ActiveSceneRoot.FindChildren

   redim strItems( oItems.Count - 1 )

   for i = 0 to oItems.Count - 1
   strItems( i ) = oItems.Item(i).Name
   next

   'Alphabetical list can be easiest when there are many items
   xsiutils.quicksort( strItems )

   set oDial = CreateObject( "XSIDial.XSIDialog" )

   index = oDial.Combo( "Select Item", strItems )

   if ( index <> -1 ) then
   GetUserSelectedObject = strItems( index )
   else
   GetUserSelectedObject = ""
   end if
   end function
   */
   Application app;
   Model root(app.GetActiveSceneRoot());
   CStringArray emptyArray;
   CRefArray cRefArray = root.FindChildren( L"", L"", emptyArray, true );

   CStringArray nameArray(cRefArray.GetCount());
   for ( long i=0; i < cRefArray.GetCount(); i++ )
   {
      nameArray[i] = SIObject(cRefArray[i]).GetName();
   }
   //todo qsort the nameArray

   // Using the COMAPIHandler for creating a "XSIDial.XSIDialog"
   CComAPIHandler xsidialog;
   xsidialog.CreateInstance( L"XSIDial.XSIDialog");
   CValue index;
   CValueArray args(cRefArray.GetCount());
   for (long y=0; y < cRefArray.GetCount(); y++)
      args[y]=nameArray[y];

   xsidialog.Call(L"Combo",index,L"Select Item",args );

   long ind = (long)index;
   return args[ind];
}

#ifdef unix
extern "C"
#endif
CStatus ImportTupOptions_Define( const CRef & in_Ctx )
{
   // Here is where we add all the parameters to the 
   // Custom Property.  This will be called each time 
   // an new instance of the Custom Property is called.
   // It is not called when an persisted Custom Property is loaded.

   Application app ;
   CustomProperty prop = Context(in_Ctx).GetSource() ;
   Parameter param ;

   // Default capabilities for most of these parameters
   int caps = siPersistable  ;
   CValue dft ;	// Used for arguments we don't want to set

   prop.AddParameter(	L"ImportFile",CValue::siString, caps, 
      L"ImportFile", L"", 
      dft, param ) ;	

   return CStatus::OK;	

}

#ifdef unix
extern "C"
#endif
CStatus ImportTupOptions_DefineLayout( const CRef & in_Ctx )
{
   // XSI will call this to define the visual appearance of the CustomProperty
   // The layout is shared between all instances of the CustomProperty
   // and is cached.  You can force the code to re-execute by using the 
   // XSIUtils.Refresh feature.

   PPGLayout oLayout = Context( in_Ctx ).GetSource() ;
   PPGItem item ;

   oLayout.Clear() ;

   item = oLayout.AddItem( L"Name" ) ;

   oLayout.AddGroup( L"Import File" ) ;
   item = oLayout.AddItem( L"ImportFile",L"Import File",siControlFilePath ) ;
   item.PutAttribute( siUINoLabel, true ) ;
   item.PutAttribute( siUIFileFilter, L"Tup format  (*.tup)|*.tup|All Files (*.*)|*.*||" ) ;
   item.PutAttribute( siUIFileMustExist, true ) ;
   item.PutAttribute( siUIOpenFile, true ) ;


   oLayout.EndGroup() ;


   return CStatus::OK;	

}


CValue InspectObj( const CString& in_inputobjs, const CString& in_keywords, const CString& in_title, const CValue& /*number*/ in_mode, bool in_throw )
{
   Application app;
   CValueArray args(5);
   CValue retval;
   long i(0);

   args[i++]= in_inputobjs;
   args[i++]= in_keywords;
   args[i++]= in_title;
   args[i++]= in_mode;
   args[i++]= in_throw;

   CStatus st = app.ExecuteCommand( L"InspectObj", args, retval );

   return retval;
}

void DeleteObj( const CValue& in_inputobj )
{
   Application app;
   CValueArray args(1);
   CValue retval;
   long i(0);

   args[i++]= in_inputobj;

   CStatus st = app.ExecuteCommand( L"DeleteObj", args, retval );

   return;
}

