//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		ARG #1 :	1 - INCREMENT TILE
//					0 - DECREMENT TILE
//
//		ARG #2 :	1 - IN U DIRECTION
//					0 - IN V DIRECTION
//
//      ARG #3 :    INCREMENT BY
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _scaleUVs
#define _scaleUVs

class scaleUVs : public MPxCommand {
public:
	scaleUVs ();
	~scaleUVs (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
	bool m_increment_tile;
	bool m_udirection;
	double m_increment_by;
	MStringArray m_selFaces,m_selUVs;
};

#endif