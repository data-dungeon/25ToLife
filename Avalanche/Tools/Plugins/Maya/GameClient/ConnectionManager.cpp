#include "GameClientPCH.h"
#include "ConnectionManager.h"
#include "ConnectCameraToGame.h"

ConnectionManager g_connectionmanager;

IMM::NewConnectionAction NewConnectionCallback(IMM::IConnection* pConnection)
{
   return g_connectionmanager.NewGameConnection(pConnection);
}

static void NotifySelSetChanged(void *pClientData)
{
   ConnectionManager *pConnectionManager = (ConnectionManager*)pClientData;
   if (pConnectionManager->GetGameConnection())
      pConnectionManager->ActiveSelectionChanged();
}

ConnectionManager::ConnectionManager()
{
   MStatus status;
   m_pConnection = NULL;
   m_pTheKitchenSink = NULL;

   if(!IMM::AddNewConnectionCallback(NewConnectionCallback))
      MGlobal::displayError("Couldn't initialize callback with game engine.\n");

   m_activeselectionchangedcallback = MModelMessage::addCallback(MModelMessage::kActiveListModified,NotifySelSetChanged,this,&status);
   if (!status)
      MGlobal::displayError("Couldn't initialize active selection callback.\n");
}

ConnectionManager::~ConnectionManager()
{
  IMM::RemoveNewConnectionCallback( NewConnectionCallback );

   delete m_pTheKitchenSink;
   m_pTheKitchenSink = NULL;

   delete m_pConnection;
   m_pConnection = NULL;

   MModelMessage::removeCallback(m_activeselectionchangedcallback);
}

IMM::NewConnectionAction ConnectionManager::NewGameConnection(IMM::IConnection* pConnection)
{
   if ( m_pConnection != NULL )
      return IMM::REFUSE_CONNECTION;

   m_pConnection = pConnection;
   m_pTheKitchenSink = new IMMEventSink( m_pConnection, this );

   ConnectCameraToGame::ConnectDefaultCamera();

   return IMM::ACCEPT_CONNECTION;
}

MStatus ConnectionManager::CreateMayaNodeConnection(MDagPath &dagPath,
                                              void (*func)( MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData ),
                                              void *pData/*=NULL*/)
{
   MStatus status;

   UINT nIndex = m_mayaconnections.Add(ConnectionEntry());
   ConnectionEntry *pConnectionEntry = &m_mayaconnections[nIndex];

   pConnectionEntry->m_connectionType = ConnectionEntry::NODE;
   pConnectionEntry->m_dagPath = dagPath;
   pConnectionEntry->m_pData = pData;
   pConnectionEntry->m_callbackID = MNodeMessage::addAttributeChangedCallback(dagPath.node(),func,pConnectionEntry,&status);
   pConnectionEntry->m_pConnectionManager = this;
   return status;
}

ConnectionEntry *ConnectionManager::GetMayaConnection(UINT nIndex)
{
   return &m_mayaconnections[nIndex];
}

void ConnectionManager::CloseMayaConnection(UINT nIndex)
{
   m_mayaconnections.RemoveAt(nIndex);
}

static void SelectedNodeChanged(MNodeMessage::AttributeMessage msg, MPlug &plug,MPlug &otherPlug, void *pClientData)
{
   ConnectionEntry *pConnectionEntry = (ConnectionEntry*)pClientData;
   ConnectionManager *pConnectionManager = pConnectionEntry->GetConnectionManager();
   IMM::IConnection *pGameConnection = pConnectionManager->GetGameConnection();
   if (!pGameConnection)
      return;  

	if (!(msg && MNodeMessage::kAttributeSet))
      return;

   MMatrix matrix = pConnectionEntry->GetDagPath().inclusiveMatrix();
  
   IMM::Matrix gamematrix;
   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         gamematrix.m[i][j] = (float)matrix[i][j];

   const char *debug = pConnectionEntry->GetInstanceName().asChar();

   gamematrix.m[3][0] = gamematrix.m[3][0] * 0.01f;
   gamematrix.m[3][1] = gamematrix.m[3][1] * 0.01f;
   gamematrix.m[3][2] = gamematrix.m[3][2] * 0.01f;

   pGameConnection->MoveInstanceByName(pConnectionEntry->GetInstanceName().asChar(),gamematrix);
}

void ConnectionManager::ActiveSelectionChanged()
{
   MSelectionList activeselection;
   MGlobal::getActiveSelectionList(activeselection);

   ClearMayaSelectedConnections();

   MDagPath dagPath;
   for (UINT i = 0; i < activeselection.length(); i++)
   {
      activeselection.getDagPath(i,dagPath);
      CreateMayaSelectedNodeConnection(dagPath,SelectedNodeChanged);
   }
}

MStatus ConnectionManager::CreateMayaSelectedNodeConnection(MDagPath &dagPath,
                                                    void (*func)( MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData ),
                                                    void *pData/*=NULL*/)
{
   MStatus status;
   UINT nIndex = m_mayaselectedconnections.Add(ConnectionEntry());
   ConnectionEntry *pConnectionEntry = &m_mayaselectedconnections[nIndex];
   pConnectionEntry->m_connectionType = ConnectionEntry::NODE;
   pConnectionEntry->m_dagPath = dagPath;
   pConnectionEntry->m_pData = pData;
   pConnectionEntry->m_callbackID = MNodeMessage::addAttributeChangedCallback(dagPath.node(),func,pConnectionEntry,&status);
   pConnectionEntry->m_pConnectionManager = this;

   MString fullPathName = dagPath.fullPathName();
   MStringArray parts;
   fullPathName.split('|',parts);

   pConnectionEntry->m_instanceName = parts[parts.length()-1];

   return status;
}

ConnectionEntry *ConnectionManager::GetMayaSelectedConnection(UINT nIndex)
{
   return &m_mayaselectedconnections[nIndex];
}

void ConnectionManager::ClearMayaSelectedConnections()
{
   m_mayaselectedconnections.RemoveAll();
}
