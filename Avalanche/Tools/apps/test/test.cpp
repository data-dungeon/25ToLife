
#include <mathlib.h>

	l_Vct3f		VxV1;
	l_Vct3f		VxV2;
	l_Vct3f		VxV3;
	l_Vct3f		VxV4;
	l_Vct3f		VxV5;

void main( void)
{
	l_Vct3f		V = { 0.0, 1.0, 0.0 };
	l_Vct3f		V1 = { 0.5,  .5, 0.0 };
	l_Vct3f		V2 = {  .5, -.5, 0.0 };
	l_Vct3f		V3 = { -.5, -.5, 0.0 };
	l_Vct3f		V4 = { -.5,  .5, 0.0 };
	l_Vct3f		V5 = { 0.0, -1.0, 0.0 };

	vctCross3f( &V, &V1, &VxV1);
	vctCross3f( &V, &V2, &VxV2);
	vctCross3f( &V, &V3, &VxV3);
	vctCross3f( &V, &V4, &VxV4);
	vctCross3f( &V, &V5, &VxV5);

}