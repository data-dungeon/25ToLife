
inline UVBezierBasisMatrix::UVBezierBasisMatrix()
{
   memset( m_uvcvs, 0, sizeof(Vector2)*16 );
}

inline UVBezierBasisMatrix &UVBezierBasisMatrix::operator = ( const UVBezierBasisMatrix &other )
{
   memcpy( m_uvcvs, other.m_uvcvs, sizeof(m_uvcvs) );
   m_uvmat = other.m_uvmat;
   return *this;
}

inline Vector2 *UVBezierBasisMatrix::GetUVCVs()
{
   return m_uvcvs;
}

inline void UVBezierBasisMatrix::SetUVCVs(Vector2 *pUVCVs)
{
   memcpy( m_uvcvs, pUVCVs, sizeof(Vector2)*16 );
}

inline DynamicMatrix *UVBezierBasisMatrix::GetUVMatrix()
{
   return &m_uvmat;
}

inline void UVBezierBasisMatrix::CalculateInteriorTangents()
{
   Vector2 *pCVs = GetUVCVs();

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

////////////////////////////////////////////////////////////////////////////////////

inline void UVPatch::Init()
{
   m_pTessUVPolyArray = NULL;
   m_nNumTessUVPolys = 0;
}

inline UVPatch::UVPatch()
{
   Init();
}

inline UVPatch::UVPatch( UINT nID )
: UVPoly( nID )
{
   Init();
}
