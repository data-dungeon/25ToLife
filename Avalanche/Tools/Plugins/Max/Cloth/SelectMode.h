#ifndef __SELECTMODE_H__
#define __SELECTMODE_H__


#define CID_CREATESELECT CID_USER + 0x4128

class SelectMode : public CommandMode
{
public:
	SelectMode(Cloth* pCloth, IObjParam* i) : m_mouseProc(pCloth, i), m_fgObj(pCloth) { m_pCloth = pCloth; }

	int Class() { return PICK_COMMAND; }
	int ID() { return CID_CREATESELECT; }
	MouseCallBack *MouseProc(int *numPoints) { *numPoints = 1; return &m_mouseProc; }
	ChangeForegroundCallback *ChangeFGProc() { return CHANGE_FG_SELECTED; }
	BOOL ChangeFG( CommandMode *oldMode ) { return (oldMode->ChangeFGProc() != &m_fgObj); }
	void EnterMode();
	void ExitMode();

private:
	ChangeFGObject m_fgObj;
	SelectMouseProc m_mouseProc;
	Cloth* m_pCloth;
};


#endif // __SELECTMODE_H__
