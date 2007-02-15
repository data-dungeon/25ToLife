//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _makeObjectsNamesSimilar
#define _makeObjectsNamesSimilar

class makeObjectsNamesSimilar : public MPxCommand {
public:
	makeObjectsNamesSimilar ();
	~makeObjectsNamesSimilar (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:

};

#endif