//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//		ARG #1 :	1 - PROPORTIONAL TEXTUREMAPPING ON
//					0 - PROPORTIONAL TEXTUREMAPPING OFF
//
//		ARG #2 :	1 - WELD INTERNAL UVS ON
//					0 - WELD INTERNAL UVS OFF
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _tileUVs
#define _tileUVs


class Vector2;

class tileUVs : public MPxCommand {
public:
	tileUVs ();
	~tileUVs (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
//	Vector2			CalcPolyUV(int vert,int col,int row,int maxcol,int maxrow);
//	Vector2			CalcPolyUVProportional(int vert,int col,int row,int maxcol,int maxrow);

	int ui_nostretch,ui_welduvs;
	double ui_scaletofit;
	HDoubleArrayList ulist,vlist;
};

#endif