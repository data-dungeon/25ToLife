//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _copyCharacter
#define _copyCharacter

class copyCharacter : public MPxCommand
{
public:
	copyCharacter ();
	~copyCharacter (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:

};

#endif