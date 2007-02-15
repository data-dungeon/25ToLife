#pragma once
#include "AString.h"
#include "HProject.h"

#undef THROW_FATAL_ERROR
#define THROW_FATAL_ERROR(MSG)			\
   {										      \
   throw (String)"<E>" + (String)MSG;  \
   }

#undef RETHROW_FATAL_ERROR
#define RETHROW_FATAL_ERROR(MSG)			\
   {										      \
   throw (String)MSG;  \
   }

#undef THROW_HELP_MSG
#define THROW_HELP_MSG(MSG)			\
   {										      \
   throw (String)MSG;		            \
   }

void DisplayWarning(String msg);
void DisplayMessage(String msg);

#undef virt_base
#define virt_base

#undef catch_msg
#define catch_msg
