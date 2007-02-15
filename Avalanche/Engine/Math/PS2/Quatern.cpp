
#if !defined(PS2) || !defined(SN)
#error This is SN PS2 only!
#endif

#ifdef EXCLUDE_GENERIC_QUATERNION_ADD
void qatAdd(const ts_Quatvw *pA, const ts_Quatvw *pB, ts_Quatvw *pResult)
{
	asm("vadd %0, %1, %2" : "=j" (pResult->v128) : "j" (pA->v128), "j" (pB->v128));
}
#endif

#ifdef EXCLUDE_GENERIC_QUATERNION_SUBTRACT
void qatSub(const ts_Quatvw *pA, const ts_Quatvw *pB, ts_Quatvw *pResult)
{
	asm("vsub %0, %1, %2" : "=j" (pResult->v128) : "j" (pA->v128), "j" (pB->v128));
}
#endif

#ifdef EXCLUDE_GENERIC_QUATERNION_TO_MATRIX
void qatToRotMat(const ts_Quatvw *pQuat, Cartesian4x4 *pMatrix)
{
	qatNormalize((ts_Quatvw*) pQuat);

#ifdef __VCPRECOMPILE // SN preprocess
	return 0.0f;
#else
	register u128 quat_in	asm("vf1");
	register u128 mtx_out_0	asm("vf1");
	register u128 mtx_out_1	asm("vf2");
	register u128 mtx_out_2	asm("vf3");
	register u128 mtx_out_3	asm("vf4");

	quat_in = pQuat->v128;

	// this asm needs to be one block to keep the interlock from getting
	// a non-interlocking instruction before it.
	asm("
		vcallms		PS2_Quat_To_Mtx
		qmfc2.i		%0,	  %1" :
		"=j" (mtx_out_0), "=j" (mtx_out_1), "=j" (mtx_out_2), "=j" (mtx_out_3) :
		"j" (quat_in));

	pMatrix->row0 = mtx_out_0;
	pMatrix->row1 = mtx_out_1;
	pMatrix->row2 = mtx_out_2;
	pMatrix->row3 = mtx_out_3;
#endif
}
#endif


