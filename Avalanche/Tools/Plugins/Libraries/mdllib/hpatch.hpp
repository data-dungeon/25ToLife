//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HPATCH INLINE METHODS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// vector math for creating patch tangents
///////////////////////////////////////////////////////////////////////////////////////////////////

inline void HPatch::calculateInteriorTangents(HVector *pVectors) {
	double twt = (2.0/3.0);
	double ont = (1.0/3.0);

	HVector A1 = pVectors[4] - pVectors[0];
	HVector A2 = pVectors[8] - pVectors[12];
	HVector B1 = pVectors[13] - pVectors[12];
	HVector B2 = pVectors[14] - pVectors[15];
	HVector C1 = pVectors[11] - pVectors[15];
	HVector C2 = pVectors[7] - pVectors[3];
	HVector D1 = pVectors[2] - pVectors[3];
	HVector D2 = pVectors[1] - pVectors[0];

	if (pVectors[0] == pVectors[12]) {
		A1 = 0;
		A2 = 0;
	}
	
	pVectors[5] = (pVectors[0] + twt*A1 + ont*C2 + twt*D2 + ont*B1);
	pVectors[9] = (pVectors[12] + twt*A2 + ont*C1 + twt*B1 + ont*D2);
	pVectors[10] = (pVectors[15] + twt*C1 + ont*A2 + twt*B2 + ont*D1);
	pVectors[6] = (pVectors[3] + twt*C2 + ont*A1 + twt*D1 + ont*B2);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// bezier patch math for calculating surface points,normals, and uv's
///////////////////////////////////////////////////////////////////////////////////////////////////

inline HVector HPatch::calcPointOnPatch(double s, double t, unsigned nPatch) {
	HVector pnt;
	HVector a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;

	a = patchVectors[nPatch].pv[0];
	b = patchVectors[nPatch].pv[4];
	c = patchVectors[nPatch].pv[8];
	d = patchVectors[nPatch].pv[12];

	e = patchVectors[nPatch].pv[1];
	f = patchVectors[nPatch].pv[5];
	g = patchVectors[nPatch].pv[9];
	h = patchVectors[nPatch].pv[13];

	i = patchVectors[nPatch].pv[2];
	j = patchVectors[nPatch].pv[6];
	k = patchVectors[nPatch].pv[10];
	l = patchVectors[nPatch].pv[14];

	m = patchVectors[nPatch].pv[3];
	n = patchVectors[nPatch].pv[7];
	o = patchVectors[nPatch].pv[11];
	p = patchVectors[nPatch].pv[15];

	double bezierbasis[4][4] = {{ -1,3,-3,1}, {3, -6, 3, 0}, {-3, 3, 0, 0 }, {1, 0, 0, 0}};
	double patchx[4][4] = {{a.x, b.x, c.x, d.x}, {e.x, f.x, g.x, h.x}, {i.x, j.x, k.x, l.x}, {m.x, n.x, o.x, p.x}};
	double patchy[4][4] = {{a.y, b.y, c.y, d.y}, {e.y, f.y, g.y, h.y}, {i.y, j.y, k.y, l.y}, {m.y, n.y, o.y, p.y}};
	double patchz[4][4] = {{a.z, b.z, c.z, d.z}, {e.z, f.z, g.z, h.z}, {i.z, j.z, k.z, l.z}, {m.z, n.z, o.z, p.z}};

	double s2 = s*s;
	double s3 = s2*s;
	double t2 = t*t;
	double t3 = t2*t;

	double S[4][4] = {{s3,s2,s,1}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
	double T[4][4] = {{t3,0,0,0}, {t2,1,0,0}, {t,0,1,0}, {1,0,0,1}};

	HMatrix44 mPatchx(patchx), mPatchy(patchy), mPatchz(patchz),mBezier(bezierbasis),mS(S),mT(T),tempA,tempB;

	tempA = mS*mBezier;
	tempB = mBezier*mT;

	HMatrix44 resultx = tempA*mPatchx*tempB;
	HMatrix44 resulty = tempA*mPatchy*tempB;
	HMatrix44 resultz = tempA*mPatchz*tempB;

	pnt.x = resultx(0,0);
	pnt.y = resulty(0,0);
	pnt.z = resultz(0,0);
	return pnt;

}

inline HVector HPatch::calcNormalOnPatch(double s, double t, unsigned nPatch) {
	HVector a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;

	a = patchVectors[nPatch].pv[0];
	b = patchVectors[nPatch].pv[4];
	c = patchVectors[nPatch].pv[8];
	d = patchVectors[nPatch].pv[12];

	e = patchVectors[nPatch].pv[1];
	f = patchVectors[nPatch].pv[5];
	g = patchVectors[nPatch].pv[9];
	h = patchVectors[nPatch].pv[13];

	i = patchVectors[nPatch].pv[2];
	j = patchVectors[nPatch].pv[6];
	k = patchVectors[nPatch].pv[10];
	l = patchVectors[nPatch].pv[14];

	m = patchVectors[nPatch].pv[3];
	n = patchVectors[nPatch].pv[7];
	o = patchVectors[nPatch].pv[11];
	p = patchVectors[nPatch].pv[15];

	double bezierbasis[4][4] = {{ -1,3,-3,1}, {3, -6, 3, 0}, {-3, 3, 0, 0 }, {1, 0, 0, 0}};
	double patchx[4][4] = {{a.x, b.x, c.x, d.x}, {e.x, f.x, g.x, h.x}, {i.x, j.x, k.x, l.x}, {m.x, n.x, o.x, p.x}};
	double patchy[4][4] = {{a.y, b.y, c.y, d.y}, {e.y, f.y, g.y, h.y}, {i.y, j.y, k.y, l.y}, {m.y, n.y, o.y, p.y}};
	double patchz[4][4] = {{a.z, b.z, c.z, d.z}, {e.z, f.z, g.z, h.z}, {i.z, j.z, k.z, l.z}, {m.z, n.z, o.z, p.z}};

	double s2 = s*s;
	double s3 = s2*s;
	double t2 = t*t;
	double t3 = t2*t;

	double S[4][4] = {{s3,s2,s,1}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
	double dS[4][4] = {{3*s2,2*s,1,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
	double T[4][4] = {{t3,0,0,0}, {t2,1,0,0}, {t,0,1,0}, {1,0,0,1}};
	double dT[4][4] = {{3*t2,0,0,0}, {2*t,1,0,0}, {1,0,1,0}, {0,0,0,1}};

	HMatrix44 mPatchx(patchx), mPatchy(patchy), mPatchz(patchz),mBezier(bezierbasis),mS(S),mT(T),
		mdS(dS),mdT(dT),tempAdS,tempBdS,tempAdT,tempBdT;

	tempAdS = mdS*mBezier;
	tempBdS = mBezier*mT;

	tempAdT = mS*mBezier;
	tempBdT = mBezier*mdT;

	HMatrix44 resultxS = tempAdS*mPatchx*tempBdS;
	HMatrix44 resultyS = tempAdS*mPatchy*tempBdS;
	HMatrix44 resultzS = tempAdS*mPatchz*tempBdS;

	HMatrix44 resultxT = tempAdT*mPatchx*tempBdT;
	HMatrix44 resultyT = tempAdT*mPatchy*tempBdT;
	HMatrix44 resultzT = tempAdT*mPatchz*tempBdT;
	
	HVector norm;
	
	norm.x = ((resultyT(0,0)*resultzS(0,0))-(resultyS(0,0)*resultzT(0,0)));
	norm.y = ((resultzT(0,0)*resultxS(0,0))-(resultzS(0,0)*resultxT(0,0)));
	norm.z = ((resultxT(0,0)*resultyS(0,0))-(resultxS(0,0)*resultyT(0,0)));

	norm.normalize();

	return norm;
}


inline HVector2D HPatch::calcUVOn4Point(double s, double t,HVector2D repeat,HPatchVectors2D patchVectors) {
	HVector2D a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
	HVector2D uv;

	a = patchVectors.pv[0];
	b = patchVectors.pv[4];
	c = patchVectors.pv[8];
	d = patchVectors.pv[12];

	e = patchVectors.pv[1];
	f = patchVectors.pv[5];
	g = patchVectors.pv[9];
	h = patchVectors.pv[13];

	i = patchVectors.pv[2];
	j = patchVectors.pv[6];
	k = patchVectors.pv[10];
	l = patchVectors.pv[14];

	m = patchVectors.pv[3];
	n = patchVectors.pv[7];
	o = patchVectors.pv[11];
	p = patchVectors.pv[15];

	HVector2D uv_edgeA,uv_edgeB,uv_edgeC,uv_edgeD;

	HVector2D uv_topleft = a, uv_lowerleft = m, uv_lowerright = p, uv_topright = d;

	uv_edgeB = (s * uv_lowerright) + ((1-s) * uv_lowerleft);
	uv_edgeD = (s * uv_topright) + ((1-s) * uv_topleft);
	HVector2D q = (t * uv_edgeB) + ((1-t) * uv_edgeD);

	uv_edgeA = (t * uv_lowerleft) + ((1-t) * uv_topleft);
	uv_edgeC = (t * uv_lowerright) + ((1-t) * uv_topright);
	HVector2D r = (s * uv_edgeC) + ((1-s) * uv_edgeA);

	uv = (q + r) * 0.5;

	uv.x = uv.x * repeat.x;
	uv.y = uv.y * repeat.y;
	return uv;
}


inline HVector2D HPatch::calcUVOn5Point(double s, double t,unsigned nPatch,
									  HVector2D repeat, HPatchVectors *pvect) {

	int cpid0 = this->cpid1[0];
	int cpid1 = this->cpid1[1];
	int cpid2 = this->cpid1[2];
	int cpid3 = this->cpid1[3];

	HVector2D a,b,c,d,e,mida,midb,midc,midd,mide,midpoint,uv;
	HPatchVectors2D patchVectors;
	patchVectors.pv.resize(16);

	a = HVector2D(pvect->pv[0].x,-pvect->pv[0].y);
	b = HVector2D(pvect->pv[3].x,-pvect->pv[3].y);
	c = HVector2D(pvect->pv[6].x,-pvect->pv[6].y);
	d = HVector2D(pvect->pv[9].x,-pvect->pv[9].y);
	e = HVector2D(pvect->pv[12].x,-pvect->pv[12].y);
	
	mida = (a+b)*0.5;
	midb = (b+c)*0.5;
	midc = (c+d)*0.5;
	midd = (d+e)*0.5;
	mide = (e+a)*0.5;
	midpoint = (a+b+c+d+e)*0.2f;
	
	if (nPatch == 0) {
		patchVectors.pv[0] = HVector2D(midpoint.x,midpoint.y);
		patchVectors.pv[3] = HVector2D(mida.x,mida.y);
		patchVectors.pv[15] = HVector2D(b.x,b.y);
		patchVectors.pv[12] = HVector2D(midb.x,midb.y);
	}
	else if (nPatch == 1) {
		patchVectors.pv[0] = HVector2D(midpoint.x,midpoint.y);
		patchVectors.pv[3] = HVector2D(midb.x,midb.y);
		patchVectors.pv[15] = HVector2D(c.x,c.y);
		patchVectors.pv[12] = HVector2D(midc.x,midc.y);
	}
	else if (nPatch == 2) {
		patchVectors.pv[0] = HVector2D(midpoint.x,midpoint.y);
		patchVectors.pv[3] = HVector2D(midc.x,midc.y);
		patchVectors.pv[15] = HVector2D(d.x,d.y);
		patchVectors.pv[12] = HVector2D(midd.x,midd.y);
	}
	else if (nPatch == 3) {
		patchVectors.pv[0] = HVector2D(midpoint.x,midpoint.y);
		patchVectors.pv[3] = HVector2D(midd.x,midd.y);
		patchVectors.pv[15] = HVector2D(e.x,e.y);
		patchVectors.pv[12] = HVector2D(mide.x,mide.y);
	}				
	else if (nPatch == 4) {
		patchVectors.pv[0] = HVector2D(midpoint.x,midpoint.y);
		patchVectors.pv[3] = HVector2D(mide.x,mide.y);
		patchVectors.pv[15] = HVector2D(a.x,a.y);
		patchVectors.pv[12] = HVector2D(mida.x,mida.y);
	}
	
	uv = calcUVOn4Point(s,t,repeat,patchVectors);
	return uv;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	patch tessellation methods
///////////////////////////////////////////////////////////////////////////////////////////////////

inline float HPatch::getDivisionConstant(const float number) {
	const float onehalf = 0.5f,onethird = 0.333f,onefourth = 0.25f,onefifth = 0.2f,onesixth = 0.167f,
				oneseventh = 0.143f,oneeigth = 0.125f,oneninth = 0.111f,onetenth = 0.1f;
	if (number == 2.0f)
		return onehalf;
	else if (number == 3.0f)
		return onethird;
	else if (number == 4.0f)
		return onefourth;
	else if (number == 5.0f)
		return onefifth;
	else if (number == 6.0f)
		return onesixth;
	else if (number == 7.0f)
		return oneseventh;
	else if (number == 8.0f)
		return oneeigth;
	else if (number == 9.0f)
		return oneninth;
	else if (number == 10.0f)
		return onetenth;
	return (1.0f/number);
}
