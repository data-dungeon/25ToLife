/*
**
**  File:   MacroChain.h
**  Date:   April 19, 2001
**  By:     Bryant Collard
**  Desc:   Applies dynamics to chained bones in a model.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/MacroChain.cpp $
**  $Revision: 13 $
**      $Date: 11/21/02 12:25p $
**    $Author: Dross $
**
*/

/******************* includes ****************************/

#include "stage/StagePCH.h"





/**********************************************************************
 *
 *  Class:       CMacroChain
 *  Function:    Initialize
 *  Access:      public
 *  Description: Constructs a chain, given a name, from an actor.
 *  Arguments:
 *      i_geom  - The geometry
 *      i_name  - The name.
 *  Return:      true for success, false for failure.
 *
 **********************************************************************/
bool CMacroChain::Initialize(CInstance* i_inst, char* i_name,
        Graphics4x4* i_modelToWorld)
{
/*
 *  Set the name of the chain and determine the maximum possible number of
 *  links.
 */
    if (!SetName(i_name))
        return(InitializeFail());
    int len = MAX_BONENAME_LEN - strlen(i_name);
    int maxLinks = 1;
    while (len > 0)
    {
        if (maxLinks > (MAXINT / 10))
        {
            maxLinks = MAXINT;
            break;
        }
        maxLinks *= 10;
        len--;
    }
/*
 *  Find the the body and the first bone. Loop through each bone just
 *  counting chain members.
 */
    int i;
    ts_Body* body = i_inst->Body();
    ts_Bone* bone;
    char name[MAX_BONENAME_LEN + 1];
    int numLinks;
    for (numLinks = 1; numLinks < maxLinks; numLinks++)
    {
        sprintf(name, "%s%d", i_name, numLinks);
        if (!boneGetBone(body, &bone, name))
            break;
    }
    if (!SetNumLinks(numLinks))
        return(InitializeFail());
/*
 *  Get pointers to each float link (all but the first one) and to any control
 *  bones.
 */
    for (i = 1; i < numLinks; i++)
    {
        sprintf(name, "%s%d", i_name, i);
        boneGetBone(body, &bone, name);
        if (!InitLink(i, bone))
            return(InitializeFail());
    }

    if (i_modelToWorld == NULL)
        i_modelToWorld = &(i_inst->Matrix());
    if (!CompleteInit(i_inst, i_modelToWorld))
        return(InitializeFail());

    return(true);
}
