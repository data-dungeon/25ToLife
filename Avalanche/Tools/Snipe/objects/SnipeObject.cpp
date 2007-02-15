#include "stdafx.h"
#include "SnipeObject.h"
#include "Interface/MainInterface.h" 
#include "Interface/CommandList.h"
#include "Component.h"
#include "GUI/ObjectExplorerBar.h"
#include "GUI/GLBaseView.h"
#include "GUI/Snipe.h"
#include "CommandPluginManager.h"
#include <../src/mfc/afximpl.h>
#include "../Utility/FileStream.h"
#include "Project.h"
#include "Selection.h"
#include "resource.h"

///////////////////////////////////////////////////////////// 
// SnipeObject::DeleteInfo

SnipeObject::DeleteInfo::DeleteInfo( SnipeObject *pSnipeObject )
{
   m_pMirrorDeleteInfo = NULL;
   m_pSnipeObject = NULL; // Must initialize to NULL before SetSnipeObjectCall because of the Dereference
   m_pPointerPropertyNodeHead = NULL;

   SetSnipeObject(pSnipeObject);

   //if (pSnipeObject)
   //{
   //   for (NotifyNode *pNotifyNode=pSnipeObject->m_pNotifyNodeHead; pNotifyNode; pNotifyNode=pNotifyNode->m_pNextNotifyNode)
   //   {
   //      SnipeObject *pObject = pNotifyNode->GetSnipeObject();
   //      if (pObject && pObject->IsProperty())
   //      {
   //         PointerProperty<SnipeObject *> *pProperty = (PointerProperty<SnipeObject *> *)pObject;
   //         ASSERT( pProperty->GetValue() == pSnipeObject );
   //         m_pPointerPropertyNodeHead = SNEW PointerPropertyNode( pProperty, m_pPointerPropertyNodeHead );
   //      }
   //   }
   //}
}

SnipeObject::DeleteInfo::~DeleteInfo()
{
   if (m_pMirrorDeleteInfo)
      delete m_pMirrorDeleteInfo;

   //for (PointerPropertyNode *pNode=m_pPointerPropertyNodeHead, *pNextNode; pNode; pNode=pNextNode)
   //{
   //   pNextNode = pNode->m_pNextNode;
   //   delete pNode;
   //}

   SetSnipeObject( NULL );
}

///////////////////////////////////////////////////////////// 
// SnipeObjectHeader

SnipeObjectHeader::SnipeObjectHeader( SnipeObject *pObject, BOOL bWriteHeader, BaseStream &bs ) :
 SnipeObjectHeaderBase(pObject, bWriteHeader, bs)
{
   if (m_bWriteHeader) 
   {
      *m_pBS << mar << '[' << m_pObject->GetClassName() << ']' << nl;
      m_nFilePos = m_pBS->GetPos();

      m_pBS->m_nMargin++;

      if (pObject->m_bUserNamed)
         bs << DataHeader( "Name", pObject->GetMatchName() ) << nl;
   }
}

SnipeObjectHeader::~SnipeObjectHeader()
{
   if (m_bWriteHeader) 
   {
      m_pBS->m_nMargin--;

      if (m_pBS->GetPos() != m_nFilePos)
         *m_pBS << mar << "[/" << m_pObject->GetClassName() << ']' << nl;
      else
      {
         m_pBS->Seek( -m_pBS->GetEOLSize()-1, BaseStream::ST_OFFSET );
         *m_pBS << "/]" << nl;
      }
   }
}

BOOL SnipeObjectHeader::IsHeader( MemoryStream &ms )
{
   ms.ReadMargin();
   return (ms.GetBuffer()[ms.GetPos()] == '[');
}

BaseStream &operator >> ( BaseStream &bs, SnipeObjectHeader &value )
{
   value.m_bBeginHeader = FALSE;
   value.m_bEndHeader = FALSE;

   bs.ReadMargin();

   char cTemp;
   bs >> cTemp; // '['
   ASSERT( cTemp == '[' );

   int nCount = bs.ParseToken(']');

   if (bs.m_tokenarray[0] == '/') // end header [/FOO]
   {
      value.m_bEndHeader = TRUE;
      value.m_strKey = &bs.m_tokenarray[1];
      return bs;
   }

   value.m_bBeginHeader = TRUE;

   if (bs.m_tokenarray[nCount-1] == '/') // begin and end header [FOO/]
   {
      bs.m_tokenarray[nCount-1] = '\0';
      value.m_bEndHeader = TRUE;
   }

   value.m_strKey = bs.m_tokenarray;

   return bs;
}

///////////////////////////////////////////////////////////// 

DataHeader::DataHeader( const String &strKey, const String &strValue )
{
   m_strKey = strKey;
   m_strValue = strValue;
}

DataHeader::DataHeader( BaseStream &bs, const String &strKey ) :
 HeaderBase( TRUE, bs)
{
   if (m_bWriteHeader) 
   {
      m_strKey = strKey;
      *m_pBS << mar << '{' << strKey << '}' << nl;
      m_nFilePos = m_pBS->GetPos();

      m_pBS->m_nMargin++; 
   }
}

DataHeader::~DataHeader()
{
   if (m_bWriteHeader) 
   {
      m_pBS->m_nMargin--; 

      if (m_pBS->GetPos() != m_nFilePos)
         *m_pBS << mar << "{/" << m_strKey << '}' << nl;
      else
      {
         m_pBS->Seek( -m_pBS->GetEOLSize()-1, BaseStream::ST_OFFSET );
         *m_pBS << "/}" << nl;
      }
   }
}

BOOL DataHeader::IsHeader( MemoryStream &ms )
{
   ms.ReadMargin();
   return (ms.GetBuffer()[ms.GetPos()] == '{');
}

BaseStream &operator << ( BaseStream &bs, const DataHeader &value )
{
   bs << mar << '{' << value.m_strKey << '=' << value.m_strValue << "/}";

   return bs;
}

BaseStream &operator >> ( BaseStream &bs, DataHeader &value )
{
   value.m_bBeginHeader = FALSE;
   value.m_bEndHeader = FALSE;

   bs.ReadMargin();

   char cTemp;
   bs >> cTemp; // '{'
   if (cTemp != '{')
      THROW_FATAL_ERROR("Failed reading DataHeader.");

   int nCount = bs.ParseToken('}');

   if (bs.m_tokenarray[0] == '/') // end header {/FOO}
   {
      value.m_bEndHeader = TRUE;
      value.m_strKey = &bs.m_tokenarray[1];
      value.m_strValue.Empty();
      return bs;
   }

   value.m_bBeginHeader = TRUE;

   if (bs.m_tokenarray[nCount-1] == '/') // begin and end header {FOO=foo/}
   {
      bs.m_tokenarray[nCount-1] = '\0';
      value.m_bEndHeader = TRUE;
   }

   value.m_strKey = bs.m_tokenarray;
   int nEqualsPos = value.m_strKey.Find('=');
   if (nEqualsPos != -1)
   {
      value.m_strValue = value.m_strKey.Right( value.m_strKey.Length() - nEqualsPos - 1 );
      value.m_strKey = value.m_strKey.Left( nEqualsPos );
   }
   else
      value.m_strValue.Empty();

   return bs;
}

///////////////////////////////////////////////////////////// 

//DataLabel::DataLabel( const String &strKey, const String &strValue )
//{
//   m_strKey = strKey;
//   m_strValue = strValue;
//}
//
//BaseStream &operator << ( BaseStream &bs, const DataLabel &value )
//{
//   bs << mar << '{' << value.m_strKey << '=' << value.m_strValue << "/}";
//
//   return bs;
//}
//
//BaseStream &operator >> ( BaseStream &bs, DataLabel &value )
//{
//   bs.ReadMargin();
//
//   char cTemp;
//   bs >> cTemp; // '{'
//
//   bs.ParseToken('=');
//   bs.GetToken( value.m_strKey );
//
//   int nCount = bs.ParseToken('}');
//   bs.m_tokenarray[nCount-1] = '\0';
//   bs.GetToken( value.m_strValue );
//
//   return bs;
//}
//
//BOOL DataLabel::IsLabel( MemoryStream &ms )
//{
//   ms.ReadMargin();
//   return (ms.GetBuffer()[ms.GetPos()] == '{');
//}
//
///////////////////////////////////////////////////////////// 
// SnipeObjectList
   
void SnipeObjectList::ComputeBoundingBox( BoundingBox &box, CGLBaseView *pView/*=NULL*/ )
{
   box.Clear();

   if (IsEmpty())
      return;

   for (POSITION pos=GetHeadPosition(); pos; )
   {
      SnipeObject *pSnipeObject = GetNext( pos );

      if (!pSnipeObject->IsVisible())
         continue;

      pSnipeObject->PushBounds( box, pView );
   }
}

void SnipeObjectList::ConvertCompatibleObjectsToSOT( SnipeObjectType nSOT )
{
   if (IsEmpty())
      return;

   if (!(nSOT==SOT_Vertex || nSOT==SOT_Edge || nSOT==SOT_Poly))
      return;

   CMap<Vertex *, Vertex *, Vertex *, Vertex *> vertexmap;
   vertexmap.InitHashTable( 1019 );

   SnipeObjectList newselectionlist;

   PolyModel *pModel = NULL;

   for (POSITION pos = GetHeadPosition(); pos; )
   {
      SnipeObject *pObject = GetNext( pos );
      if (pObject->IsKindOf(SOT_Vertex))
      {
         if (pModel==NULL)
            pModel = (PolyModel *)((Vertex *)pObject)->GetOwner();
         vertexmap.SetAt( (Vertex *)pObject, (Vertex *)pObject );
      }
      else if (pObject->IsKindOf( SOT_Edge ))
      {
         Edge *pEdge = (Edge *)pObject;
         if (pModel==NULL)
            pModel = (PolyModel *)pEdge->GetOwner();
         vertexmap.SetAt(pEdge->GetFirstVertex(), pEdge->GetFirstVertex());
         vertexmap.SetAt(pEdge->GetSecondVertex(), pEdge->GetSecondVertex());
      }
      else if (pObject->IsKindOf( SOT_Poly ))
      {
         Poly *pPoly = (Poly *)pObject;
         if (pModel==NULL)
            pModel = (PolyModel *)pPoly->GetOwner();
         for (UINT i=0; i<pPoly->GetNumSides(); i++)
         {
            Vertex *pVertex = pPoly->GetVertexAt(i);
            pVertex->AddToVertexMap( vertexmap );
         }
      }
      else
         newselectionlist.AddTail( pObject );
   }

   if (pModel)
   {
      if (nSOT == SOT_Vertex)
      {
         for (POSITION pos = vertexmap.GetStartPosition(); pos; )
         {
            Vertex *pVertex, *pTemp;
            vertexmap.GetNextAssoc( pos, pVertex, pTemp );
            newselectionlist.AddTail( pVertex );
         }
      }
      else if (nSOT == SOT_Edge)
      {
         for (POSITION pos=pModel->GetHeadEdgePosition(); pos; )
         {
            Edge *pEdge = pModel->GetNextEdge( pos );
            Vertex *pTemp;
            if (vertexmap.Lookup(pEdge->GetFirstVertex(), pTemp) && vertexmap.Lookup(pEdge->GetSecondVertex(), pTemp))
               newselectionlist.AddTail( pEdge );
         }
      }
      else if (nSOT == SOT_Poly)
      {
         for (POSITION pos=pModel->GetHeadFacePosition(); pos; )
         {
            Poly *pPoly = pModel->GetNextFace( pos );
            BOOL bFound = TRUE;
            for (UINT i=0; i<pPoly->GetNumSides(); i++)
            {
               Vertex *pVertex = pPoly->GetVertexAt(i);
               if (!pVertex->IsInVertexMap(vertexmap))
               {
                  bFound = FALSE;
                  break;
               }
            }
            if (bFound)
               newselectionlist.AddTail( pPoly );
         }
      }
   }
   
   RemoveAll();

   for (POSITION pos = newselectionlist.GetHeadPosition(); pos; )
      AddTail( newselectionlist.GetNext(pos) );
}

// SnipeObjectInfo //////////////////////////////////////////////
SnipeObjectInfoList *SnipeObjectInfo::m_pSnipeObjectInfoList;

SnipeObjectInfo::SnipeObjectInfo(SnipeObjectType nSOT, const char *szClassName, const char *szMatchName, PFN_CREATESNIPEOBJECT pfnCreate, PFN_CREATESNIPEOBJECTARRAY pfnCreateArray, int nNumPickElements, BOOL bIsCompoment/*=FALSE*/)
{
   m_nSOT = nSOT;
   m_strClassName = szClassName;
   m_strMatchName = szMatchName;
   m_pfnCreate = pfnCreate;
   m_pfnCreateArray = pfnCreateArray;
   m_bIsComponent = bIsCompoment;

   m_nResourceID = IDS_SOT_Unknown + nSOT;

   if (m_pSnipeObjectInfoList==NULL)
      m_pSnipeObjectInfoList = SNEW SnipeObjectInfoList;

   m_pSnipeObjectInfoList->Add(this);

   m_bNumPickElements = nNumPickElements; 
}

SnipeObjectInfo::~SnipeObjectInfo()
{
   m_pSnipeObjectInfoList->Remove(this);

   if (m_pSnipeObjectInfoList->IsEmpty()) 
   {
      delete m_pSnipeObjectInfoList;
      m_pSnipeObjectInfoList = NULL;
   }
}

const String &SnipeObjectInfo::GetLocalizedName()
{
   if (m_strLocalizedName.IsEmpty())
      m_strLocalizedName = String::GetResourceString( m_nResourceID );
   return m_strLocalizedName;
}

GLImageList &SnipeObjectInfo::GetImageListIcon()
{
   if (m_glImageListIcon.IsEmpty())
      m_glImageListIcon.Create( m_nResourceID );
   return m_glImageListIcon;
}

SnipeObject *SnipeObjectInfo::CreateObject() const
{
   return m_pfnCreate();
}

SnipeObject *SnipeObjectInfo::CreateObjectArray(UINT nCount)
{
   return m_pfnCreateArray(nCount);
}

BOOL IsFileOfType( const String &strFileName, SnipeObjectType nSOT )
{
   SnipeObjectInfo *pInfo = SnipeObjectInfo::GetBySOT( nSOT );
   if (pInfo==NULL)
   {
      ASSERT(pInfo);
      return FALSE;
   }

   FileStream fs;
   try 
   {
      fs.Open( strFileName, BaseStream::BS_INPUT|BaseStream::BS_ASCII );
   }
   catch (const String &strReason) {
      g_outputWindow << "Error in IsFileOfType: " << strReason << "\n";
      return FALSE;
   }
   catch (...) {
      g_outputWindow << "Unknown error occurred in IsFileOfType." << "\n";
      return FALSE;
   }

   DataHeader label;
   try 
   {
      fs >> label;
   }
   catch(...)
   {
      return SOT_Unknown;
   }

   if (label.m_strKey != "FileType")
      return FALSE;

   return label.m_strValue == pInfo->m_strClassName;
}

SnipeObjectType GetFileType( const String &strFileName )
{
   FileStream fs;
   try 
   {
      fs.Open( strFileName, BaseStream::BS_INPUT|BaseStream::BS_ASCII );
   }
   catch (const String &strReason) {
      g_outputWindow << "Error in GetFileType: " << strReason << "\n";
      return SOT_Unknown;
   }
   catch (...) {
      g_outputWindow << "Unknown error occurred in GetFileType." << "\n";
      return SOT_Unknown;
   }

   DataHeader label;
   try 
   {
      fs >> label;
   }
   catch(...)
   {
      return SOT_Unknown;
   }
   
   if (label.m_strKey != "FileType")
      return SOT_Unknown;

   SnipeObjectInfo *pInfo = SnipeObjectInfo::GetByClassName( label.m_strValue );
   if (pInfo==NULL) 
   {
      ASSERT(FALSE);
      return SOT_Unknown;
   }
   return pInfo->m_nSOT;
}

// SnipeObjectInfoList //////////////////////////////////////////////
SnipeObjectInfoList::SnipeObjectInfoList()
{
   m_sotmap.InitHashTable( 107 );
   m_classnamemap.InitHashTable( 107 );
   m_matchnamemap.InitHashTable( 107 );
}

SnipeObjectInfoList::~SnipeObjectInfoList()
{
}

void SnipeObjectInfoList::Add( SnipeObjectInfo *pInfo )
{
   m_snipeobjectinfolist.AddTail(pInfo);
   m_sotmap.SetAt(pInfo->m_nSOT, pInfo);
   m_classnamemap.SetAt(pInfo->m_strClassName.Get(), pInfo);
   m_matchnamemap.SetAt(pInfo->m_strMatchName.Get(), pInfo);
}

void SnipeObjectInfoList::Remove( SnipeObjectInfo *pInfo )
{
   POSITION pos;

   pos = m_snipeobjectinfolist.Find( pInfo );
   ASSERT( pos );
   m_snipeobjectinfolist.RemoveAt( pos );

   m_sotmap.RemoveKey( pInfo->m_nSOT );
   m_classnamemap.RemoveKey( pInfo->m_strClassName.Get() );
   m_matchnamemap.RemoveKey( pInfo->m_strMatchName.Get() );
}

BOOL SnipeObjectInfoList::IsEmpty() const
{
   return m_snipeobjectinfolist.IsEmpty();
}

SnipeObjectInfo *SnipeObjectInfoList::GetSnipeObjectInfoBySOT(SnipeObjectType nSOT)
{
  SnipeObjectInfo *pInfo;

   if (!m_sotmap.Lookup(nSOT, pInfo))
      return NULL;
   return pInfo;
}

SnipeObjectInfo *SnipeObjectInfoList::GetSnipeObjectInfoByClassName(const String &strClassName)
{
  SnipeObjectInfo *pInfo;

   if (!m_classnamemap.Lookup(strClassName, pInfo))
      return NULL;
   return pInfo;
}

SnipeObjectInfo *SnipeObjectInfoList::GetSnipeObjectInfoByMatchName(const String &strMatchName)
{
  SnipeObjectInfo *pInfo;

   if (!m_matchnamemap.Lookup(strMatchName, pInfo))
      return NULL;
   return pInfo;
}

/////////////////////////////////////////////////////////////////////////////
// SnipeObject

BOOL SnipeObject::m_bMirrorMode;
int SnipeObject::m_nCurrentPickElement;

SnipeObject::SnipeObject()
{
   m_nSnipeObjectFlags = 0;
   m_bVisible = TRUE;
   m_bAllowReinsert = TRUE;
   m_pNotifyNodeHead = NULL;
   m_pMirror = NULL;
   m_bMirrorDirty = TRUE;
   m_nRefCount = 0;
}

void SnipeObject::PostConstruct()
{
}

SnipeObject::~SnipeObject()
{
   if (m_pPrevCommandObject==this)
      m_pPrevCommandObject = NULL;
}

void SnipeObject::PreDestruct()
{
   ASSERT(!IsSelected()); // should have been caught by HierObject::OnObjectNotify or Component::Disconnect
   NotifyNodes( NOTIFYOBJ_DELETE, NULL, (LPARAM)this );
   if (GetMainInterface())
      GetMainInterface()->NotifyNodes( NOTIFYOBJ_DELETE, NULL, (LPARAM)this );
   //ASSERT( m_pNotifyNodeHead==NULL );
}

/////////////////////////////////////////////////////////////////////////////
// Root of message maps

const AFX_MSGMAP SnipeObject::messageMap = {	NULL,	&SnipeObject::_messageEntries[0] };

const AFX_MSGMAP* SnipeObject::GetMessageMap() const
{
	return &SnipeObject::messageMap;
}

const AFX_MSGMAP* SnipeObject::GetThisMessageMap()
{
	return &SnipeObject::messageMap;
}

const AFX_MSGMAP_ENTRY SnipeObject::_messageEntries[] = {
   SNIPEOBJECT_ON_UPDATE_COMMAND_UI_RANGE( ID_PLUGIN_COMMAND_FIRST, ID_PLUGIN_COMMAND_LAST, OnUpdatePluginCommand )
   SNIPEOBJECT_ON_COMMAND_EX_RANGE( ID_PLUGIN_COMMAND_FIRST, ID_PLUGIN_COMMAND_LAST, OnPluginCommand )
   { 0, 0, AfxSig_end, 0 }
};

struct AFX_CMDHANDLERINFO
{
	SnipeObject* pTarget;
	void (AFX_MSG_CALL SnipeObject::*pmf)(void);
};

union SnipeObjectMessageMapFunctions
{
	SNIPEOBJECT_AFX_PMSG pfn;   // generic member function pointer

   void (AFX_MSG_CALL SnipeObject::*pfnCmd_v_v)();
   BOOL (AFX_MSG_CALL SnipeObject::*pfnCmd_b_u)(UINT);
   void (AFX_MSG_CALL SnipeObject::*pfnCmdUI_v_C)(CCmdUI*);
};

AFX_STATIC BOOL AFXAPI _SnipeObjectDispatchCmdMsg(SnipeObject* pTarget, UINT nID, int nCode,
	SNIPEOBJECT_AFX_PMSG pfn, void* pExtra, UINT_PTR nSig, AFX_CMDHANDLERINFO* pHandlerInfo)
		// return TRUE to stop routing
{
	union SnipeObjectMessageMapFunctions mmf;
	mmf.pfn = pfn;
	BOOL bResult = TRUE; // default is ok

	if (pHandlerInfo != NULL)
	{
		// just fill in the information, don't do it
		pHandlerInfo->pTarget = pTarget;
		pHandlerInfo->pmf = mmf.pfn;
		return TRUE;
	}

	switch (nSig)
	{
	default:    // illegal
		ASSERT(FALSE);
		return 0;

	case AfxSigCmd_EX:
		// extended command (passed ID, returns bContinue)
		ASSERT(pExtra == NULL);
		bResult = (pTarget->*mmf.pfnCmd_b_u)(nID);
		break;
	case AfxSigCmdUI:
		{
			// ON_UPDATE_COMMAND_UI or ON_UPDATE_COMMAND_UI_REFLECT case
			ASSERT(CN_UPDATE_COMMAND_UI == (UINT)-1);
			ASSERT(nCode == CN_UPDATE_COMMAND_UI || nCode == 0xFFFF);
			ASSERT(pExtra != NULL);
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			ASSERT(!pCmdUI->m_bContinueRouting);    // idle - not set
			(pTarget->*mmf.pfnCmdUI_v_C)(pCmdUI);
			bResult = !pCmdUI->m_bContinueRouting;
			pCmdUI->m_bContinueRouting = FALSE;     // go back to idle
		}
		break;
	case AfxSigCmd_v:
		// normal command or control notification
		ASSERT(CN_COMMAND == 0);        // CN_COMMAND same as BN_CLICKED
		ASSERT(pExtra == NULL);
		(pTarget->*mmf.pfnCmd_v_v)();
		break;
	}
	return bResult;
}

BOOL SnipeObject::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// determine the message number and code (packed into nCode)
	const AFX_MSGMAP* pMessageMap;
	const AFX_MSGMAP_ENTRY* lpEntry;
	UINT nMsg = 0;

	if (nCode != CN_UPDATE_COMMAND_UI)
	{
		nMsg = HIWORD(nCode);
		nCode = LOWORD(nCode);
	}

	// for backward compatibility HIWORD(nCode)==0 is WM_COMMAND
	if (nMsg == 0)
		nMsg = WM_COMMAND;

	// look through message map to see if it applies to us
	for (pMessageMap = GetMessageMap(); pMessageMap; pMessageMap = (*pMessageMap->pfnGetBaseMap)())
	{
		// Note: catches BEGIN_MESSAGE_MAP(CMyClass, CMyClass)!
//		ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());

		lpEntry = AfxFindMessageEntry(pMessageMap->lpEntries, nMsg, nCode, nID);
		if (lpEntry != NULL) // found it
			return _SnipeObjectDispatchCmdMsg(this, nID, nCode, (SNIPEOBJECT_AFX_PMSG)lpEntry->pfn, pExtra, lpEntry->nSig, pHandlerInfo);

      if (!pMessageMap->pfnGetBaseMap)
         break;
   }
	return FALSE;   // not handled
}

BOOL SnipeObject::IsMouseOverObject() 
{ 
   return SelectionList::GetMouseOverObject() == this; 
}

BOOL SnipeObject::IsFocusObject() 
{ 
   return SelectionList::GetFocusObject() == this; 
}

BOOL SnipeObject::IsNearestFocusObject() 
{ 
   return SelectionList::GetNearestFocusObject() == this; 
}

CommandUIPlugin *SnipeObject::GetCommandUIPlugin(UINT nID)
{
   CommandUIPluginList *pList = GetCommandUIPluginList(GetThisSOT()); 
   for (POSITION pos = pList->GetHeadPosition(); pos; ) { 
      CommandUIPlugin *pPlugin = pList->GetNext(pos); 
      if (pPlugin->GetCmdID() == nID) 
         return pPlugin; 
   }
   return NULL;
}

BOOL SnipeObject::IsDraw()
{
   return IsVisible() || IsSelected();
}

const SnipeObject *SnipeObject::IsKindOf( SnipeObjectType nType ) const
{
   if (nType == SOT_SnipeObject)
      return this;
   else
      return NULL;
}

void SnipeObject::OnUpdatePluginCommand( CCmdUI *pCmdUI )
{
   CommandUIPlugin *pPlugin = GetCommandUIPlugin(pCmdUI->m_nID);
   if (pPlugin==NULL) {
      pCmdUI->ContinueRouting();
      return;
   }

   try {
      pPlugin->GetOnUpdateCommandObjectCallback()(pPlugin->GetPluginData(), this, pCmdUI);
   }
   catch(const String &strReason)
   {
      g_outputWindow << pPlugin->FormatError("OnUpdateCommandUI", strReason) << "\n";
   }
   catch(...) {
      g_outputWindow << pPlugin->FormatError("OnUpdateCommandUI", "Unknown") << "\n";
   }
}

BOOL SnipeObject::OnPluginCommand( UINT nID )
{
   CommandUIPlugin *pPlugin = GetCommandUIPlugin(nID);

   if (pPlugin == NULL)
      return FALSE; // Not for us, continue routing

   CommandPlugin *pCommand = NULL;
   String strParameters;

   try {
      pPlugin->GetOnCommandObjectCallback()(pPlugin->GetPluginData(), nID, this, pCommand, strParameters);
   }
   catch(const String &strReason)
   {
      g_outputWindow << pPlugin->FormatError("OnCommand", strReason) << "\n";
      return TRUE;
   }
   catch(...) {
      g_outputWindow << pPlugin->FormatError("OnCommand", "Unknown") << "\n";
      return TRUE;
   }

   if (pCommand==NULL) 
   {
      GetApp().DrawInvalidControls();
      return TRUE;
   }

   CommandList *pCommandList = GetMainInterface()->GetCurrentCommandList();
   if (pCommandList==NULL) {
      String strMsg("%OBJECTNAME of type %OBJECTTYPE cannot execute commands.");
      strMsg.Replace("%OBJECTNAME", GetTitle()); 
      strMsg.Replace("%OBJECTTYPE", GetClassName()); 
      g_outputWindow << strMsg << "\n";
      return TRUE;
   }

   StringList strResults;
   pCommandList->Do(pCommand, strParameters, strResults);

   return TRUE;
}

BOOL SnipeObject::IsDesendantOf(SnipeObject *pMatchObject) const
{
   for (SnipeObject *pParent = GetParent(); pParent; pParent = pParent->GetParent()) 
      if (pParent == pMatchObject)
         return TRUE;
   return FALSE;
}

void SnipeObject::CountPickObjectType( CGLBaseView *pView, UINT &nCount )
{
   if (IsKindOf(pView->m_nPickObjectType))
      nCount++;
}

SnipeObject *SnipeObject::GetCommandObject(BOOL bSkipThis/*=TRUE*/)
{
   for (SnipeObject *pObject= bSkipThis ? GetParent() : this; pObject; pObject=pObject->GetParent())
      if (pObject->IsCommandObject())
         return pObject;
   return NULL;
}

CommandList *SnipeObject::GetCommandList()
{
   if (GetParent()==NULL)
      return NULL;
   return GetParent()->GetCommandList();
}

BOOL SnipeObject::GetDrawColor( RGBAFloat &color )
{
   static RGBAFloat black( 0,0,0,1 );

   if (IsDrawMouseOverColor())
      color.Set( 1.0f, 0.5f, 0.0f, color.GetAlpha() );
   else if (IsDrawFocusColor())
      color.Set( 1.0f, 0.8f, 0.0f, color.GetAlpha() );
   else if (IsSelected())
      color.Set( 1.0f, 1.0f, 0.0f, color.GetAlpha() );
   else if (IsDrawMirrorColor())
      color.Set( 1.0f, 0.0f, 1.0f, color.GetAlpha() );
   else
      return FALSE;

   if (IsLocked())
   {
      color += black;
      color *= 0.5;
   }

   return TRUE;
}

SnipeObject *SnipeObject::FindObject( const String &strMatchName, SnipeObject *pExclude/*=NULL*/ )
{
   for (int i=0; i<GetNumChildren(); i++) 
   {
      SnipeObject *pSnipeObject = GetChildAt(i);
      if (pSnipeObject == pExclude)
         continue;
      if (pSnipeObject->GetMatchName() == strMatchName)
         return pSnipeObject;
   }

   return NULL;
}

Property *SnipeObject::FindProperty( const String &strPropertyMatchName )
{
   PropertyArray *pPropertyArray = GetPropertyArray();
   if (pPropertyArray==NULL)
      return NULL;
   
   for (int i=0; i<pPropertyArray->GetCount(); i++)
   {
      Property *pProperty = pPropertyArray->GetAt(i);
      if (pProperty->GetMatchName() == strPropertyMatchName)
         return pProperty;
   }
   return NULL;
}

SnipeObject *SnipeObject::FindDescendant(const String &strMatchName)
{
   for (int i=0; i<GetNumChildren(); i++)
   {
      SnipeObject *pChild = GetChildAt(i);
      if (pChild->GetMatchName() == strMatchName)
         return pChild;
      SnipeObject *pFound = pChild->FindDescendant( strMatchName );
      if (pFound)
         return pFound;
   }
   return NULL;
}

String SnipeObject::GetUniqueName( const String &strName )
{
   SnipeObject *pParentHierObject = GetParent();
   if (!pParentHierObject)
      return strName;

   int nCount = 1;
   String strBaseName = strName.GetNameOnlyWithoutSuffix().GetWithoutIntOnEnd();
   String strSuffix = strName.GetSuffixOnly();

   String strUniqueName = strName;

   while (pParentHierObject->FindObject( strUniqueName, this ))
   {
      if (strSuffix.IsEmpty())
         strUniqueName = strBaseName + ++nCount;
      else
         strUniqueName = strBaseName + ++nCount + '.' + strSuffix;
   }
   
   return strUniqueName;
}

void SnipeObject::InvalidateDependantViews()
{
   GetApp().InvalidateAllViews(this);
}

//////////////////////////////////////////////////////////////////////////////
SnipeObject *SnipeObject::m_pPrevCommandObject;

String SnipeObject::GetTitle() const
{
   static String name("Write a GetTitle()!");
   return name;
}

BOOL SnipeObject::AddNotifyNode( UINT msgs, SnipeObject *pObject )
{
   ASSERT( pObject );

   for (NotifyNode *pNode=m_pNotifyNodeHead; pNode; pNode=pNode->m_pNextNotifyNode) 
   {
      if (pNode->GetSnipeObject() == pObject)
      {
         if ((pNode->m_notificationmsgs | msgs) == pNode->m_notificationmsgs)
            return FALSE;
         pNode->m_notificationmsgs |= msgs;
         return TRUE;
      }
   }

   // Add notify nodes to the head, so the last in is the first out in deletion cases
   SnipeObjectNotifyNode *pNewNode = SNEW SnipeObjectNotifyNode( pObject, msgs );
   pNewNode->m_pNextNotifyNode = m_pNotifyNodeHead;
   m_pNotifyNodeHead = pNewNode;

   return TRUE;
}

BOOL SnipeObject::RemoveNotifyNode( SnipeObject *pObject, UINT msgs/*=NOTIFYOBJ_ALL*/ )
{
   for (NotifyNode **ppNode=&m_pNotifyNodeHead, *pNode=m_pNotifyNodeHead; pNode; ppNode=&pNode->m_pNextNotifyNode, pNode=pNode->m_pNextNotifyNode) 
   {
      if (pNode->GetSnipeObject() == pObject)
      {
         pNode->m_notificationmsgs &= ~msgs;
         if (!pNode->m_notificationmsgs)
         {
            *ppNode = pNode->m_pNextNotifyNode;
            delete pNode;
         }
         break;
      }
   }
   return TRUE;
}

BOOL SnipeObject::AddCallback( UINT msgs, PFN_ONOBJECTNOTIFY pfnOnObjectNotify, void *pData )
{
   ASSERT( pfnOnObjectNotify );

   for (NotifyNode *pNode=m_pNotifyNodeHead; pNode; pNode=pNode->m_pNextNotifyNode) 
   {
      if (pNode->GetPFN()==pfnOnObjectNotify && pNode->GetData()==pData)
      {
         if ((pNode->m_notificationmsgs | msgs) == pNode->m_notificationmsgs)
            return FALSE;
         pNode->m_notificationmsgs |= msgs;
         return TRUE;
      }
   }

   // Add notify nodes to the head, so the last in is the first out in deletion cases
   CallbackNotifyNode *pNewNode = SNEW CallbackNotifyNode( pfnOnObjectNotify, pData, msgs );
   pNewNode->m_pNextNotifyNode = m_pNotifyNodeHead;
   m_pNotifyNodeHead = pNewNode;

   return TRUE;
}

BOOL SnipeObject::RemoveCallback( PFN_ONOBJECTNOTIFY pfnOnObjectNotify, void *pData, UINT msgs/*=NOTIFYOBJ_ALL*/ )
{
   for (NotifyNode **ppNode=&m_pNotifyNodeHead, *pNode=m_pNotifyNodeHead; pNode; ppNode=&pNode->m_pNextNotifyNode, pNode=pNode->m_pNextNotifyNode) 
   {
      if (pNode->GetPFN()==pfnOnObjectNotify && pNode->GetData()==pData)
      {
         pNode->m_notificationmsgs &= ~msgs;
         if (!pNode->m_notificationmsgs)
         {
            *ppNode = pNode->m_pNextNotifyNode;
            delete pNode;
         }
         return FALSE;
      }
   }

   return TRUE;
}

BOOL SnipeObject::NotifyNodes( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
//if (msg == NOTIFYOBJ_SELECTIONLISTCHANGED)
//  g_outputWindow << "NOTIFYOBJ_SELECTIONLISTCHANGED: " << (BOOL)wParam << "\n";

   NotifyNode *pNextNode;
   for (NotifyNode *pNode=m_pNotifyNodeHead; pNode; pNode=pNextNode)
   {
      pNextNode = pNode->m_pNextNotifyNode;
      if (pNode->m_notificationmsgs & msg)
         pNode->OnObjectNotify( msg, wParam, lParam );
   }

   return TRUE;
}

BOOL SnipeObject::IsUsedBy( SnipeObject *pObject ) const
{
   for (NotifyNode *pNode=m_pNotifyNodeHead; pNode; pNode=pNode->m_pNextNotifyNode)
   {
      SnipeObject *pNodeObject = pNode->GetSnipeObject();
      if (pNodeObject==pObject || pNodeObject->IsDesendantOf( pObject ))
         return TRUE;
   }

   return FALSE;
}

CGLObjectTreeItem *SnipeObject::AllocTreeItem( CGLTreeControl *pTreeControl )
{
   return SNEW CGLObjectTreeItem( pTreeControl );
}

void SnipeObject::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   CGLObjectTreeControl::m_glImageListTreeIcons.Draw( x, 0.0f, GetIconIndex());

   glColor3f( 0.1f, 0.1f, 0.1f );
   pItem->DrawText( x+CGLObjectTreeControl::m_glImageListTreeIcons.GetWidthEach()+2, 12, GetTitle());
}

CommandUIPluginList *SnipeObject::GetCommandUIPluginList(SnipeObjectType nType)
{
   return GetMainInterface()->GetCommandUIPluginManager()->GetCommandUIPluginListObject(nType);
}

void SnipeObject::Select()
{
   static CommandPlugin *pSelectCommandPlugin = GetMainInterface()->GetCommandPluginManager()->FindCommandByCLIName( "SnipeBasicCommands", "Select" );

   if (pSelectCommandPlugin)
   {
      SnipeObject *pCommandObject = GetCommandObject();
      if (pCommandObject)
      {
         GetMainInterface()->SetCurrentWorkingObject( pCommandObject );
         String strObjectDescription;
         if (GetMainInterface()->ObjectToDescription( this, strObjectDescription ))
         {
            String strOptions;
            if (IsDownShift() && IsDownControl())
               strOptions = (String)"-o SUBTRACT "+strObjectDescription;
            else if (IsDownShift())
               strOptions = (String)"-o ADD "+strObjectDescription;
            else if (IsDownControl())
               strOptions = (String)"-o TOGGLE "+strObjectDescription;
            else
               strOptions = strObjectDescription;

            CommandList *pCommandList = pCommandObject->GetCommandList();
            GetMainInterface()->SetCurrentCommandObject( pCommandObject );
            StringList strResults;
            pCommandList->Do( pSelectCommandPlugin, strOptions, strResults );
         }
      }
   }
}

void SnipeObject::SetSelected( BOOL bSelect ) // Never call directly, call SelectionList::Add...
{
   if (!IsSelectable() || m_bSelected==bSelect)
      return;

   SnipeObject *pCurrentCommandObject = GetMainInterface()->GetCurrentCommandObject();
   if (bSelect && pCurrentCommandObject != m_pPrevCommandObject)
   {
      if (m_pPrevCommandObject)
      {
         SelectionList *pPrevSelectionList = m_pPrevCommandObject->GetSelectionList();
         if (pPrevSelectionList && !pPrevSelectionList->IsEmpty())
         {
            static CommandPlugin *pDeselectAllCommandPlugin = GetMainInterface()->GetCommandPluginManager()->FindCommandByCLIName( "SnipeBasicCommands", "DeselectAll" );
            if (pDeselectAllCommandPlugin)
            {
               GetMainInterface()->SetCurrentCommandObject( m_pPrevCommandObject );
               StringList strResults;

               m_pPrevCommandObject->GetCommandList()->Do( pDeselectAllCommandPlugin, "", strResults );
               GetMainInterface()->SetCurrentCommandObject( pCurrentCommandObject );
            }
         }
      }
      m_pPrevCommandObject = pCurrentCommandObject;
   }

   m_bSelected = bSelect;

   if (!m_bSelected && SelectionList::GetFocusObject()==this)
      SelectionList::SetFocusObject(NULL, NULL);

   OnSelectedChanged();
   if (!IsComponent()) // strictly for speed reasons although it isn't too bad (wait and see if someone needs notified on components)
   {
      OnObjectNotify( NOTIFYOBJ_SELECTIONCHANGED, bSelect, (LPARAM)this );
      GetMainInterface()->NotifyNodes( NOTIFYOBJ_SELECTIONCHANGED, bSelect, (LPARAM)this );
   }

#if 0
   // This test check to see if a selected object is in the selection list
   // and vice vera.  THis could get off if someone called this function
   // directly instead of calling SelectionList::Add or Remove...
   SelectionList *pList = GetMainInterface()->GetCurrentSelectionList();
   if (pList)
   {
      BOOL bInSelection = pList->Find( this );
      if (m_bSelected && !bInSelection)
         ASSERT(FALSE);
      else if (!m_bSelected && bInSelection)
         ASSERT(FALSE);
   }
#endif
}

void SnipeObject::SetVisible( BOOL bVisible )
{
   if (m_bVisible==bVisible)
      return;

   m_bVisible = bVisible;
   OnObjectNotify( NOTIFYOBJ_VISIBILITYCHANGED, bVisible, (LPARAM)this );
   
   SnipeObject *pMirror = GetMirrorObject();
   if (pMirror)
      pMirror->SetVisible( bVisible );
}

void SnipeObject::SetVisibleInTree( BOOL bVisible )
{
   if (m_bVisible==bVisible)
      return;

   m_bVisible = bVisible;
   OnObjectNotify( NOTIFYOBJ_VISIBILITYINTREECHANGED, bVisible, (LPARAM)this );
}

void SnipeObject::SetLocked( BOOL bLocked )
{
   if (m_bLocked==bLocked)
      return;

   m_bLocked = bLocked;
   OnObjectNotify( NOTIFYOBJ_LOCKEDCHANGED, bLocked, (LPARAM)this );

   SnipeObject *pMirror = GetMirrorObject();
   if (pMirror)
      pMirror->SetLocked( bLocked );
}

SnipeObjectList *SnipeObject::CreateListFromSOT(SnipeObjectType nSOT) // this needs to be KindOf?  can I do that?
{
   SnipeObjectInfo *pInfo = SnipeObjectInfo::GetBySOT(nSOT);
   if (pInfo==NULL)
      return NULL;

   SnipeObjectList *pList = NULL;
   if (pInfo->m_bIsComponent)
   {
      SimpleComponentList *pComponentList = FindComponentList( pInfo->m_strMatchName );
      if (pComponentList)
      {
         pList = SNEW SnipeObjectList;
         for (POSITION pos=pComponentList->GetHeadPosition(); pos; )
         {
            Component *pComponent = pComponentList->GetNext( pos );
            pList->AddTail(pComponent);
         }
      }
   }
   else
   {
      for (int i=0; i<GetNumChildren(); i++)
      {
         SnipeObject *pSnipeObject = GetChildAt( i );
         if (pSnipeObject->IsKindOf( nSOT ))
         {
            if (!pList)
               pList = SNEW SnipeObjectList;
            pList->AddTail( pSnipeObject );
         }
      }
   }

   return pList;
}

SnipeObjectList *SnipeObject::CreateListFromSOT(const String &strMatchName)
{
   SnipeObjectInfo *pInfo = SnipeObjectInfo::GetByMatchName(strMatchName);
   if (pInfo==NULL)
      return NULL;

   if (pInfo->m_bIsComponent) {
      SimpleComponentList *pComponentList = FindComponentList( strMatchName );
      if (pComponentList==NULL)
         return NULL;

      SnipeObjectList *pList = SNEW SnipeObjectList;
      for (POSITION pos=pComponentList->GetHeadPosition(); pos; )
      {
         Component *pComponent = pComponentList->GetNext( pos );
         pList->AddTail(pComponent);
      }
      return pList;
   }
   else {
      ASSERT(FALSE); // Now what?
      return NULL;
   }
}

SnipeObject *SnipeObject::FindComponent(const String &strComponentConst, String *pError)
{
   String strComponent = strComponentConst;

   int nStartBrace = strComponent.Find('(');
   if (nStartBrace == -1) {
      if (pError)
         *pError = "No '(' found for component";
      return NULL;
   }
   int nEndBrace = strComponent.Find(')');
   if (nEndBrace == -1) {
      if (pError)
         *pError = "No ')' found for component";
      return NULL;
   }

   char *buf = strComponent.Get();
   buf[nStartBrace] = '\0';
   buf[nEndBrace] = '\0';
   String strComponentList = &buf[nStartBrace+1];

   SimpleComponentList *pList = FindComponentList(strComponent);
   if (pList == NULL) {
      if (pError)
         *pError = (String)"Unknown component type " + strComponent;
      return NULL;
   }

   strComponentList.RemoveWhiteSpace();
   buf = strComponentList.Get();
   int nLength = strComponentList.Length();

   for (int i=0; i < nLength; i++)
   {
      if (buf[i] == ',' || buf[i] == ' ' || buf[i] == ':')
      {
         buf[i]='\0';
         break;
      }
   }
   int nID = atoi(buf);

   SnipeObject *pObject = pList->Find(nID);
   if (!pObject && pError)
      *pError = strComponentConst + " does not exist.";

   return pObject;
}

BOOL SnipeObject::FindComponents(SnipeObjectList *pObjectList, const String &strComponentConst, StringList &strErrorList)
{
   ASSERT(pObjectList);

   BOOL bFoundOne = FALSE;

   String strComponent = strComponentConst;

   int nStartBrace = strComponent.Find('(');
   if (nStartBrace == -1) 
      return FALSE;

   int nEndBrace = strComponent.Find(')');
   if (nEndBrace == -1)
   {
      String strError("Missing matching ')' for component: %1");
      strError.Replace("%1", strComponent);
      strErrorList.AddTail( strError );
      return FALSE;
   }

   char *buf = strComponent.Get();
   buf[nStartBrace] = '\0';
   buf[nEndBrace] = '\0';
   String strComponentList = &buf[nStartBrace+1];

   SimpleComponentList *pList = FindComponentList(strComponent);
   if (pList == NULL)
   {
      String strError("Unknown component type: %1");
      strError.Replace("%1", strComponent);
      strErrorList.AddTail( strError );
      return FALSE;
   }

   strComponentList.RemoveWhiteSpace();
   buf = strComponentList.Get();
   int nLength = strComponentList.Length();

   StringList *pComponentlist = strComponentList.Split(',');
   ASSERT(pComponentlist);

   Component *pComponent;
   for (POSITION pos = pComponentlist->GetHeadPosition(); pos; )
   {
      String &strCurrentComponent = pComponentlist->GetNext( pos );
      int nColon = strCurrentComponent.Find(':');
      if (nColon > 0) //Range
      {
         char *pBuf = strCurrentComponent.Get();
         pBuf[nColon] = '\0';
         int nStartID = atoi(pBuf);
         int nEndID = atoi(pBuf+nColon+1);
         if (nEndID < nStartID)
         {
            int nTemp = nStartID;
            nStartID = nEndID;
            nEndID = nTemp;
         }
         for (int i=nStartID; i <= nEndID; i++)
         {
            pComponent = pList->Find(i);
            if (pComponent==NULL)
            {
               if (strErrorList.IsEmpty()) // only put the first one in there, otherwise it is too slow
               {
                  String strMsg("Unable to locate %1(%2) for %3.  Only reporting first unfound %1.");
                  strMsg.Replace("%1", (String)strComponent);
                  strMsg.Replace("%2", (String)i);
                  strMsg.Replace("%3", GetMatchName());
                  strErrorList.AddTail( strMsg );
               }
            }
            else
            {
               pObjectList->AddTail(pComponent);
               bFoundOne = TRUE;
            }
         }
      }
      else
      { // Single component
         int nID = atoi(strCurrentComponent.Get());
         pComponent = pList->Find(nID);
         if (pComponent==NULL)
         {
            String strMsg("Unable to locate component [%1] for Object [%2]");
            strMsg.Replace("%1", (String)nID);
            strMsg.Replace("%2", GetMatchName());
            strErrorList.AddTail( strMsg );
            delete pComponentlist;
            return FALSE;
         }
         else
         {
            pObjectList->AddTail(pComponent);
            bFoundOne = TRUE;
         }
      }
   }
   delete pComponentlist;
   return bFoundOne;
}

const Matrix44 &SnipeObject::GetObjectToWorldMatrix( const Time &time/*=GetTime()*/ )
{
   return Identity44();
}

const Matrix44 &SnipeObject::GetWorldToObjectMatrix( const Time &time/*=GetTime()*/ )
{
   return Identity44();
}

void SnipeObject::MarkSaveDependents()
{
   m_bSaveDependent = TRUE;
}

void SnipeObject::SaveFile( BOOL bSaveAs/*=FALSE*/ )
{
   FileInfoProperty *pInfo = GetFileInfo();
   String strFileName;

   if (pInfo==NULL) 
   {
      AfxMessageBox("Object is not of a savable type.", MB_OK|MB_ICONEXCLAMATION);
      return;
   }

   if ( !pInfo->m_strFileName.GetFileName( strFileName, bSaveAs ) )
      return;

   CWaitCursor waitcursor;
   FileStream fs;

   try 
   {
      fs.Open( strFileName );
   }
   catch (const String &strReason) 
   {
      AfxMessageBox( strReason.Get(), MB_OK|MB_ICONEXCLAMATION );
      return;
   }
 
   String strOldBasePath = SetBasePath( strFileName.GetPathOnly() );

   fs << DataHeader( "FileType", GetClassName() ) << nl;
   fs << DataHeader( "Version", SNIPEVERSION ) << nl;
   fs << nl;

   try 
   {
      MarkSaveDependents();
      GetMainInterface()->GetProject()->Save( fs, TRUE );
   }
   catch (const String &strReason) 
   {
      AfxMessageBox( strReason.Get(), MB_OK|MB_ICONEXCLAMATION );
   }
   catch (...)
   {
      String strReason("Unknown error occurred while saving %1.");
      strReason.Replace("%1", GetTitle());
      AfxMessageBox( strReason.Get(), MB_OK|MB_ICONEXCLAMATION );
   }

   SetBasePath( strOldBasePath );
}

void SnipeObject::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   m_bSaveDependent = FALSE;
//  TODO: OffModified();
}

SnipeObject *SnipeObject::GetMirrorObject()
{
   if ( !m_bMirrorMode ) 
   {
      SetMirrorDirty();
      return NULL;
   }

   if (m_bMirrorDirty)
   {
      SnipeObject *pMirror = FindMirrorObject();
      SetMirrorObject( pMirror );  
   }

   if (m_pMirror && /*m_pMirror != this &&*/ IsSelected() && m_pMirror->IsSelected())
      return NULL;

   return m_pMirror;
}

void SnipeObject::SetMirrorObject( SnipeObject *pObject )
{
   m_bMirrorDirty = FALSE;

   if ( m_pMirror == pObject )
      return;

   m_pMirror = pObject;

   if (pObject)
      pObject->SetMirrorObject(this);
}

Selection *SnipeObject::AddSelection()
{
   SelectionContainer *pSelectionContainer = CreateSelectionContainer();
   if (!pSelectionContainer)
      return NULL;

   Selection *pSelection = Selection::Create();
   pSelectionContainer->AddChildAtTail( pSelection );

   return pSelection;
}

BOOL SnipeObject::Delete( DeleteInfo *&pDeleteInfo, String &strOutError ) // pass in the block of memory returned by AllocDeleteInfo
{
   if (pDeleteInfo && pDeleteInfo->m_pMirrorDeleteInfo)
      pDeleteInfo->m_pMirrorDeleteInfo->GetSnipeObject()->Delete(pDeleteInfo->m_pMirrorDeleteInfo, strOutError);
   else
   {
      SnipeObject *pMirror = GetMirrorObject();

      if (pMirror)
         pMirror->Delete( pDeleteInfo->m_pMirrorDeleteInfo, strOutError );
   }

   return TRUE;
}

BOOL SnipeObject::UnDelete( DeleteInfo *pDeleteInfo )
{
   if (pDeleteInfo->m_pMirrorDeleteInfo)
      pDeleteInfo->m_pMirrorDeleteInfo->GetSnipeObject()->UnDelete( pDeleteInfo->m_pMirrorDeleteInfo );

   //for (PointerPropertyNode *pNode=pDeleteInfo->m_pPointerPropertyNodeHead; pNode; pNode=pNode->m_pNextNode)
   //{
   //   PointerProperty<SnipeObject *> *pPointerProperty = pNode->m_pPointerProperty;
   //   pPointerProperty->StoreValue( this, 0, FALSE );
   //}

   return TRUE;
}

BaseStream &operator << ( BaseStream &bs, const SnipeObject *&value )
{
   ASSERT(bs.IsAsciiStream());
   String strDescription;
   if (GetMainInterface()->ObjectToDescription( value, strDescription ))
      bs << strDescription;
   return bs;
}

BaseStream &operator >> ( BaseStream &bs, SnipeObject *&value )
{
   ASSERT(bs.IsAsciiStream());
   String strDescription;
   if (bs.ReadLine( strDescription ))
   {
      String strError;
      value = GetMainInterface()->DescriptionToObject( strDescription, &strError );
      if (!value)
         g_outputWindow << strError << "\n";
   }
   return bs;
}

void SnipeObject::PushBounds( BoundingBox &box, CGLBaseView *pView/*=NULL*/ )
{
   Vector vPos( 0.0f );

   GetWorldPos( vPos );
   if (pView)
      pView->WorldToView( vPos );
   box.Push( vPos );
}

// copy argument into this
void SnipeObject::Copy( SnipeObject *pSnipeObject )
{
   ASSERT( GetSOT() == pSnipeObject->GetSOT() );
   MemoryStream ms;

   pSnipeObject->Save( ms );

   ms.Seek( 0, BaseStream::ST_BEGINNING );
   Load( ms );
}

SnipeObject *SnipeObject::Duplicate()
{
   SnipeObjectInfo *pSnipeObjectInfo = pSnipeObjectInfo->GetBySOT( GetSOT() );
   SnipeObject *pSnipeObject = pSnipeObjectInfo->CreateObject();

   pSnipeObject->Copy( this );

   return pSnipeObject;
}

