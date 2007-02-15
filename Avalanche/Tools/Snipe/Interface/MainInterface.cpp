#include "stdafx.h"
#include "maininterface.h"
#include "Project.h"
#include "PolyModel.h"
#include "PatchModel.h"
#include "BezierSplineModel.h"
#include "Shortcut.h"
#include "CommandPluginManager.h"
#include "CommandUIPluginManager.h"
#include "ManipulatorPluginManager.h"
#include "NavigatorPluginManager.h"
#include "ModelTranslatorPluginManager.h"
#include "SceneTranslatorPluginManager.h"
#include "MotionTranslatorPluginManager.h"
#include "MotionSetTranslatorPluginManager.h"
#include "SnipeNavigator.h"
#include "GUI/MainFrm.h"
#include "Resource.h"
#include "../Utility/FileStream.h"
#include "GUI/BaseDoc.h"

CGLBaseView *MainInterface::m_pCurrentView;

RGBAFloat g_selectedcolor( 0.0f, 1.0f, 0.0f, 1.0f );
RGBAFloat g_selectedcolorlocked( 0.5f, 0.65f, 0.5f, 1.0f );
RGBAFloat g_focuscolor( 1.0f, 1.0f, 0.0f, 1.0f );
RGBAFloat g_focuscolorlocked( 0.65f, 0.65f, 0.5f, 1.0f );
RGBAFloat g_mirrorcolor( 1.0f, 0.0f, 1.0f, 1.0f );
RGBAFloat g_mirrorcolorlocked( 0.65f, 0.5f, 0.65f, 1.0f );

MainInterface::MainInterface()
{
   m_pProject = NULL;
   m_pCurrentWorkingObject = NULL;
   m_pCurrentCommandObject = NULL;
   m_pCurrentSelectionList = NULL;
   m_pCurrentCommandList = NULL;
   m_pCurrentNavigator = NULL;
}

MainInterface::~MainInterface()
{
}

void MainInterface::Init()
{
   m_pluginmanagers.SetSize(NUM_PLUGINTYPES);

   for (int i=0; i < NUM_PLUGINTYPES; i++ ) {
      switch (i) {
         case COMMAND_PLUGIN:
            m_pluginmanagers.SetAt(i, SNEW CommandPluginManager);
            break;
         case COMMANDUI_PLUGIN:
            m_pluginmanagers.SetAt(i, SNEW CommandUIPluginManager);
            break;
         case MANIPULATOR_PLUGIN:
            m_pluginmanagers.SetAt(i, SNEW ManipulatorPluginManager);
            break;
         case NAVIGATOR_PLUGIN:
            m_pluginmanagers.SetAt(i, SNEW NavigatorPluginManager);
            break;
         case MODELTRANSLATOR_PLUGIN:
            m_pluginmanagers.SetAt(i, SNEW ModelTranslatorPluginManager);
            break;
         case SCENETRANSLATOR_PLUGIN:
            m_pluginmanagers.SetAt(i, SNEW SceneTranslatorPluginManager);
            break;
         case MOTIONTRANSLATOR_PLUGIN:
            m_pluginmanagers.SetAt(i, SNEW MotionTranslatorPluginManager);
            break;
         case MOTIONSETTRANSLATOR_PLUGIN:
            m_pluginmanagers.SetAt(i, SNEW MotionSetTranslatorPluginManager);
            break;
         default:
            ASSERT(FALSE);
            break;
      }
   }

   m_pluginmanagers.Init(); // This will Load the plugins

   PluginList *pList = GetManipulatorPluginManager()->GetPluginList();
   for (POSITION pos = pList->GetHeadPosition(); pos; )
      m_manipulatormanager.AddSnipeManipulator((ManipulatorPlugin *)pList->GetNext(pos));

   pList = GetNavigatorPluginManager()->GetPluginList();
   for (POSITION pos = pList->GetHeadPosition(); pos; )
      m_navigatormanager.AddSnipeNavigator((NavigatorPlugin *)pList->GetNext(pos));

   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glNavigatorPropertyBar.FillObjectCombo();
   if (!m_navigatormanager.m_alllist.IsEmpty())
      SetCurrentNavigator( (SnipeNavigator *)m_navigatormanager.m_alllist.GetHead() ); // Set to first Navigator for now

   m_commanduimanager.Init(); // must be after m_pluginmanagers.Init();
}

void MainInterface::SetCurrentNavigator( SnipeNavigator *pNavigator )
{
   m_pCurrentNavigator = pNavigator;

   for (POSITION docpos=GetApp().GetDocList().GetHeadPosition(); docpos; ) {
      CBaseDoc *pDoc = GetApp().GetDocList().GetNext( docpos );
      for (POSITION viewpos = pDoc->m_viewlist.GetHeadPosition(); viewpos; ) {
         CGLBaseView *pView = pDoc->m_viewlist.GetNext( viewpos );
         pView->SetNavigatorInstance( pNavigator );
      }
   }
}

void MainInterface::SetProject( Project *pProject )
{ 
   m_pProject = pProject;
}

Project *MainInterface::CreateProject()
{
   if (!m_pProject)
      m_pProject = Project::Create();
   return m_pProject;
}

float GetFPS()
{
   if (GetMainInterface()->GetProject())
      return GetMainInterface()->GetProject()->m_fps;
   else
      return 30.0f;
}

void MainInterface::SetCurrentView( CGLBaseView *pView )
{
   m_pCurrentView = pView;
   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glViewPropertyBar.SetObject( pView );
   
   if (pView)
   {
      GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glNavigatorPropertyBar.SetObject( pView->GetNavigatorInstance() );
      GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glManipulatorPropertyBar.SetObject( pView->GetManipulatorInstance() );
   }
   else
   {
      GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glNavigatorPropertyBar.SetObject( NULL );
      GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glPropertyFrame.m_glManipulatorPropertyBar.SetObject( NULL );
   }
}

void MainInterface::SetCurrentCommandObject( SnipeObject *pObject )
{ 
   if (m_pCurrentCommandObject == pObject)
      return;

   ASSERT( !pObject || pObject->IsCommandObject() );
   m_pCurrentCommandObject = pObject;

   if (pObject)
   {
      m_pCurrentSelectionList = pObject->GetSelectionList(); 
      m_pCurrentCommandList = pObject->GetCommandList();
   }
   else
   {
      m_pCurrentSelectionList = NULL; 
      m_pCurrentCommandList = NULL;
   }
}

BOOL MainInterface::DoCommand(const String &strCommandName, const String &strParameters, StringList &strResults, BOOL bTempCommand/*=FALSE*/)
{
   CommandPlugin *pCommandPlugin = GetCommandPluginManager()->FindCommandByCLIName( "", strCommandName );
   if (pCommandPlugin && GetCurrentCommandObject() && GetCurrentCommandObject()->GetCommandList())
      return GetCurrentCommandObject()->GetCommandList()->Do( pCommandPlugin, strParameters, strResults, bTempCommand );
   else
      return FALSE;
}

BOOL MainInterface::DoCommand(CommandPlugin *pCommandPlugin, const String &strParameters, StringList &strResults, BOOL bTempCommand/*=FALSE*/)
{
   if (GetCurrentCommandObject() && GetCurrentCommandObject()->GetCommandList())
      return GetCurrentCommandObject()->GetCommandList()->Do( pCommandPlugin, strParameters, strResults, bTempCommand );
   else
      return FALSE;
}

BOOL MainInterface::UndoCommand()
{
   CommandList *pList = NULL;

   if (GetCurrentCommandObject())
      pList = GetCurrentCommandObject()->GetCommandList();

   if (pList && pList->HasUndo())
   {
      pList->Undo();
      return TRUE;
   }

   return FALSE;
}

BOOL MainInterface::KillLastCommand()
{
   CommandList *pList = NULL;

   if (GetCurrentCommandObject())
      pList = GetCurrentCommandObject()->GetCommandList();

   if (pList && pList->HasUndo())
   {
      BOOL bDisplay = GetApp().IsDisplayCommandInOutput();
      GetApp().SetDisplayCommandInOutput(FALSE);
      pList->Undo();
      pList->RemoveRedo();
      GetApp().SetDisplayCommandInOutput(bDisplay);
      if (bDisplay)
      {
         String strDesc;
         GetMainInterface()->ObjectToDescription( GetCurrentCommandObject(), strDesc, FALSE );
         g_outputWindow << strDesc << "<:>" << "KillLastCommand\n";
      }

      return TRUE;
   }

   return FALSE;
}

BOOL MainInterface::RedoCommand()
{
   CommandList *pList = NULL;

   if (GetCurrentCommandObject())
      pList = GetCurrentCommandObject()->GetCommandList();

   if (pList && pList->HasRedo())
   {
      pList->Redo();
      return TRUE;
   }

   return FALSE;
}

BOOL MainInterface::UndoTempCommand()
{
   CommandList *pList = NULL;

   if (GetCurrentCommandObject())
      pList = GetCurrentCommandObject()->GetCommandList();

   if (pList)
      return pList->UndoTempCommand();

   return FALSE;
}

BOOL MainInterface::CommitTempCommand()
{
   CommandList *pList = NULL;

   if (GetCurrentCommandObject())
      pList = GetCurrentCommandObject()->GetCommandList();

   if (pList)
      return pList->CommitTempCommand();

   return FALSE;
}

// Use | for hier, . for properties and components, ( ) for component ID's and : for ranges
// Examples
// Model1                  - This will Add Model1 to CurrentWorkingObject
// |Project|Models|Model1  - Uses as is
// vertex(1,5,6)           - Uses the CurrentWorkingObjects vertex list and returns the first element if DescriptionToObject is called or all three vertices if DescriptionToObjects is called
// Model1.vertex(1,5:10)   - This will Add Model1 to CurrentWorkingObject and returns the the vertices

SnipeObject *MainInterface::DescriptionToObject(const String &strObjectConst, String *pError/*=NULL*/)
{
   SnipeObject *pCurrentObject;
   String strObject;

   if (strObjectConst[0]=='|')
   {
      pCurrentObject = m_pProject;
      strObject = strObjectConst.Right(strObjectConst.Length()-1);
   }
   else
   {
      strObject = strObjectConst;
      pCurrentObject = m_pCurrentWorkingObject;

      while (strObject.Left(3) == "..|")
      {
         if (!pCurrentObject)
            break;
         pCurrentObject = pCurrentObject->GetParent();
         strObject = strObject.Right( strObject.Length() - 3 );
      }
   }

   if (!pCurrentObject)
   {
      if (pError)
         *pError = "No project";
      return NULL;
   }

   StringTokenList *pList = strObject.Split("|.", 2);
   char cPrevToken = '\0';
   for (POSITION pos = pList->GetHeadPosition(); pos; )
   {
      StringToken token = pList->GetNext( pos );
      String &strCurrentObject = token.m_strValue;

      if (pCurrentObject == NULL)
      {
         if (strCurrentObject == m_pProject->GetMatchName())
            pCurrentObject = m_pProject;
      }
      else if (strCurrentObject.Find( '(' ) != -1)
         pCurrentObject = pCurrentObject->FindComponent(strCurrentObject, pError);
      else // Either a property or an object
      {
         if (cPrevToken=='.') // must be a property
            pCurrentObject = pCurrentObject->FindProperty( strCurrentObject );
         else // Could still possible be a property if the property belongs to the command object, so lets look for the object first and if it doesn't exist lets then look at the properties.
         {
            SnipeObject *pOriginalCurrentObject = pCurrentObject;
            pCurrentObject = pOriginalCurrentObject->FindObject(strCurrentObject);
            if (pCurrentObject == NULL)
               pCurrentObject = pOriginalCurrentObject->FindProperty( strCurrentObject );
         }
      }

      if (pCurrentObject == NULL)
      {
         if (pError)
            *pError = (String)"Can't locate " + strCurrentObject + "\n";
         delete pList;
         return NULL;
      }
      cPrevToken = token.m_cToken; 
   }

   delete pList;

   return pCurrentObject;
}

// Does not handle relative ..| yet
SnipeObjectList *MainInterface::DescriptionToObjects(const String &strObjects, StringList &strOutErrorList)
{
   strOutErrorList.IsEmpty();

   SnipeObject *pCurrentObject;
   MemoryStream ms;
   ms.Open( strObjects.Get(), strObjects.Length(), BaseStream::BS_INPUT );

   SnipeObjectList *pObjectList = SNEW SnipeObjectList;

   String strOption;
   while (!ms.Eof())
   {
      if (!ms.ParseTokenWhiteSpace())
         break;

      String strObject;
      ms.GetToken(strObject);

      if (strObject[0]=='|' || m_pCurrentWorkingObject == NULL)
      {
         pCurrentObject = NULL;
         strObject = strObject.Right(strObject.Length()-1);
      }
      else
         pCurrentObject = m_pCurrentWorkingObject;

      StringTokenList *pList = strObject.Split("|.", 2);

      BOOL bFoundComponent = FALSE;
      char cPrevToken = '\0';
      for (POSITION pos = pList->GetHeadPosition(); pos; )
      {
         StringToken token = pList->GetNext( pos );
         String &strCurrentObject = token.m_strValue;
         
         StringList strComponentErrorList;
         if (strCurrentObject.Find( '(' ) != -1 && pCurrentObject->FindComponents(pObjectList, strCurrentObject, strComponentErrorList))
            bFoundComponent = TRUE;

         if (bFoundComponent)
            break;

         if (pCurrentObject == NULL)
         {
            if (strCurrentObject == m_pProject->GetMatchName())
               pCurrentObject = m_pProject;
         }
         else // Either a property or an object
         {
            if (cPrevToken=='.') // must be a property
               pCurrentObject = pCurrentObject->FindProperty( strCurrentObject );
            else // Could still possible be a property if the property belongs to the command object, so lets look for the object first and if it doesn't exist lets then look at the properties.
            {
               SnipeObject *pOriginalCurrentObject = pCurrentObject;
               pCurrentObject = pOriginalCurrentObject->FindObject(strCurrentObject);
               if (pCurrentObject == NULL)
                  pCurrentObject = pOriginalCurrentObject->FindProperty( strCurrentObject );
            }
         }

         if (pCurrentObject == NULL)
         {
            if (!strComponentErrorList.IsEmpty())
            {
               for (POSITION pos = strComponentErrorList.GetHeadPosition(); pos; )
                  strOutErrorList.AddTail( strComponentErrorList.GetNext(pos) );
               break;
            }
            else
            {
               strOutErrorList.AddTail((String)"Can't locate " + strCurrentObject);
               break;
            }
         }

         cPrevToken = token.m_cToken; 
      }

      delete pList;

      if (!bFoundComponent && pCurrentObject) // SnipeObject, not component
         pObjectList->AddTail(pCurrentObject);
   }
   return pObjectList;
}

static void ReverseOrderToRelativeObject(const SnipeObject *pRelativeObject, const SnipeObject *pObject, MemoryStream &ms)
{
   SnipeObject *pParent = pObject->GetParent();
   if (pParent && pParent != pRelativeObject )
      ReverseOrderToRelativeObject(pRelativeObject, pParent, ms);

   if (pParent == NULL || pParent != pRelativeObject)
      ms << '|';

   String strMatchName = pObject->GetMatchName();
   int nLength = strMatchName.Length();
   for (int i=0; i<nLength; i++)
   {
      if (strMatchName[i] == ' ' || strMatchName[i] == '.')
         break;
   }
   if (i<nLength) // found a space or period.  wrap the description in quotes
      ms << (String)'"' + pObject->GetMatchName() + '"';
   else
      ms << pObject->GetMatchName();
}

static void ReverseOrderProperties( Property *pProperty, MemoryStream &ms)
{
   SnipeObject *pParent = pProperty->GetParent();
   if (pParent && pParent != pProperty->GetObject())
      ReverseOrderProperties( (Property *)pParent, ms);

   if (pParent != pProperty->GetObject())
      ms << '.';

   String strMatchName = pProperty->GetMatchName();
   int nLength = strMatchName.Length();
   for (int i=0; i<nLength; i++)
   {
      if (strMatchName[i] == ' ' || strMatchName[i] == '.')
         break;
   }
   if (i<nLength) // found a space or period.  wrap the description in quotes
      ms << (String)'"' + strMatchName + '"';
   else
      ms << strMatchName;
}

BOOL MainInterface::ObjectToDescription( const SnipeObject *pMatchObject, String &strOutDescription, BOOL bRelativeToCurrentWorkingObject/*=TRUE*/ )
{
   MemoryStream ms;
   ms.Open(BaseStream::BS_OUTPUT); // Faster than using strings

   strOutDescription.Empty();

   if (!ObjectToDescription( pMatchObject, ms, bRelativeToCurrentWorkingObject))
      return FALSE;

   if (ms.GetLength())
   {
      ms << '\0';
      strOutDescription = (char *)ms.GetBuffer();
   }

   return TRUE;
}

BOOL MainInterface::ObjectToDescription( const SnipeObject *pMatchObject, MemoryStream &ms, BOOL bRelativeToCurrentWorkingObject/*=TRUE*/ )
{
   if (!pMatchObject)
      return TRUE;

   const SnipeObject *pObject;
   Component *pComponent = NULL;
   Property *pProperty = NULL;

   if (pMatchObject->IsProperty())
   {
      pProperty = (Property *)pMatchObject;
      pMatchObject = pProperty->GetObject();
   }

   if (pMatchObject->IsComponent())
   {
      pComponent = (Component *)pMatchObject;
      pObject = pComponent->GetOwner();
   }
   else
      pObject = pMatchObject;

   if (!bRelativeToCurrentWorkingObject || pObject != m_pCurrentWorkingObject)
   {
      if (bRelativeToCurrentWorkingObject && m_pCurrentWorkingObject!=NULL)
      {
         if (pObject == m_pCurrentWorkingObject || pObject->IsDesendantOf(m_pCurrentWorkingObject)) // Am I a descendant of pCurrentWorkingObject
            ReverseOrderToRelativeObject(m_pCurrentWorkingObject, pObject, ms);
         else // Cousin/Sibling
         {
            SnipeObject *pCommonParent = m_pCurrentWorkingObject;
            while (pCommonParent && !pObject->IsDesendantOf(pCommonParent))
               pCommonParent = pCommonParent->GetParent();
            if (pCommonParent==NULL)
               return FALSE;
            SnipeObject *pParent = m_pCurrentWorkingObject;
            while (pParent != pCommonParent)
            {
               ms << "..|";
               pParent = pParent->GetParent();
            }
            ReverseOrderToRelativeObject(pCommonParent, pObject, ms);
         }
      }
      else
         ReverseOrderToRelativeObject(NULL, pObject, ms);

      if (pComponent || pProperty)
         ms << ".";
   }

   if (pComponent)
   {
      ms << pComponent->GetCommandLineName() << '(' << (ULONG)pComponent->GetID() << ')'; 
      if (pProperty)
         ms << ".";
   }

   if (pProperty)
      ReverseOrderProperties( pProperty, ms );

   return TRUE;
}

//class ComponentList : public CList<Component *>
//{
//public:
//   String m_strCommandLineName;
//  
//   ComponentList(const String &strCommandLineName)
//   {
//      m_strCommandLineName = strCommandLineName;
//   }
//};

////////////////////////////////////////////////////
// ComponentArray
class ComponentArray : public CArray<Component *>
{
public:
   String m_strCommandLineName;

   ComponentArray( const String &strCommandLineName )
   {
      SetSize( 0, 500 );
      m_strCommandLineName = strCommandLineName;
   }

   void Sort() 
   {
      Sort( 0, GetSize()-1 );
   }

private:
   void Sort( int nLow, int nHigh );
};

void ComponentArray::Sort( int nLow, int nHigh )
{
   int nUp, nDown, nBreak, nValueBreak;
   Component *pTemp;

   if (nLow < nHigh)
   {
      if (nHigh - nLow == 1)
      {
         if (GetAt(nLow)->GetID() > GetAt(nHigh)->GetID())
         {
            pTemp = GetAt(nLow);
            SetAt(nLow, GetAt(nHigh));
            SetAt(nHigh, pTemp);
         }
      }
      else
      {
         nBreak = (nLow+nHigh)/2;
         nValueBreak = GetAt(nBreak)->GetID();
         pTemp = GetAt(nBreak);
         SetAt(nBreak, GetAt(nHigh));
         SetAt(nHigh, pTemp);
         do
         {
            nUp = nLow;
            nDown = nHigh;
            while (nUp < nDown && (int)GetAt(nUp)->GetID() <= nValueBreak)
               nUp++;
            while(nDown > nUp && (int)GetAt(nDown)->GetID() >= nValueBreak)
               nDown--;
            if (nUp < nDown)
            {
               pTemp = GetAt(nUp);
               SetAt(nUp, GetAt(nDown));
               SetAt(nDown, pTemp);
            }
         } while (nUp < nDown);

         pTemp = GetAt(nUp);
         SetAt(nUp, GetAt(nHigh));
         SetAt(nHigh, pTemp);
         if (nUp - nLow < nHigh - nUp)
         {
            Sort( nLow, nUp - 1 );
            Sort( nUp + 1, nHigh );
         }
         else
         {
            Sort( nUp + 1, nHigh );
            Sort( nLow, nUp - 1 );
         }
      }
   }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
SnipeObject *MainInterface::CreateObject( SnipeObjectType nSOT, SnipeObject::DeleteInfo **pDeleteInfo/*=NULL*/)
{
   SnipeObjectInfo *pSnipeObjectInfo = SnipeObjectInfo::GetBySOT( nSOT );
   if (!pSnipeObjectInfo)
      return NULL;

   SnipeObject *pSnipeObject = pSnipeObjectInfo->CreateObject();
   if (pSnipeObject && pDeleteInfo)
      *pDeleteInfo = pSnipeObject->AllocDeleteInfo();

   return pSnipeObject;
}


typedef CMap <SnipeObjectType, SnipeObjectType, ComponentArray *, ComponentArray *> ComponentArrayMap;

BOOL MainInterface::ObjectsToDescription( const SnipeObjectList *pObjectList, String &strOutDescription, BOOL bRelativeToCurrentWorkingObject/*=TRUE*/ )
{
   strOutDescription.Empty();

   MemoryStream ms;
   ms.Open(BaseStream::BS_OUTPUT); // Faster than using strings

   if (!ObjectsToDescription( pObjectList, ms, bRelativeToCurrentWorkingObject))
      return FALSE;

   if (ms.GetLength()) 
   {
      ms << '\0';
      strOutDescription = (char *)ms.GetBuffer();
   }
   return TRUE;
}

BOOL MainInterface::ObjectsToDescription( const SnipeObjectList *pObjectList, MemoryStream &ms, BOOL bRelativeToCurrentWorkingObject/*=TRUE*/ )
{
   // Separate our pObjectList by owners, most likely they all have the same owner
   CMap < SnipeObject *, SnipeObject *, ComponentArrayMap *, ComponentArrayMap *> objectmap;
   objectmap.InitHashTable(1019); // different SnipeObjects not components

   ComponentArray *pComponentArray;
   ComponentArrayMap *pComponentMap;
   Component   *pComponent;
   SnipeObjectType nSOT;
   BOOL bError = TRUE;
   SnipeObject *pMatchObject;

   for (POSITION pos = pObjectList->GetHeadPosition(); pos; )
   { 
      pMatchObject = pObjectList->GetNext( pos );
      if (!pMatchObject)
         continue;

      if (pMatchObject->IsComponent()) {
         pComponent = (Component *)pMatchObject;
         pMatchObject = (HierObject *)pComponent->GetOwner();
      }
      else
         pComponent = NULL;

      if (!objectmap.Lookup(pMatchObject, pComponentMap))
      {
         pComponentMap = SNEW ComponentArrayMap;
         pComponentMap->InitHashTable(13);
         objectmap.SetAt(pMatchObject, pComponentMap); 
      }
      ASSERT ( pComponentMap );

      if ( pComponent )
      {
         nSOT = pComponent->GetSOT();
         if (!pComponentMap->Lookup( nSOT, pComponentArray) )
         {
            pComponentArray = SNEW ComponentArray(pComponent->GetCommandLineName());
            pComponentMap->SetAt( nSOT, pComponentArray );
         }
         UINT nID = pComponent->GetID();
         pComponentArray->Add( pComponent );
         //if (pComponentArray->IsEmpty() || (nID < pComponentArray->GetHead()->GetID()))
         //   pComponentArray->AddHead( pComponent );
         //else
         //{
         //   POSITION pos;
         //   for (pos = pComponentArray->GetTailPosition(); pos; ) // Sort
         //   {
         //      UINT nCurID = (pComponentArray->GetAt(pos))->GetID();
         //      if (nID > nCurID)
         //         break;
         //      pComponentArray->GetPrev(pos);
         //   }
         //   ASSERT( pos );
         //   pComponentArray->InsertAfter(pos, pComponent);
         //}
      }
      else
         pComponentMap->SetAt( SOT_Unknown, NULL );  // Unknowns means that the select id for the object not a component
   }

   // Sort
   for (POSITION pos1 = objectmap.GetStartPosition(); pos1; )
   {
      objectmap.GetNextAssoc( pos1, pMatchObject, pComponentMap );
      if (pComponentMap) {
         for (POSITION pos2 = pComponentMap->GetStartPosition(); pos2; )
         {
            pComponentMap->GetNextAssoc( pos2, nSOT, pComponentArray );
            if (pComponentArray)
               pComponentArray->Sort();
         }
      }
   }

   // Lets write each owner and its components
   for (POSITION pos1 = objectmap.GetStartPosition(); pos1; ) 
   {
      objectmap.GetNextAssoc( pos1, pMatchObject, pComponentMap );
      for (POSITION pos2 = pComponentMap->GetStartPosition(); pos2; ) 
      {
         pComponentMap->GetNextAssoc( pos2, nSOT, pComponentArray );
         if (!bRelativeToCurrentWorkingObject || pMatchObject != m_pCurrentWorkingObject) {
            if (bRelativeToCurrentWorkingObject && m_pCurrentWorkingObject!=NULL) {
               // Am I a descendant of pCurrentWorkingObject
               if (pMatchObject == m_pCurrentWorkingObject || pMatchObject->IsDesendantOf(m_pCurrentWorkingObject))
                  ReverseOrderToRelativeObject(m_pCurrentWorkingObject, pMatchObject, ms);
               else { // Cousin or Sibling
                  SnipeObject *pCommonParent = m_pCurrentWorkingObject;
                  while (pCommonParent && !pMatchObject->IsDesendantOf(pCommonParent))
                     pCommonParent = pCommonParent->GetParent();
                  if (pCommonParent==NULL)
                     goto ERROR_EXIT;
                  SnipeObject *pParent = m_pCurrentWorkingObject;
                  while (pParent != pCommonParent) {
                     ms << "..|";
                     pParent = pParent->GetParent();
                  }
                  ReverseOrderToRelativeObject(pCommonParent, pMatchObject, ms);
               }
            }
            else
               ReverseOrderToRelativeObject(NULL, pMatchObject, ms);
            if (nSOT != SOT_Unknown) 
               ms << '.';
         }

         if (pComponentArray)
         {
            if (nSOT != SOT_Unknown) 
               ms << pComponentArray->m_strCommandLineName << '(' ;

            UINT nLastID = (UINT)-1;
            BOOL bInRange = FALSE;

            int nComponentCount = pComponentArray->GetCount();
            for (int nComponent=0; nComponent<nComponentCount; nComponent++)
            {
               pComponent = pComponentArray->GetAt( nComponent );
               UINT nID = pComponent->GetID();
               if (nLastID != (UINT) -1 && nID-1 == nLastID) {
                  bInRange = TRUE;
               }
               else {
                  if (bInRange) {
                     bInRange = FALSE;
                     ms << ':';
                     ms << nLastID;
                     ms << ',';
                  }
                  else {
                     if (nLastID != (UINT) -1)
                     ms << ',';
                  }
                  ms << nID;
               }
               nLastID = nID;
            }

            if (bInRange) {
               ms << ':';
               ms << nLastID;
            }

            if (nSOT != SOT_Unknown) 
               ms << ')';
         }

         if (pos2)
            ms << ' ';
      }
      if (pos1)
         ms << ' ';
   }

   bError = FALSE;

ERROR_EXIT:
   // Clean Up
   for (POSITION pos1 = objectmap.GetStartPosition(); pos1; ) {
      objectmap.GetNextAssoc( pos1, pMatchObject, pComponentMap );
      if (pComponentMap) {
         for (POSITION pos2 = pComponentMap->GetStartPosition(); pos2; ) {
           pComponentMap->GetNextAssoc( pos2, nSOT, pComponentArray );
           delete pComponentArray;
         }
         delete pComponentMap;
      }
   }

   return !bError;
}

void DoSelectCommand( SnipeObject *pCommandObject, SnipeObjectList *pSelectionList )
{
   ASSERT( pCommandObject );
   ASSERT( pSelectionList );

   static CommandPlugin *pSelectCommandPlugin = GetMainInterface()->GetCommandPluginManager()->FindCommandByCLIName( "SnipeBasicCommands", "Select" );

   if (pSelectCommandPlugin)
   {
      GetMainInterface()->SetCurrentWorkingObject( pCommandObject );
      String strObjectDescription;
      if (GetMainInterface()->ObjectsToDescription( pSelectionList, strObjectDescription ))
      {
         CommandList *pCommandList = pCommandObject->GetCommandList();
         GetMainInterface()->SetCurrentCommandObject( pCommandObject );
         StringList strResults;
         pCommandList->Do( pSelectCommandPlugin, strObjectDescription, strResults );
      }
   }
}
