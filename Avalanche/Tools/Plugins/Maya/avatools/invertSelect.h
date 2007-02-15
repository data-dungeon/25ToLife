//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _invertSelect
#define _invertSelect

class invertSelect : public MPxCommand {
public:
	invertSelect ();
	~invertSelect (); 

	MStatus	doIt( const MArgList& args );
	MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;
static	MStatus		invertSelection(bool include_invisible);

	static void*	creator();

private:
	bool ui_include_invisible;
};

#endif