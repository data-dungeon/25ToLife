#ifndef __GIF_INLINE_H
#define __GIF_INLINE_H

#include <eestruct.h>

typedef signed char s_char;

namespace GIFInline
{

	inline u_long128 createGIFTAG(u_short nloop, u_char nreg, u_long regs, u_char eop = true, u_char flag = GIFTag::FLG_PACKED, u_int prim = 0xFFFFFFFF)
	{
		u_long128 tag;
		u_long halftag;
	
		// whats the pre bit set to?
		bool pre = false;
		if(prim != 0xFFFFFFFF)
		{
			pre = true;
		}
		else
		{
			prim = 0;
		}
	
		// generate the lower 64 bits of the gif tag
		halftag = ((u_long)nreg << 60) | ((u_long)flag << 58) | ((u_long)prim << 47) | ((u_long)pre << 46) | ((u_long)eop << 15) | nloop;
	
		// jam them together into a 128 bit integer
		asm("pextlw %0, %1, %2" : "=r" (tag) : "r" (regs), "r" (halftag));
		asm("pexcw %0, %1" : "=r" (tag) : "r" (tag));
	
		// done.
		return tag;
	}

	inline u_long128 createAD(u_long reg, u_long value)
	{
		u_long128 gifad;

		asm("pextlw %0, %1, %2" : "=r" (gifad) : "r" (reg), "r" (value));
		asm("pexcw %0, %1" : "=r" (gifad) : "r" (gifad));

		return gifad;
	}

	/*-- vertex info. reg--*/

	inline u_long128 createAD_PRIM(u_int prim)
	{
		return createAD(SCE_GS_PRIM, prim);
	}

	inline u_long128 createAD_RGBAQ(u_char R, u_char G, u_char B, u_char A, float Q)
	{
		return createAD(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(R, G, B, A, (*(u_int*)&Q)));
	}

	inline u_long128 createAD_ST(float S, float T)
	{
		return createAD(SCE_GS_ST, SCE_GS_SET_ST((*(u_int*)&S), (*(u_int*)&T)));
	}

	inline u_long128 createAD_UV(u_short U, u_short V)
	{
		return createAD(SCE_GS_UV, SCE_GS_SET_UV(U, V));
	}

	inline u_long128 createAD_XYZF2(u_short X, u_short Y, u_int Z, u_char F)
	{
		return createAD(SCE_GS_XYZF2, SCE_GS_SET_XYZF2(X, Y, Z, F));
	}

	inline u_long128 createAD_XYZ2(u_short X, u_short Y, u_int Z)
	{
		return createAD(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(X, Y, Z));
	}

	inline u_long128 createAD_FOG(u_char F)
	{
		return createAD(SCE_GS_FOG, SCE_GS_SET_FOG(F));
	}

	inline u_long128 createAD_XYZF3(u_short X, u_short Y, u_int Z, u_char F)
	{
		return createAD(SCE_GS_XYZF3, SCE_GS_SET_XYZF3(X, Y, Z, F));
	}

	inline u_long128 createAD_XYZ3(u_short X, u_short Y, u_int Z)
	{
		return createAD(SCE_GS_XYZ3, SCE_GS_SET_XYZ3(X, Y, Z));
	}

	inline u_long128 createAD_XYOFFSET_1(u_short X, u_short Y)
	{
		return createAD(SCE_GS_XYOFFSET_1, SCE_GS_SET_XYOFFSET_1(X, Y));
	}

	inline u_long128 createAD_XYOFFSET_2(u_short X, u_short Y)
	{
		return createAD(SCE_GS_XYOFFSET_2, SCE_GS_SET_XYOFFSET_2(X, Y));
	}

	inline u_long128 createAD_PRMODECONT(bool usePrim)
	{
		return createAD(SCE_GS_PRMODECONT, SCE_GS_SET_PRMODECONT(usePrim));
	}


	/*-- drawing attribute reg. --*/
	inline u_long128 createAD_PRMODE(bool iip, bool tme, bool fge, bool abe, bool aa1, bool fst, bool ctxt, bool fix)
	{
		return createAD(SCE_GS_PRMODE, SCE_GS_SET_PRMODE(iip, tme, fge, abe, aa1, fst, ctxt, fix));
	}

	inline u_long128 createAD_TEX0_1(u_short tbp, u_char tbw, u_char psm, u_char tw, u_char th, bool tcc, u_char tfx, u_short cbp, u_char cpsm, bool csm, u_char csa, u_char cld)
	{
		return createAD(SCE_GS_TEX0_1, SCE_GS_SET_TEX0_1(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld));
	}

	inline u_long128 createAD_TEX0_2(u_short tbp, u_char tbw, u_char psm, u_char tw, u_char th, bool tcc, u_char tfx, u_short cbp, u_char cpsm, bool csm, u_char csa, u_char cld)
	{
		return createAD(SCE_GS_TEX0_2, SCE_GS_SET_TEX0_2(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld));
	}

	inline u_long128 createAD_TEX1_1(bool lcm, u_char mxl, bool mmag, u_char mmin, bool mtba, u_char L, u_short K)
	{
		return createAD(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1(lcm, mxl, mmag, mmin, mtba, L, K));
	}

	inline u_long128 createAD_TEX1_2(bool lcm, u_char mxl, bool mmag, u_char mmin, bool mtba, u_char L, u_short K)
	{
		return createAD(SCE_GS_TEX1_2, SCE_GS_SET_TEX1_2(lcm, mxl, mmag, mmin, mtba, L, K));
	}

	inline u_long128 createAD_TEX2_1(u_char psm, u_short cbp, u_char cpsm, bool csm, u_char csa, u_char cld)
	{
		return createAD(SCE_GS_TEX2_1, SCE_GS_SET_TEX2_1(psm, cbp, cpsm, csm, csa, cld));
	}

	inline u_long128 createAD_TEX2_2(u_char psm, u_short cbp, u_char cpsm, bool csm, u_char csa, u_char cld)
	{
		return createAD(SCE_GS_TEX2_2, SCE_GS_SET_TEX2_2(psm, cbp, cpsm, csm, csa, cld));
	}

	inline u_long128 createAD_TEXCLUT(u_char cbw, u_char cou, u_short cov)
	{
		return createAD(SCE_GS_TEXCLUT, SCE_GS_SET_TEXCLUT(cbw, cou, cov));
	}

	inline u_long128 createAD_SCANMSK(u_char msk = 0)
	{
		return createAD(SCE_GS_SCANMSK, SCE_GS_SET_SCANMSK(msk));
	}

	inline u_long128 createAD_MIPTBP1_1(u_short tbp1, u_char tbw1, u_short tbp2, u_char tbw2, u_short tbp3, u_char tbw3)
	{
		return createAD(SCE_GS_MIPTBP1_1, SCE_GS_SET_MIPTBP1_1(tbp1, tbw1, tbp2, tbw2, tbp3, tbw3));
	}

	inline u_long128 createAD_MIPTBP1_2(u_short tbp1, u_char tbw1, u_short tbp2, u_char tbw2, u_short tbp3, u_char tbw3)
	{
		return createAD(SCE_GS_MIPTBP1_2, SCE_GS_SET_MIPTBP1_2(tbp1, tbw1, tbp2, tbw2, tbp3, tbw3));
	}

	inline u_long128 createAD_MIPTBP2_1(u_short tbp4, u_char tbw4, u_short tbp5, u_char tbw5, u_short tbp6, u_char tbw6)
	{
		return createAD(SCE_GS_MIPTBP2_1, SCE_GS_SET_MIPTBP2_1(tbp4, tbw4, tbp5, tbw5, tbp6, tbw6));
	}

	inline u_long128 createAD_MIPTBP2_2(u_short tbp4, u_char tbw4, u_short tbp5, u_char tbw5, u_short tbp6, u_char tbw6)
	{
		return createAD(SCE_GS_MIPTBP2_2, SCE_GS_SET_MIPTBP2_2(tbp4, tbw4, tbp5, tbw5, tbp6, tbw6));
	}

	inline u_long128 createAD_CLAMP_1(u_char wms, u_char wmt, u_short minu, u_short maxu, u_short minv, u_short maxv)
	{
		return createAD(SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(wms, wmt, minu, maxu, minv, maxv));
	}

	inline u_long128 createAD_CLAMP_2(u_char wms, u_char wmt, u_short minu, u_short maxu, u_short minv, u_short maxv)
	{
		return createAD(SCE_GS_CLAMP_2, SCE_GS_SET_CLAMP_2(wms, wmt, minu, maxu, minv, maxv));
	}

	inline u_long128 createAD_TEXA(u_char ta0, bool aem, u_char ta1)
	{
		return createAD(SCE_GS_TEXA, SCE_GS_SET_TEXA(ta0, aem, ta1));
	}

	inline u_long128 createAD_FOGCOL(u_char R, u_char G, u_char B)
	{
		return createAD(SCE_GS_FOGCOL, SCE_GS_SET_FOGCOL(R, G, B));
	}

	inline u_long128 createAD_TEXFLUSH(u_long unused = 0)
	{
		return createAD(SCE_GS_TEXFLUSH, unused);
	}


	/*-- pixel operation reg. --*/
	inline u_long128 createAD_SCISSOR_1(u_short X0, u_short X1, u_short Y0, u_short Y1)
	{
		return createAD(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR_1(X0, X1, Y0, Y1));
	}

	inline u_long128 createAD_SCISSOR_2(u_short X0, u_short X1, u_short Y0, u_short Y1)
	{
		return createAD(SCE_GS_SCISSOR_2, SCE_GS_SET_SCISSOR_2(X0, X1, Y0, Y1));
	}

	inline u_long128 createAD_ALPHA_1(u_char A, u_char B, u_char C, u_char D, u_char FIX)
	{
		return createAD(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(A, B, C, D, FIX));
	}

	inline u_long128 createAD_ALPHA_2(u_char A, u_char B, u_char C, u_char D, u_char FIX)
	{
		return createAD(SCE_GS_ALPHA_2, SCE_GS_SET_ALPHA_2(A, B, C, D, FIX));
	}

	inline u_long128 createAD_DIMX(s_char DM00, s_char DM01, s_char DM02, s_char DM03, 
												 s_char DM10, s_char DM11, s_char DM12, s_char DM13, 
												 s_char DM20, s_char DM21, s_char DM22, s_char DM23, 
												 s_char DM30, s_char DM31, s_char DM32, s_char DM33)
	{
		return createAD(SCE_GS_DIMX, SCE_GS_SET_DIMX(DM00, DM01, DM02, DM03, DM10, DM11, DM12, DM13, DM20, DM21, DM22, DM23, DM30, DM31, DM32, DM33));
	}

	inline u_long128 createAD_DTHE(bool dthe)
	{
		return createAD(SCE_GS_DTHE, SCE_GS_SET_DTHE(dthe));
	}

	inline u_long128 createAD_COLCLAMP(bool clamp)
	{
		return createAD(SCE_GS_COLCLAMP, SCE_GS_SET_COLCLAMP(clamp));
	}

	inline u_long128 createAD_TEST_1(bool ate, u_char atst, u_char aref, u_char afail, bool date, bool datm, bool zte, u_char ztst)
	{
		// due to a hardware bug, this ASSERT verifies a TRC requirement...
		ASSERT(zte == true);
		return createAD(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(ate, atst, aref, afail, date, datm, zte, ztst));
	}

	inline u_long128 createAD_TEST_2(bool ate, u_char atst, u_char aref, u_char afail, bool date, bool datm, bool zte, u_char ztst)
	{
		// due to a hardware bug, this ASSERT verifies a TRC requirement...
		ASSERT(zte == true);
		return createAD(SCE_GS_TEST_2, SCE_GS_SET_TEST_2(ate, atst, aref, afail, date, datm, zte, ztst));
	}

	inline u_long128 createAD_PABE(bool pabe)
	{
		return createAD(SCE_GS_PABE, SCE_GS_SET_PABE(pabe));
	}

	inline u_long128 createAD_FBA_1(bool fba)
	{
		return createAD(SCE_GS_FBA_1, SCE_GS_SET_FBA_1(fba));
	}

	inline u_long128 createAD_FBA_2(bool fba)
	{
		return createAD(SCE_GS_FBA_2, SCE_GS_SET_FBA_2(fba));
	}


	/*-- buffer reg. --*/
	inline u_long128 createAD_FRAME_1(u_short fbp, u_char fbw, u_char psm, u_int fbmsk)
	{
		return createAD(SCE_GS_FRAME_1, SCE_GS_SET_FRAME_1(fbp, fbw, psm, fbmsk));
	}

	inline u_long128 createAD_FRAME_2(u_short fbp, u_char fbw, u_char psm, u_int fbmsk)
	{
		return createAD(SCE_GS_FRAME_2, SCE_GS_SET_FRAME_2(fbp, fbw, psm, fbmsk));
	}

	inline u_long128 createAD_ZBUF_1(u_short zbp, u_char psm, bool zmsk = 0)
	{
		return createAD(SCE_GS_ZBUF_1, SCE_GS_SET_ZBUF_1(zbp, psm, zmsk));
	}

	inline u_long128 createAD_ZBUF_2(u_short zbp, u_char psm, bool zmsk = 0)
	{
		return createAD(SCE_GS_ZBUF_2, SCE_GS_SET_ZBUF_2(zbp, psm, zmsk));
	}


	/*-- inter-buffer transfer reg. --*/
	inline u_long128 createAD_BITBLTBUF(u_short sbp, u_char sbw, u_char spsm, u_short dbp, u_char dbw, u_char dpsm)
	{
		return createAD(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm));
	}

	inline u_long128 createAD_TRXPOS(u_short SX, u_short SY, u_short DX, u_short DY, u_char dir)
	{
		return createAD(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(SX, SY, DX, DY, dir));
	}

	inline u_long128 createAD_TRXREG(u_short RRW, u_short RRH)
	{
		return createAD(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(RRW, RRH));
	}

	inline u_long128 createAD_TRXDIR(u_char dir = 0x0)
	{
		return createAD(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(dir));
	}

	inline u_long128 createAD_HWREG(u_long data)
	{
		return createAD(SCE_GS_HWREG, data);
	}


	/*-- other reg. --*/
	inline u_long128 createAD_SIGNAL(u_int id, u_int idmask)
	{
		return createAD(SCE_GS_SIGNAL, SCE_GS_SET_SIGNAL(id, idmask));
	}

	inline u_long128 createAD_FINISH(u_long unused = 0)
	{
		return createAD(SCE_GS_FINISH, unused);
	}

	inline u_long128 createAD_LABEL(u_int id, u_int idmask)
	{
		return createAD(SCE_GS_LABEL, SCE_GS_SET_LABEL(id, idmask));
	}

	inline u_long128 createAD_NOP(u_long unused = 0)
	{
		return createAD(SCE_GS_NOP, unused);
	}

} // namespace GIFInline

#endif // __GIF_INLINE_H
