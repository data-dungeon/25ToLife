//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _multilisterToBoxes
#define _multilisterToBoxes

class multilisterToBoxes : public MPxCommand {
public:
	multilisterToBoxes ();
	~multilisterToBoxes (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MStatus SetMultiListerToBoxes();

};

#endif