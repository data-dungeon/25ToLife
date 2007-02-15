
// BezierBasisMatrix

inline DynamicMatrix *BezierBasisMatrix::GetMatrix()
{
   return &m_mat;
}

// VectorBezierBasisMatrix

inline VectorBezierBasisMatrix::VectorBezierBasisMatrix()
{
   memset( m_cvs, 0, sizeof(Vector)*16 );
}

inline VectorBezierBasisMatrix &VectorBezierBasisMatrix::operator = ( const VectorBezierBasisMatrix &other )
{
   memcpy( m_cvs, other.m_cvs, sizeof(m_cvs) );
   m_mat = other.m_mat;
   return *this;
}

inline Vector *VectorBezierBasisMatrix::GetCVs()
{
   return m_cvs;
}

inline void VectorBezierBasisMatrix::SetCVs(Vector *pCVs)
{
   memcpy( m_cvs, pCVs, sizeof(Vector)*16 );
}

inline void VectorBezierBasisMatrix::CalculateInteriorTangents()
{
   Vector *pCVs = GetCVs();

   float twothirds = (2.0f/3.0f);
   float onethird = (1.0f/3.0f);

   Vector A1,A2,B1,B2,C1,C2,D1,D2;

   A1 = pCVs[4] - pCVs[0];
   A2 = pCVs[8] - pCVs[12];
   B1 = pCVs[13] - pCVs[12];
   B2 = pCVs[14] - pCVs[15];
   C1 = pCVs[11] - pCVs[15];
   C2 = pCVs[7] - pCVs[3];
   D1 = pCVs[2] - pCVs[3];
   D2 = pCVs[1] - pCVs[0];

   pCVs[5] = (pCVs[0] + twothirds*A1 + onethird*C2 + twothirds*D2 + onethird*B1);
   pCVs[9] = (pCVs[12] + twothirds*A2 + onethird*C1 + twothirds*B1 + onethird*D2);
   pCVs[10] = (pCVs[15] + twothirds*C1 + onethird*A2 + twothirds*B2 + onethird*D1);
   pCVs[6] = (pCVs[3] + twothirds*C2 + onethird*A1 + twothirds*D1 + onethird*B2);
}

// Vector2BezierBasisMatrix

inline Vector2BezierBasisMatrix::Vector2BezierBasisMatrix()
{
   memset( m_cvs, 0, sizeof(Vector2)*16 );
}

inline Vector2BezierBasisMatrix &Vector2BezierBasisMatrix::operator = ( const Vector2BezierBasisMatrix &other )
{
   memcpy( m_cvs, other.m_cvs, sizeof(m_cvs) );
   m_mat = other.m_mat;
   return *this;
}

inline Vector2 *Vector2BezierBasisMatrix::GetCVs()
{
   return m_cvs;
}

inline void Vector2BezierBasisMatrix::SetCVs(Vector2 *pCVs)
{
   memcpy( m_cvs, pCVs, sizeof(Vector2)*16 );
}

inline void Vector2BezierBasisMatrix::CalculateInteriorTangents()
{
   Vector2 *pCVs = GetCVs();

   float twothirds = (2.0f/3.0f);
   float onethird = (1.0f/3.0f);

   Vector2 A1,A2,B1,B2,C1,C2,D1,D2;

   A1 = pCVs[4] - pCVs[0];
   A2 = pCVs[8] - pCVs[12];
   B1 = pCVs[13] - pCVs[12];
   B2 = pCVs[14] - pCVs[15];
   C1 = pCVs[11] - pCVs[15];
   C2 = pCVs[7] - pCVs[3];
   D1 = pCVs[2] - pCVs[3];
   D2 = pCVs[1] - pCVs[0];

   pCVs[5] = (pCVs[0] + twothirds*A1 + onethird*C2 + twothirds*D2 + onethird*B1);
   pCVs[9] = (pCVs[12] + twothirds*A2 + onethird*C1 + twothirds*B1 + onethird*D2);
   pCVs[10] = (pCVs[15] + twothirds*C1 + onethird*A2 + twothirds*B2 + onethird*D1);
   pCVs[6] = (pCVs[3] + twothirds*C2 + onethird*A1 + twothirds*D1 + onethird*B2);
}

// Vector4BezierBasisMatrix

inline Vector4BezierBasisMatrix::Vector4BezierBasisMatrix()
{
   memset( m_cvs, 0, sizeof(Vector4)*16 );
}

inline Vector4BezierBasisMatrix &Vector4BezierBasisMatrix::operator = ( const Vector4BezierBasisMatrix &other )
{
   memcpy( m_cvs, other.m_cvs, sizeof(m_cvs) );
   m_mat = other.m_mat;
   return *this;
}

inline Vector4 *Vector4BezierBasisMatrix::GetCVs()
{
   return m_cvs;
}

inline void Vector4BezierBasisMatrix::SetCVs(Vector4 *pCVs)
{
   memcpy( m_cvs, pCVs, sizeof(Vector4)*16 );
}

inline void Vector4BezierBasisMatrix::CalculateInteriorTangents()
{
   Vector4 *pCVs = GetCVs();

   float twothirds = (2.0f/3.0f);
   float onethird = (1.0f/3.0f);

   Vector4 A1,A2,B1,B2,C1,C2,D1,D2;

   A1 = pCVs[4] - pCVs[0];
   A2 = pCVs[8] - pCVs[12];
   B1 = pCVs[13] - pCVs[12];
   B2 = pCVs[14] - pCVs[15];
   C1 = pCVs[11] - pCVs[15];
   C2 = pCVs[7] - pCVs[3];
   D1 = pCVs[2] - pCVs[3];
   D2 = pCVs[1] - pCVs[0];

   pCVs[5] = (pCVs[0] + twothirds*A1 + onethird*C2 + twothirds*D2 + onethird*B1);
   pCVs[9] = (pCVs[12] + twothirds*A2 + onethird*C1 + twothirds*B1 + onethird*D2);
   pCVs[10] = (pCVs[15] + twothirds*C1 + onethird*A2 + twothirds*B2 + onethird*D1);
   pCVs[6] = (pCVs[3] + twothirds*C2 + onethird*A1 + twothirds*D1 + onethird*B2);
}

////////////////////////////////////////////////////////////////////////////////////

inline void PropertyPatch::Init()
{
   m_pTessPropertyPolyArray = NULL;
   m_nNumTessPropertyPolys = 0;
   m_pInternalTessPropertyEdges = NULL;
   m_pBezierBasisArray = NULL;
   m_bDirtyTangents = FALSE;
}

inline PropertyPatch::PropertyPatch()
{
   Init();
}

inline PropertyPatch::PropertyPatch( UINT nID )
: PropertyPoly( nID )
{
   Init();
}

inline void PropertyPatch::SetNumBasisMatrices( UINT nCount, BezierBasisMatrixType nType )
{
   ClearBezierBasisMatrices();
   if (nCount == 0)
      return;
   m_pBezierBasisArray = SNEW BezierBasisMatrix*[nCount];
   for (UINT i = 0; i < nCount; i++)
   {
      if (nType == BezierBasisMatrixType::VECTOR2BEZIERBASIS)
         m_pBezierBasisArray[i] = SNEW Vector2BezierBasisMatrix;
      else if (nType == BezierBasisMatrixType::VECTOR4BEZIERBASIS)
         m_pBezierBasisArray[i] = SNEW Vector4BezierBasisMatrix;
      else 
         m_pBezierBasisArray[i] = SNEW VectorBezierBasisMatrix;
   }
}