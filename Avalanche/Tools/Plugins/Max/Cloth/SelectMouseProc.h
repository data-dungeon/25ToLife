#ifndef __SELECTMOUSEPROC_H__
#define __SELECTMOUSEPROC_H__

// helper function
float SquaredDistanceToLine(const Point3& lineBegin, const Point3& lineEnd, const Point3& point, bool clamp = true);


class SelectMouseProc : public MouseCallBack
{
public:
	SelectMouseProc(Cloth* pCloth, IObjParam *i) { m_pCloth = pCloth; pInterface = i; }
	int proc(HWND hwnd, int msg, int point, int flags, IPoint2 m );

protected:
	BOOL HitTest(ClothModData* pModData, ViewExp* pViewPort, IPoint2* pt, Object* pObject, INode* pNode);
	BOOL SelectMouseProc::DragSelectHitTest(ClothModData *pModData, ViewExp* pViewport, Object* pObject, INode* pNode);
	BOOL AnyHits(ViewExp* pViewPort) { return pViewPort->NumSubObjHits(); }

private:
	Cloth*			m_pCloth;
	IObjParam*		pInterface;
	IPoint2			lastPoint;
	IPoint2			om;
	Tab<IPoint2>	mouseHitList;
};


#endif // __SELECTMOUSEPROC_H__
