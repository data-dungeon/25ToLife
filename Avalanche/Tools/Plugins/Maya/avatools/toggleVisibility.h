//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _toggleVisibility
#define _toggleVisibility

class toggleVisibility : public MPxCommand {
public:
	toggleVisibility ();
	~toggleVisibility (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

	static void	toggleComponentsVisibility(MStringArray &selected);
	static void	toggleObjectsVisibility(MStringArray &selected);

protected:
		MStringArray resultarray;
};

#endif