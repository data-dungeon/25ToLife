//============================================================================
//= MathPCH.h - Precompiled header for Math project
//============================================================================

#ifndef __MATH_PCH_H
#define __MATH_PCH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "platform/BaseType.h"
#include "Layers/Assert/Assert.h"
#include "Math/MathClass.h"
#include "Math/MathStats.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/BuildView.h"
#include "Math/Quatern.h"
#include "Math/fastrand.h"

#ifdef PS2
#include "platform/CompilerControl.h"
#include "Display/PS2/Renderer/Code/MasterList.h"
#endif //PS2

#endif // __MATH_PCH_H
