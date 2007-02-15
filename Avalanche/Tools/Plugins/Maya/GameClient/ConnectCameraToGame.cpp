#include "GameClientPCH.h"
#include "ConnectCameraToGame.h"

ConnectCameraToGame::ConnectCameraToGame()
{
}

ConnectCameraToGame::~ConnectCameraToGame()
{
}

void* ConnectCameraToGame::creator()
{
   return new ConnectCameraToGame();
}

bool ConnectCameraToGame::isUndoable() const
{
   return false;
}

MStatus ConnectCameraToGame::doIt( const MArgList& args )
{
   MStatus status = MS::kFailure;

   MSelectionList selection;

   if (args.length()>0)
   {  
      MString cameraName;
      args.get(0,cameraName);
      selection.add(cameraName);
   }

   if (selection.length()==0)
   {
      MGlobal::getActiveSelectionList(selection);
      if (selection.length()==0)
         return MS::kFailure;
   }

   selection.getDagPath(0,m_cameraDagPath);

   status = redoIt();


   return status;
}

static void NotifyCameraTransformChanged(MNodeMessage::AttributeMessage msg, MPlug &plug,MPlug &otherPlug, void *pClientData)
{
   MStatus status;

   ConnectionEntry *pConnectionEntry = (ConnectionEntry*)pClientData;

   ConnectionManager *pConnectionManager = pConnectionEntry->GetConnectionManager();

   IMM::IConnection *pGameConnection = pConnectionManager->GetGameConnection();

   if (!pGameConnection)
      return;

   if (!(msg && MNodeMessage::kAttributeSet))
      return;

   MDagPath cameraDagPath;
   cameraDagPath = pConnectionEntry->GetDagPath();
   cameraDagPath.push(cameraDagPath.child(0));
   MString test = cameraDagPath.fullPathName();
   const char *debug = test.asChar();

   MFnCamera fnCamera(cameraDagPath);
   MPoint eyepoint =fnCamera.eyePoint(MSpace::kWorld,&status);
   MPoint focuspoint = fnCamera.centerOfInterestPoint(MSpace::kWorld,&status);
   MVector upvect = fnCamera.upDirection(MSpace::kWorld,&status);

   IMM::Vector3 eye,focus,up;
   eye.v[0] = (float)eyepoint.x * 0.01f;
   eye.v[1] = (float)eyepoint.y * 0.01f;
   eye.v[2] = (float)eyepoint.z * 0.01f;
   focus.v[0] = (float)focuspoint.x * 0.01f;
   focus.v[1] = (float)focuspoint.y * 0.01f;
   focus.v[2] = (float)focuspoint.z * 0.01f;
   up.v[0] = (float)upvect.x;
   up.v[1] = (float)upvect.y;
   up.v[2] = (float)upvect.z;

   pGameConnection->UpdateCamera(eye,focus,up);
}

MStatus ConnectCameraToGame::redoIt()
{
   MStatus status;

   status = g_connectionmanager.CreateMayaNodeConnection(m_cameraDagPath,NotifyCameraTransformChanged);
   if (!status)
      MGlobal::displayError("Couldn't create callback.");

   return MS::kSuccess;
}

void ConnectCameraToGame::ConnectDefaultCamera()
{
   MStatus status;
   MString cameraName = "persp";
   MSelectionList selection;
   selection.add(cameraName);
   if (selection.length()==0)
      return;
   MDagPath cameraDagPath;
   selection.getDagPath(0,cameraDagPath);
   status = g_connectionmanager.CreateMayaNodeConnection(cameraDagPath,NotifyCameraTransformChanged);
   if (!status)
      MGlobal::displayError("Couldn't create default camera callback.");
}
