#ifndef __CONNECTIONMANAGER_H__
#define __CONNECTIONMANAGER_H__

#undef _WINDOWS_
#include <afxtempl.h>

#include "IMMDLL.h"

class ConnectionManager;

class ConnectionEntry
{
   friend ConnectionManager;
public:
   ConnectionEntry();
   ~ConnectionEntry();

   MDagPath GetDagPath() { return m_dagPath; }
   ConnectionManager *GetConnectionManager() { return m_pConnectionManager; }
   MString GetInstanceName() { return m_instanceName; }

private:
   enum connectionType { MODEL, SCENE, NODE } m_connectionType;
   void *m_pData;
	MCallbackId m_callbackID;
   MDagPath m_dagPath;
   ConnectionManager *m_pConnectionManager;
   MString m_instanceName;
};

class ConnectionManager
{
public:
   ConnectionManager();
   ~ConnectionManager();

   IMM::NewConnectionAction NewGameConnection(IMM::IConnection* connection);

   MStatus CreateMayaNodeConnection(MDagPath &dagPath,
                                    void (*func)( MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData ),
                                    void *pData=NULL);
   ConnectionEntry *GetMayaConnection(UINT nIndex);
   void CloseMayaConnection(UINT nIndex);

   MStatus CreateMayaSelectedNodeConnection(MDagPath &dagPath,
      void (*func)( MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData ),
      void *pData=NULL);
   ConnectionEntry *GetMayaSelectedConnection(UINT nIndex);
   void ClearMayaSelectedConnections();

   IMM::IConnection *GetGameConnection() { return m_pConnection; }

   void ActiveSelectionChanged();

private:
   CArray<ConnectionEntry> m_mayaconnections;
   CArray<ConnectionEntry> m_mayaselectedconnections;
   MCallbackId m_activeselectionchangedcallback;

   IMM::IConnection *m_pConnection;
   class IMMEventSink : public IMM::IEventSink
   {
      IMM::IConnection *m_pConnection;
      ConnectionManager *m_pConnectionManager;
   public:
      IMMEventSink(
         IMM::IConnection *pConnection,
         ConnectionManager *pConnectionManager
         ) :
      m_pConnection( pConnection ),
      m_pConnectionManager( pConnectionManager )
      {
         m_pConnection->SetEventSink(this);
      }
      ~IMMEventSink()
      {
         m_pConnection->SetEventSink(NULL);
      }

      virtual void OnCloseConnection()
      {
         TRACE("sink reported closed\n");
         // we must POST this message as otherwise, the result would delete
         // the sink, which is a bit illegal (to delete it from within it)
         // not to mention that the result also kills the connection, which
         // contains the thread that called this routine... shutting down
         // is nasty business
         //        mDialog->PostMessage( WM_COMMAND, IDC_DISCONNECT, NULL );
      }
   };

   IMMEventSink* m_pTheKitchenSink;

};

extern ConnectionManager g_connectionmanager;

#endif