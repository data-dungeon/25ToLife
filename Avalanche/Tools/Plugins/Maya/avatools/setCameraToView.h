//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		0 - front, 1 - back, 2 - left, 3 - right, 4 - top, 5 - bottom
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _setCameraToView
#define _setCameraToView

class setCameraToView : public MPxCommand
{
public:
	setCameraToView ();
	~setCameraToView (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
	int m_cameraview;
};

#endif