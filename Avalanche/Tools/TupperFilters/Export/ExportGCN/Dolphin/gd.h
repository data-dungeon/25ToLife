/*---------------------------------------------------------------------------*
  Project:  Dolphin GD library
  File:     gd.h

  Copyright 2001 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/tools/TupperFilters/ExportGCN/Dolphin/gd.h $
 * 
 * 1     6/20/03 4:19p Todd Blackburn
    
    2     9/19/01 3:55p Carl
    More win32 compatibility defines.
    
    1     9/12/01 1:53p Carl
    Initial revision of GD: Graphics Display List Library.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __GD_H__
#define __GD_H__

/*---------------------------------------------------------------------------*/

#ifdef WIN32
#define inline __inline
#endif

/*---------------------------------------------------------------------------*/

#include <dolphin/gd/GDBase.h>
#include <dolphin/gd/GDFile.h>
#include <dolphin/gd/GDGeometry.h>
#include <dolphin/gd/GDLight.h>
#include <dolphin/gd/GDPixel.h>
#include <dolphin/gd/GDTev.h>
#include <dolphin/gd/GDTexture.h>
#include <dolphin/gd/GDTransform.h>
#include <dolphin/gd/GDVert.h>

/*---------------------------------------------------------------------------*/

#endif // __GD_H__
