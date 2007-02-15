////////////////////////////////////////////////////////////////////////////
//
// TupperAttribPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupperAttribPch_SN.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Dross        Date: 11/20/02   Time: 12:26p
 * Created in $/Avalanche/tools/TupperAttrib
 * 
 * *****************  Version 3  *****************
 * User: Tyler Colbert Date: 10/31/02   Time: 11:23a
 * Updated in $/Avalanche/gamelibnew/TupperAttrib
 * Changed PS2 builds to compile all source files together in the PCH.cpp
 * file.  This aids compilation speed
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

#include "TupperAttribPch.h"


/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */
#if defined(PS2) && defined(SN)
#include "ConfigOverride.cpp"
#include "ConfigBase.cpp"
#include "ConfigCheckBox.cpp"
#include "ConfigCheckList.cpp"
#include "ConfigComboBox.cpp"
#include "ConfigEditBox.cpp"
#include "ConfigFreeColorBox.cpp"
#include "ConfigNumeric.cpp"
#include "ConfigRadioGroup.cpp"
#include "ConfigRoot.cpp"
#include "ConfigSelection.cpp"
#include "ConfigSetColorBox.cpp"
#include "KeyValue.cpp"
#include "KeySelection.cpp"
#include "KeyValueContext.cpp"
#include "AccessInstance.cpp"
#include "AccessPath.cpp"
#include "AccessRoot.cpp"
#endif // defined(PS2) && defined(SN)
