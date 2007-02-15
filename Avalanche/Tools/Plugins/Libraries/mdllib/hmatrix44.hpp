/////////////////////////////////////////////////////////////////////////////////////////
// HMATRIX44 INLINE METHODS
/////////////////////////////////////////////////////////////////////////////////////////

inline HMatrix44::HMatrix44(){
	type = COLUMN_MAJOR;
	memset(matrix,0,sizeof(double)*16);
}

inline HMatrix44::HMatrix44(mattype t) {
	type = t;
	memset(matrix,0,sizeof(double)*16);
}

inline HMatrix44::HMatrix44(const double m[4][4]) {
	type = COLUMN_MAJOR;
	memcpy(matrix,m,sizeof(double)*16);
}

inline HMatrix44::HMatrix44(const double m[4][4],mattype t) {
	type = t;
	memcpy(matrix,m,sizeof(double)*16);
}

inline HMatrix44::HMatrix44(const HMatrix44 &other){
	type = other.type;
	memcpy(matrix,other.matrix,sizeof(double)*16);
}

inline HMatrix44::~HMatrix44(){}

inline HMatrix44 &HMatrix44::operator = (const HMatrix44 &other) {
	type = other.type;
	memcpy(matrix,other.matrix,sizeof(double)*16);
	return *this;
}

inline HMatrix44 &HMatrix44::operator += (const HMatrix44 &other) {
	matrix[0][0] += other.matrix[0][0];
	matrix[0][1] += other.matrix[0][1];
	matrix[0][2] += other.matrix[0][2];
	matrix[0][3] += other.matrix[0][3];

	matrix[1][0] += other.matrix[1][0];
	matrix[1][1] += other.matrix[1][1];
	matrix[1][2] += other.matrix[1][2];
	matrix[1][3] += other.matrix[1][3];

	matrix[2][0] += other.matrix[2][0];
	matrix[2][1] += other.matrix[2][1];
	matrix[2][2] += other.matrix[2][2];
	matrix[2][3] += other.matrix[2][3];

	matrix[3][0] += other.matrix[3][0];
	matrix[3][1] += other.matrix[3][1];
	matrix[3][2] += other.matrix[3][2];
	matrix[3][3] += other.matrix[3][3];
	return *this;
}

inline HMatrix44 &HMatrix44::operator -= ( const HMatrix44 &other ) {
	matrix[0][0] -= other.matrix[0][0];
	matrix[0][1] -= other.matrix[0][1];
	matrix[0][2] -= other.matrix[0][2];
	matrix[0][3] -= other.matrix[0][3];

	matrix[1][0] -= other.matrix[1][0];
	matrix[1][1] -= other.matrix[1][1];
	matrix[1][2] -= other.matrix[1][2];
	matrix[1][3] -= other.matrix[1][3];

	matrix[2][0] -= other.matrix[2][0];
	matrix[2][1] -= other.matrix[2][1];
	matrix[2][2] -= other.matrix[2][2];
	matrix[2][3] -= other.matrix[2][3];

	matrix[3][0] -= other.matrix[3][0];
	matrix[3][1] -= other.matrix[3][1];
	matrix[3][2] -= other.matrix[3][2];
	matrix[3][3] -= other.matrix[3][3];
	return *this;
}

inline HMatrix44 &HMatrix44::operator *= ( const HMatrix44 &other ) {
	HMatrix44 c;
  // For speed reasons, use the unroll-loops option of your compiler
	for(unsigned i = 0; i < 4; i++)
		for(unsigned j = 0; j < 4; j++)
			for(unsigned k = 0; k < 4; k++)
				c.matrix[i][j] += matrix[i][k] * other.matrix[k][j];
	*this = c;
	return *this; 
}

inline HMatrix44 &HMatrix44::operator *= ( double scale ) {
	matrix[0][0] *= scale;
	matrix[0][1] *= scale;
	matrix[0][2] *= scale;
	matrix[0][3] *= scale;

	matrix[1][0] *= scale;
	matrix[1][1] *= scale;
	matrix[1][2] *= scale;
	matrix[1][3] *= scale;

	matrix[2][0] *= scale;
	matrix[2][1] *= scale;
	matrix[2][2] *= scale;
	matrix[2][3] *= scale;

	matrix[3][0] *= scale;
	matrix[3][1] *= scale;
	matrix[3][2] *= scale;
	matrix[3][3] *= scale;
	return *this; 
}

inline void HMatrix44::transpose() {
	HMatrix44 c;
	for(unsigned i = 0; i < 4; i++)
		for(unsigned j = 0; j < 4; j++)
				c.matrix[i][j] = matrix[j][i];
	*this = c;
}

inline HMatrix44 HMatrix44::operator + ( const HMatrix44 &other ) const {
	return HMatrix44(*this) += other;
}

inline HMatrix44 HMatrix44::operator - ( const HMatrix44 &other ) const {
	return HMatrix44(*this) -= other;
}

inline HMatrix44 HMatrix44::operator * ( const HMatrix44 &other ) const {
	return HMatrix44(*this) *= other;
}

inline HMatrix44 HMatrix44::operator * ( double scale ) const {
	return HMatrix44(*this) *= scale;
}

inline bool HMatrix44::operator == ( const HMatrix44 &other ) const {
	int i;
	i = memcmp(&matrix,&other.matrix,sizeof(double)*16);
	if (i == 1)
		return true;
	return false;
}

inline bool HMatrix44::operator == ( double other ) const {
	int i;
	double temp[4][4];
	memset(&temp,(int)other,sizeof(double)*16);
	i = memcmp(&matrix,&temp,sizeof(double)*16);
	if (i == 1)
		return true;
	return false;;
}

inline bool HMatrix44::operator != ( const HMatrix44 &other ) const {
	return !(*this == other);
}

inline double &HMatrix44::operator()(unsigned row, unsigned col ){
	return matrix[row][col];
}

inline double HMatrix44::operator()(unsigned row, unsigned col ) const{
	return matrix[row][col];
}

inline void HMatrix44::rotateX(const double angle) {
	double sine = sin(angle),cosine = cos(angle);
	double xrotmat[4][4] = {{1, 0, 0, 0}, {0, cosine, -sine, 0}, {0, sine, cosine ,0 }, {0, 0, 0, 1}};
	HMatrix44 temp(xrotmat);
	if (this->type == COLUMN_MAJOR) {
		temp.transpose();
		(*this) = temp * (*this);
	}
	else
		(*this) *= temp; 
}

inline void HMatrix44::rotateY(const double angle) {
	double sine = sin(angle),cosine = cos(angle);
	double yrotmat[4][4] = {{ cosine, 0, sine, 0}, {0, 1, 0, 0}, {-sine ,0, cosine, 0 }, {0, 0, 0, 1}};
	HMatrix44 temp(yrotmat);
	if (this->type == COLUMN_MAJOR) {
		temp.transpose();
		(*this) = temp * (*this);
	}
	else
		(*this) *= temp;
}

inline void HMatrix44::rotateZ(const double angle) {
	double sine = sin(angle),cosine = cos(angle);
	double zrotmat[4][4] = {{ cosine, -sine, 0, 0}, {sine, cosine, 0, 0}, {0, 0, 1, 0 }, {0, 0, 0, 1}};
	HMatrix44 temp(zrotmat);
	if (this->type == COLUMN_MAJOR) {
		temp.transpose();
		(*this) = temp * (*this);
	}
	else
		(*this) *= temp;
}

inline void HMatrix44::translate(const HVector &t) {
	double tmat[4][4] = {{ 1, 0, 0, t.x }, {0, 1, 0, t.y }, { 0, 0, 1, t.z }, { 0, 0, 0, 1 }};
	HMatrix44 temp(tmat);
	if (this->type == COLUMN_MAJOR) {
		temp.transpose();
		(*this) = temp * (*this);
	}
	else
		(*this) *= temp;
}

inline void HMatrix44::scale(const HVector &s) {
	double smat[4][4] = {{ s.x, 0, 0, 0 }, { 0, s.y, 0, 0 }, { 0, 0, s.z, 0 }, { 0, 0, 0, 1 }};
	(*this) *= smat;
}

inline void HMatrix44::setIdentity() {
	double identity[4][4] = {{ 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 }};
	(*this) = identity;
}

inline void HMatrix44::setFloatMatrix(float m[4][4]) {
	for (unsigned i = 0; i < 4; i++)
		for (unsigned j = 0; j < 4; j++)
			m[i][j] = (float) this->matrix[i][j];
}