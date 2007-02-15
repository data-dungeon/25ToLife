
#ifndef ORIENT_LIKE_CMD_H
#define ORIENT_LIKE_CMD_H

class OrientLikeCmd : public MPxCommand 
{
public:
	OrientLikeCmd();
	~OrientLikeCmd();

	MStatus	doIt ( const MArgList& );
	MStatus undoIt();
 	MStatus redoIt();
	bool isUndoable() const;

	static MSyntax newSyntax();
	static void *creator();

private:
	MObject GetOrientLikeConstraintObject(const MSelectionList &selectionList,MStatus *pStatus = NULL) const;
	int GetTargetLogicalIndex(const MObject &testTarget,const MPlug &targetsPlug, MStatus *pStatus = NULL) const;

	MDagModifier m_dagMod[3];
	bool m_bUndoable;

	static MSyntax m_cachedSyntax;
	static bool m_bCachedSyntax;
};

#endif