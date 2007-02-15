/***************************************************************************/
// the basic c lib functions should not be used, so make them spout errors
// if they are used.
/***************************************************************************/

#ifdef M_MATH_H

#undef sin
#undef cos
#undef tan

#undef sinf
#undef cosf
#undef tanf

#undef asin
#undef acos
#undef atan
#undef atan2

#undef asinf
#undef acosf
#undef atanf
#undef atan2f

#undef sinh
#undef cosh
#undef tanh

#undef sinhf
#undef coshf
#undef tanhf

#undef sqrt
#undef sqrtf

#undef fabs
#undef floor
#undef ceil
#undef mod
#undef fmod

#undef fabsf
#undef floorf
#undef ceilf
#undef fmodf

#undef pow
#undef exp
#undef log
#undef log10

#undef powf
#undef expf
#undef logf
#undef log10f

#endif
