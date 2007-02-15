//////////////////////////////////////////////////////////////////////////////////////////
//
// INLINE SPLINE SEGMENT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////

inline HSplineSegment::HSplineSegment() {
	v0 = 0;
	v1 = 0;
	cv0 = 0;
	cv1 = 0;
	memset(&flags,0,sizeof(HSplineSegment::segflags));
}

inline HSplineSegment::HSplineSegment(const HSplineSegment &other) {
	v0 = other.v0;
	v1 = other.v1;
	cv0 = other.cv0;
	cv1 = other.cv1;
	adj_patchIndicies = other.adj_patchIndicies;
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
}

// default - tangents not finalized
inline HSplineSegment::HSplineSegment(const HVector &sv0, const HVector &scv0, const HVector &scv1, const HVector &sv1) {
	v0 = sv0;
	cv0 = scv0;
	cv1 = scv1;
	v1 = sv1;
	memset(&flags,0,sizeof(HSplineSegment::segflags));
}

// optional - flag whether tangents are finalized
inline HSplineSegment::HSplineSegment(const HVector &sv0, const HVector &scv0, const HVector &scv1, const HVector &sv1, bool finaltangents) {
	v0 = sv0;
	cv0 = scv0;
	cv1 = scv1;
	v1 = sv1;
	memset(&flags,0,sizeof(HSplineSegment::segflags));
	if (finaltangents)
		flags.kIsFinalized = 1;
}

inline HSplineSegment::~HSplineSegment(){}

inline void HSplineSegment::reverse() {
	HSplineSegment temp;
	if (!this->flags.kIsFinalized) {
		temp = *this;
		v0 = temp.v1;
		v1 = temp.v0;
		cv0 = temp.cv1;
		cv1 = temp.cv0;
		cv0.negate();
		cv1.negate();
	}
}

inline void HSplineSegment::flipTangents() {
	HSplineSegment temp = *this;
	if (!this->flags.kIsFinalized) {
		cv0 = temp.cv1;
		cv1 = temp.cv0;
	}
}

inline HSplineSegment &HSplineSegment::operator = ( const HSplineSegment &other ) {
	v0 = other.v0;
	v1 = other.v1;
	cv0 = other.cv0;
	cv1 = other.cv1;
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	adj_patchIndicies = other.adj_patchIndicies;
	return *this;
}

/*
inline bool HSplineSegment::isStraight(double tol) {
	HSplineSegment seg = *this;

	if (!this->flags.kIsFinalized)
		seg.finalize();

	HVector a = seg.cv0 - seg.v0;
	HVector b = seg.v1 - seg.v0;

	a.Normalize();
	b.Normalize();
	
	double dif = a.Dot(b);

	if (dif > 1.0-tol)
	{
		HVector c = seg.v1 - seg.cv1;
		c.Normalize();
		dif = b.Dot(c);
		if (dif > 1.0-tol) 
			return true;
	}
	return false;
}
*/

inline bool HSplineSegment::isStraight(double tol) {
	HSplineSegment seg = *this;

	if (!this->flags.kIsFinalized)
		seg.finalize();

	HVector a = seg.cv0 - seg.v0;
	HVector b = seg.v1 - seg.v0;
	b.normalize();
	HVector c = seg.cv1 - seg.v1;
	
	HVector dif = a^b;

	if (dif.length() < tol){
		dif = b^c;
		if (dif.length() < tol) 
			return true;
	}
	return false;
}

inline HVector HSplineSegment::getPointAtParam(double t) {
	HSplineSegment seg(this->v0,this->cv0,this->cv1,this->v1);

	if (!this->flags.kIsFinalized)
		seg.finalize();

	HVector p;
	p.x = (double)HSpline::bezier(t,seg.v0.x,seg.cv0.x,seg.cv1.x,seg.v1.x);
	p.y = (double)HSpline::bezier(t,seg.v0.y,seg.cv0.y,seg.cv1.y,seg.v1.y);
	p.z = (double)HSpline::bezier(t,seg.v0.z,seg.cv0.z,seg.cv1.z,seg.v1.z);
	return p;
}

inline HVector HSplineSegment::getDerivativeAtParam(double t) {
	HSplineSegment seg(this->v0,this->cv0,this->cv1,this->v1);

	if (!this->flags.kIsFinalized)
		seg.finalize();

	HVector p;
	p.x = (double)HSpline::derivative(t,seg.v0.x,seg.cv0.x,seg.cv1.x,seg.v1.x);
	p.y = (double)HSpline::derivative(t,seg.v0.y,seg.cv0.y,seg.cv1.y,seg.v1.y);
	p.z = (double)HSpline::derivative(t,seg.v0.z,seg.cv0.z,seg.cv1.z,seg.v1.z);
	return p;
}

inline void HSplineSegment::finalize() {
	if (this->flags.kHasNoLength)
		return;

	if (!this->flags.kIsFinalized) {
		cv0 = v0 + (cv0/3.0);
		cv1 = v1 - (cv1/3.0);
		this->flags.kIsFinalized = 1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// the result is two non finalized segments
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSplineSegment::splitSplineSegment(HSplineSegment &seg1,HSplineSegment &seg2) {
	HVector c0,c1,c2,c3;
	HVector L0,L1,L2,L3,R0,R1,R2,R3,T;
	HSplineSegment sseg = (*this);

	if (!this->flags.kIsFinalized)
		sseg.finalize();

	if (sseg.flags.kBorderSegment) {
		seg1.flags.kBorderSegment = 1;
		seg2.flags.kBorderSegment = 1;
	}

	c0 = sseg.v0; c1 = sseg.cv0; c2 = sseg.cv1; c3 = sseg.v1;

	L0 = c0;
	R3 = c3;
	L1 = c0 + 0.5 * (c1-c0);
	R2 = c2 + 0.5 * (c3-c2);
	T = c1 + 0.5 * (c2-c1);
	L2 = L1 + 0.5 * (T-L1);
	R1 = T + 0.5 * (R2-T);
	L3 = L2 + 0.5 * (R1-L2);
	R0 = L3;

	HVector tangent = (R1-L2) * 0.5;
	HVector midpoint = sseg.getPointAtParam(0.5);

	seg1.v0 = L0;
	seg1.cv0 = (L1 - L0) * 3.0;
	seg1.cv1 = (L3 - L2) * 3.0;
	seg1.v1 = L3;

	seg2.v0 = R0;
	seg2.cv0 = (R1 - R0) * 3.0;
	seg2.cv1 = (R3 - R2) * 3.0;
	seg2.v1 = R3;
}

inline HSplineSegment &HSplineSegment::operator = ( const HVector &other ) {
	v0 = other;
	v1 = other;
	cv0 = other;
	cv1 = other;
	memset(&flags,0,sizeof(HSplineSegment::segflags));
	return *this;
}

inline void HSplineSegment::doSubdivide(double tol,HDoubleArray &percents) {
	this->subdivide(tol,0.0,1.0,percents);
	percents.append(1.0);
	percents.sort();
}

inline void HSplineSegment::subdivide(double tol,double start,double end,HDoubleArray &percents) {
	HSplineSegment seg = *this,segA,segB;

	if (!this->flags.kIsFinalized)
		seg.finalize();

	if (!seg.isStraight(tol)) {
		seg.splitSplineSegment(segA,segB);
		double mid = (start + end) * 0.5;
		segA.subdivide(tol,start,mid,percents);
		segB.subdivide(tol,mid,end,percents);
	}
	else 
		percents.append(start);
}
