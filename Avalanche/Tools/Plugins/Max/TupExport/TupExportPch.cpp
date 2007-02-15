////////////////////////////////////////////////////////////////////////////
//
// TupExportPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupExportPch.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 9:49a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Removed xref update options
 * Added exporter data & time as well as database version to export
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:39p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Start of standard material exporting
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Level A  complete
 */

#include "TupExportPch.h"											

const char *g_pExporterDate = __DATE__;
const char *g_pExporterTime = __TIME__;
