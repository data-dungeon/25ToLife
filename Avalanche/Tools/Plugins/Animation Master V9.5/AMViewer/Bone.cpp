//

#include "stdafx.h"
#include "Model.h"

void Bone::ComputeBoneToStandard(const Vector &pivotpos, const Vector &endpos, const Vector &rollpos)
{
   m_bonetostandard = Rotate2V((endpos-pivotpos), Vector( 0, 0, 1 ));
   Vector rollvec = m_bonetostandard * (rollpos-pivotpos);
   float roll = (float)atan2( -rollvec.x, rollvec.y );
   if ( roll )
      m_bonetostandard = ZRotateRadians( -roll ) * m_bonetostandard;
   m_bonetostandard.SetTranslate( m_bonetostandard * -pivotpos );
}

Matrix33 Bone::ComputeParentToBone()
{
   if ( m_parent )
      return ToMatrix33( m_bonetostandard ) *
             ToMatrix33( m_parent->m_bonetostandard ).Transpose();
   else
      return Identity33();
}

Matrix34 Bone::CreateMatrix( const TSQ &tsq )
{
   Matrix34 actionmatrix( tsq.quat );
   MatrixTimesScale( actionmatrix, tsq.scale );
   actionmatrix.SetTranslate( tsq.translate );
   return m_standardtoaction * actionmatrix * m_bonetostandard;
}

void Bone::ComputeStandardToAction(Matrix33 &bonetoparent)
{
   m_standardtoaction = m_bonetostandard.InvertRotateTranslate();
   m_standardtoaction *= ToMatrix34( ComputeParentToBone() * bonetoparent );
}

