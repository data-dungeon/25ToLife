#ifndef __CONNECTCAMERATOGAME_H__
#define __CONNECTCAMERATOGAME_H__

class ConnectCameraToGameData
{
public:
   MDagPath m_cameraDagPath;
};

class ConnectCameraToGame : public MPxCommand
{
public:
   ConnectCameraToGame ();
   ~ConnectCameraToGame (); 

   MStatus		doIt( const MArgList& args );
   MStatus     redoIt();
   bool        isUndoable() const;

   static void*	creator();

   static void ConnectDefaultCamera();

public:
   MDagPath m_cameraDagPath;
};

#endif