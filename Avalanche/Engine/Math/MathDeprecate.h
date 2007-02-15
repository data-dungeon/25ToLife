/***************************************************************************/
// the basic c lib functions should not be used, so make them spout errors
// if they are used.
/***************************************************************************/

#ifdef M_MATH_H

#define sin sin__DEPRECATED__use_Math_class
#define cos cos__DEPRECATED__use_Math_class
#define tan tan__DEPRECATED__use_Math_class

#define sinf sinf__DEPRECATED__use_Math_class
#define cosf cosf__DEPRECATED__use_Math_class
#define tanf tanf__DEPRECATED__use_Math_class

#define asin  asin__DEPRECATED__use_Math_class
#define acos  acos__DEPRECATED__use_Math_class
#define atan  atan__DEPRECATED__use_Math_class
#define atan2 atan2__DEPRECATED__use_Math_class

#define asinf  asinf__DEPRECATED__use_Math_class
#define acosf  acosf__DEPRECATED__use_Math_class
#define atanf  atanf__DEPRECATED__use_Math_class
#define atan2f atan2f__DEPRECATED__use_Math_class

#define sinh sinh__DEPRECATED__use_Math_class
#define cosh cosh__DEPRECATED__use_Math_class
#define tanh tanh__DEPRECATED__use_Math_class

#define sinhf sinhf__DEPRECATED__use_Math_class
#define coshf coshf__DEPRECATED__use_Math_class
#define tanhf tanhf__DEPRECATED__use_Math_class

#define sqrt  sqrt__DEPRECATED__use_Math_class
#define sqrtf sqrtf__DEPRECATED__use_Math_class

#define fabs  fabs__DEPRECATED__use_Math_class
#define floor floor__DEPRECATED__use_Math_class
#define ceil  ceil__DEPRECATED__use_Math_class
#define mod   mod__DEPRECATED__use_Math_class
#define fmod  fmod__DEPRECATED__use_Math_class

#define fabsf  fabsf__DEPRECATED__use_Math_class
#define floorf floorf__DEPRECATED__use_Math_class
#define ceilf  ceilf__DEPRECATED__use_Math_class
#define fmodf  fmodf__DEPRECATED__use_Math_class

#define pow   pow__DEPRECATED__use_Math_class
#define exp   exp__DEPRECATED__use_Math_class
#define log   log__DEPRECATED__use_Math_class
#define log10 log10__DEPRECATED__use_Math_class

#define powf   powf__DEPRECATED__use_Math_class
#define expf   expf__DEPRECATED__use_Math_class
#define logf   logf__DEPRECATED__use_Math_class
#define log10f log10f__DEPRECATED__use_Math_class

#endif
