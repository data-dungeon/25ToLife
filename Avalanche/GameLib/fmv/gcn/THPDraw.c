/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPDraw.c

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLibnew/fmv/gcn/THPDraw.c $
 * 
 * 3     7/19/03 3:44p Todd Blackburn
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 10:52a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#include <demo.h>
#include "THPDraw.h"

/*---------------------------------------------------------------------------*
    Name:           THPGXRestore

    Description:    Return swap table data used to basic status

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void THPGXRestore( void )
{
    GXSetZMode(GX_ENABLE, GX_ALWAYS, GX_DISABLE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_SET);
    
    GXSetNumTexGens(1);
    GXSetNumChans(1);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);        

	 GXSetTexCoordScaleManually( GX_TEXCOORD0, GX_ENABLE, 1, 1);
	 GXSetTexCoordScaleManually( GX_TEXCOORD1, GX_ENABLE, 1, 1);
	 

    // Swap mode settings
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);

    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED,   GX_CH_GREEN,
                                        GX_CH_BLUE,  GX_CH_ALPHA); // RGBA
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED,   GX_CH_RED,
                                        GX_CH_RED,   GX_CH_ALPHA); // RRRA
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN,
                                        GX_CH_GREEN, GX_CH_ALPHA); // GGGA
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE,  GX_CH_BLUE,
                                        GX_CH_BLUE,  GX_CH_ALPHA); // BBBA

}



/*---------------------------------------------------------------------------*
    Name:           THPGXYuv2RgbSetup

    Description:    Setup routine needed for rendering movies.

    Arguments:      rmode  Pointer for currently set GXRenderModeObj

    Returns:        None
 *---------------------------------------------------------------------------*/

void THPGXYuv2RgbSetup( GXRenderModeObj *rmode )
{
    s32         scrWidth;
    s32         scrHeight;
    Mtx44       pMtx;
    Mtx         mMtx;
    
    scrWidth  = rmode->fbWidth;
    scrHeight = rmode->efbHeight;

    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    // Geometry
    MTXOrtho(pMtx, 0.0f, (f32)scrHeight, 0.0f, (f32)scrWidth, 0.0f, -1.0F);
    GXSetProjection(pMtx, GX_ORTHOGRAPHIC);
    GXSetViewport(0.0F, 0.0F, (f32)scrWidth, (f32)scrHeight, 0.0F, 1.0F);
    GXSetScissor(0, 0, (u32)scrWidth, (u32)scrHeight);
	 GXSetCullMode( GX_CULL_NONE);

    MTXIdentity(mMtx);
    GXLoadPosMtxImm(mMtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    
    // Framebuffer
    GXSetZMode(GX_ENABLE, GX_ALWAYS, GX_DISABLE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_FALSE);
    GXSetDispCopyGamma(GX_GM_1_0);
	 GXSetZCompLoc( GX_TRUE);
	 GXSetAlphaCompare( GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

    // Color channels
    GXSetNumChans(0);
    
    // Texture coord generation
    GXSetNumTexGens(2);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	 GXSetTexCoordScaleManually( GX_TEXCOORD0, GX_DISABLE, 1, 1);
	 GXSetTexCoordScaleManually( GX_TEXCOORD1, GX_DISABLE, 1, 1);

    // Texture cache
    GXInvalidateTexAll();

    // Vertex formats
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS,  GX_POS_XYZ, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_TEX0, GX_TEX_ST,  GX_U16, 0);

    // *******************************************************
    //    TEV settings to perform YUV->RGB conversion
    // *******************************************************
    GXSetNumTevStages(4);
    
    // Stage 0 (Cb texture)
    //    R = -90, B = -114+226Cb, A = 135-88Cb
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC,
                    GX_CC_KONST, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA,
                    GX_CA_KONST, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    
    // Stage 1 (Cr texture)
    //    R = 2*(R+179Cr), B = 2*B, A = A-182Cr
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC,
                    GX_CC_KONST, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO,
                    GX_CS_SCALE_2, GX_FALSE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA,
                    GX_CA_KONST, GX_CA_APREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
    GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    
    // Stage 2 (Y texture)
    //    R += 255Y, B += 255Y, A += 255Y [do clamp]
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC,
                    GX_CC_ONE, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_TEXA, GX_CA_ZERO,
                    GX_CA_ZERO, GX_CA_APREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);

    // Stage 3
    //    Merge Alpha into the Green channel
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_APREV, GX_CC_CPREV,
                    GX_CC_KONST, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO,
                    GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO,
                    GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
    
    // Constant colors
    GXSetTevColorS10(GX_TEVREG0, (GXColorS10){ -90, 0, -114, 135 });
    GXSetTevKColor(GX_KCOLOR0, (GXColor){   0,   0, 226,  88 });
    GXSetTevKColor(GX_KCOLOR1, (GXColor){ 179,   0,   0, 182 });
    GXSetTevKColor(GX_KCOLOR2, (GXColor){ 255,   0, 255, 128 });

    // Swap mode tables
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED,   GX_CH_GREEN,
                                        GX_CH_BLUE,  GX_CH_ALPHA); // RGBA
}


/*---------------------------------------------------------------------------*
    Name:           THPGXYuv2RgbDraw

    Description:    Draw polygon with movie texture attached.

    Arguments:      y_data        y texture pointer
                    u_data        u texture pointer
                    v_data        v texture pointer
                    x             Upper-left x coordinate on TV screen to
                                   display polygon.
                    y             Upper-left y coordinate on TV screen to
                                   display polygon.
                    textureWidth  Width of texture
                    textureHeight Height of texture
                    polygonWidth  Width of polygon
                    polygonHeight Height of polygon

    Returns:        None
 *---------------------------------------------------------------------------*/

void 
THPGXYuv2RgbDraw
( 
    u8* y_data, 
    u8* u_data, 
    u8* v_data, 
    s16 x, 
    s16 y, 
    s16 textureWidth, 
    s16 textureHeight,
    s16 polygonWidth,
    s16 polygonHeight
    
 )
{
    GXTexObj    tobj0, tobj1, tobj2;

    // Y Texture
    GXInitTexObj(&tobj0, y_data, (u16)textureWidth, (u16)textureHeight, 
                 GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&tobj0, GX_NEAR, GX_NEAR, 0, 0, 0, 0, 0, GX_ANISO_1);
    GXLoadTexObj(&tobj0, GX_TEXMAP0);

    // Cb Texture
    GXInitTexObj(&tobj1, u_data, (u16)(textureWidth>>1), (u16)(textureHeight>>1),
                 GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&tobj1, GX_NEAR, GX_NEAR, 0, 0, 0, 0, 0, GX_ANISO_1);
    GXLoadTexObj(&tobj1, GX_TEXMAP1);

    // Cr Texture
    GXInitTexObj(&tobj2, v_data, (u16)(textureWidth>>1), (u16)(textureHeight>>1),
                 GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&tobj2, GX_NEAR, GX_NEAR, 0, 0, 0, 0, 0, GX_ANISO_1);
    GXLoadTexObj(&tobj2, GX_TEXMAP2);

    // Draw a quad
    GXBegin(GX_QUADS, GX_VTXFMT7, 4);
        GXPosition3s16(x, y, 0);
        GXTexCoord2u16(0, 0);
        GXPosition3s16((s16)(x+polygonWidth), y, 0);
        GXTexCoord2u16(1, 0);
        GXPosition3s16((s16)(x+polygonWidth), (s16)(y+polygonHeight), 0);
        GXTexCoord2u16(1, 1);
        GXPosition3s16(x, (s16)(y+polygonHeight), 0);
        GXTexCoord2u16(0, 1);
    GXEnd();
}

