#ifndef __RESTORE__H
#define __RESTORE__H

class CModData;
class CPerFaceDataMod;

// ---Selection Restoration Methods
class CSelectRestoreObj : public RestoreObj 
{
	BitArray m_baUsel;
	BitArray m_baRsel;
	CPerFaceDataMod *m_pPFDMod;
	CModData *m_pModData;
	int m_nLevel;

public:
	CSelectRestoreObj(CPerFaceDataMod *m, CModData *d);
	CSelectRestoreObj(CPerFaceDataMod *m, CModData *d, int level);
	void Restore( int isUndo );
	void Redo();
	int Size();
	void EndHold();
	TSTR Description();
};
#endif __RESTORE__H
