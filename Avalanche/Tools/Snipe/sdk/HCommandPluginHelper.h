#pragma once
#include "HPluginHelper.h"
#include "HCommandPlugin.h"
#include "HCommandPluginManager.h"
#include "MemoryStream.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper class

class HSelectionList;

// delimets the string at spaces
class TokenList : public String
{
private:
   char *m_pEnd;
   char *m_pCurParam;
   BOOL  m_bDone;

public:
   TokenList( const String &strParameters ) :
      String( strParameters )
   {
      m_bDone = FALSE;
      m_pEnd = Get() + Length();
      m_pCurParam = NULL;

      Replace( ' ', '\0', FALSE );
   }

   char *GetNext()
   {
      if (m_bDone)
         return NULL;

      char *pPos;
      if (m_pCurParam)
      {
         pPos = m_pCurParam + 1;

         // buzz past all characters in current command
         for ( ; pPos<m_pEnd && *pPos!='\0'; pPos++)
            ;
      }
      else
         pPos = Get();

      // buzz past all nulls
      for ( ; pPos<m_pEnd && *pPos=='\0'; pPos++)
         ;

      if (pPos < m_pEnd)
         m_pCurParam = pPos;
      else
      {
         m_bDone = TRUE;
         m_pCurParam = NULL;
      }

      return m_pCurParam;
   }
};

/////////////////////////////////////////////////////////////////////////
// C++ Helper class for HCommandPlugin
class HCommandPluginManager;
class CommandInstance;
class HCommandPluginHelper : public HPluginHelper
{
public:
   HCommandPlugin *GetPlugin() { return (HCommandPlugin *)HPluginHelper::GetPlugin(); }

   // This is for executing a command from within another command only!!!!!!
   // This by passes the history lists
   CommandInstance *ExecuteInternalCommand( String strParameters, StringList &strResults );

   // Overrideables
   virtual catch_msg void Init(HCommandPluginManager *pManager) {} ; // to resolve HCommandPlugin *
   virtual String GetHelp() const { return ""; }

   // Pure
   virtual CommandInstance *CreateCommandInstance() = 0;
   virtual UINT GetBitmapID() const = 0;
   virtual const char *GetCLIName() const = 0; // Name used in scripts and commandline
};

class CommandInstance
{
private:
   HCommandPluginHelper *m_pCommandPluginHelper;
public:
   CommandInstance(HCommandPluginHelper *pCommandPlugin) { m_pCommandPluginHelper = pCommandPlugin; }
   virtual ~CommandInstance() {}

   // Operations
   HCommandPluginHelper *GetCommandPluginHelper() { return m_pCommandPluginHelper; }

   // Overridables
   virt_base virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist ) 
   {
      if (strcmp( szToken, "-?") == 0)
      {
         THROW_HELP_MSG( m_pCommandPluginHelper->GetHelp() );
      }
      return FALSE;
   }

   virt_base virtual void OnParse( const String &strParameters )
   {
      TokenList tokenlist( strParameters );

      while (TRUE)
      {
         char *szToken = tokenlist.GetNext();
         if (!szToken)
            break;

         if (!ParseToken( szToken, tokenlist ))
            THROW_FATAL_ERROR( GetCommandPluginHelper()->GetTitleName() + " : invalid option " + szToken + ".");
      }
   }

   virt_base virtual BOOL OnValidate( String &strOutError ) { return TRUE; } // Called after OnParse;
   virt_base virtual void OnRedo() 
   {
      StringList strResult;
      OnDo(strResult); 
   }
   virt_base virtual BOOL OnDo(StringList &strResults) { return TRUE; }
   virt_base virtual void OnUndo() {};

   // Pure
   virt_base virtual UndoType GetUndoType() = 0;
};

////////////////////////////////////////////////////////////////////////////
// Inlines

inline CommandInstance *HCommandPluginHelper::ExecuteInternalCommand( String strParameters, StringList &strResults )
{
   CommandInstance *pInstance = CreateCommandInstance();
   try
   {
      pInstance->OnParse( strParameters );
   }
   catch(const String strReason)
   {
      delete pInstance;
      DisplayWarning( strReason );
      return NULL;
   }
   catch(...)
   {
      delete pInstance;
      DisplayWarning("Unable to parse command instance parameters");
      return NULL;
   }

   try
   {
      String strError;
      if (!pInstance->OnValidate(strError))
      {
         DisplayWarning(strError);
         delete pInstance;
         return NULL;
      }

   }
   catch(const String strReason)
   {
      DisplayWarning( strReason );
      delete pInstance;
      return NULL;
   }
   catch(...)
   {
      DisplayWarning("Unable to validate command instance parameters");
      delete pInstance;
      return NULL;
   }
   try
   {
      if (!pInstance->OnDo(strResults))
      {
         DisplayWarning("Unable to execute command");
         delete pInstance;
         return NULL;
      }

   }
   catch(const String strReason)
   {
      DisplayWarning( strReason );
      delete pInstance;
      return NULL;
   }
   catch(...)
   {
      DisplayWarning("Unable to execute command");
      delete pInstance;
      return NULL;
   }

   return pInstance;
}

inline HCommandPluginHelper *FindCommandByClassNameHelper(HCommandPluginManager *pCommandManager, const char *szPluginName, const char *szClassName)
{
   ASSERT (pCommandManager);
   HCommandPlugin *pPlugin = pCommandManager->FindCommandByClassName(szPluginName, szClassName);
   if (pPlugin==NULL)
      return NULL;
   return (HCommandPluginHelper *)pPlugin->GetPluginData();
}

inline HCommandPluginHelper *FindCommandByCLINameHelper(HCommandPluginManager *pCommandManager, const char *szPluginName, const char *szCLIName)
{
   ASSERT (pCommandManager);
   HCommandPlugin *pPlugin = pCommandManager->FindCommandByCLIName(szPluginName, szCLIName);
   if (pPlugin==NULL)
      return NULL;
   return (HCommandPluginHelper *)pPlugin->GetPluginData();
}

inline void Init_HCommandPluginHelper(void *pHelper, HCommandPluginManager *pManager)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HCommandPluginHelper *)pHelper)->Init( pManager );
}

inline void Create_CommandInstance(void *pHelper, const String &strOutParameters, void *&pOutCommand )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CommandInstance *pCommand = NULL;

   try {
      pCommand = ((HCommandPluginHelper *)pHelper)->CreateCommandInstance();
      ASSERT(pCommand);
      pCommand->OnParse( strOutParameters );
   }
   catch( const String &strMsg ) {
      if (pCommand)
         delete pCommand;
      RETHROW_FATAL_ERROR( strMsg );
   }
   catch( ... ) {
      if (pCommand)
         delete pCommand;
      throw;
   }
   pOutCommand = (void *)pCommand;
} 

inline void Destroy_CommandInstance( void *pCommand ) 
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
   if (pCommand) 
      delete (CommandInstance *)pCommand; 
}

inline BOOL OnValidate_CommandInstance( void *pCommand, String &strOutError ) 
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
   return ((CommandInstance *)pCommand)->OnValidate( strOutError ); 
}

inline UndoType GetUndoType_CommandInstance( void *pCommand ) 
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
   return ((CommandInstance *)pCommand)->GetUndoType(); 
}

inline BOOL OnDo_CommandInstance( void *pCommand, StringList &strResults ) 
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
   return ((CommandInstance *)pCommand)->OnDo(strResults); 
}

inline void OnRedo_CommandInstance( void *pCommand ) 
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
   ((CommandInstance *)pCommand)->OnRedo(); 
}

inline void OnUndo_CommandInstance( void *pCommand ) 
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
   ((CommandInstance *)pCommand)->OnUndo(); 
}

inline HCommandPluginHelper *GetCommandPluginHelper( HCommandPlugin *pPlugin )
{
   ASSERT(pPlugin);
   return (HCommandPluginHelper *)pPlugin->GetPluginData();\
}

/////////////////////////////////////////////////////////////////////////
// Macro

#define ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, helper_class ) \
{ \
   try \
   { \
      HCommandPluginHelper *pHelper = new helper_class; \
      HCommandPlugin *pPlugin = pSnipePlugin->AddCommandPlugin(pHelper, Destroy_PluginData); \
      pHelper->SetPlugin(pPlugin); \
      pPlugin->SetClassName( pHelper->GetClassName() ); \
      pPlugin->SetTitleName( pHelper->GetTitleName() ); \
      pPlugin->SetCLIName( pHelper->GetCLIName() ); \
      pPlugin->SetBitmapID( pHelper->GetBitmapID() ); \
      pPlugin->SetInitCommandPluginCallback( Init_HCommandPluginHelper ); \
      pPlugin->SetCreateCommandInstanceCallback( Create_CommandInstance ); \
      pPlugin->SetDestroyCommandInstanceCallback( Destroy_CommandInstance ); \
      pPlugin->SetValidateCommandInstanceCallback( OnValidate_CommandInstance ); \
      pPlugin->SetSupportUndoCommandInstanceCallback( GetUndoType_CommandInstance ); \
      pPlugin->SetDoCommandInstanceCallback( OnDo_CommandInstance ); \
      pPlugin->SetRedoCommandInstanceCallback( OnRedo_CommandInstance ); \
      pPlugin->SetUndoCommandInstanceCallback( OnUndo_CommandInstance ); \
      String strHelp = pHelper->GetHelp();\
      if (!strHelp.IsEmpty()) \
         pPlugin->SetHelp(strHelp); \
   } \
   catch(...) \
   { \
      String strMsg( "Failed initializing Command: %1."); \
      strMsg.Replace( "%1", #helper_class ); \
      DisplayWarning( strMsg ); \
   } \
}

#define DECLARE_COMMAND_PLUGINHELPER(class_name, baseclass_name) \
   DECLARE_PLUGINHELPER(class_name, baseclass_name )

