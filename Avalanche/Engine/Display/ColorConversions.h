#ifndef __COLORCONVERSIONS_H__
#define __COLORCONVERSIONS_H__

///////////////////////////////////////////////////////////////////////////
//  GENERIC COLOR MANIPULATION MACROS
///////////////////////////////////////////////////////////////////////////

#define RGB_TO_COLOR(r,g,b)     ( 0xFF000000       | (((s32)(r))<<16) | (((s32)(g))<<8) | ((s32)(b)) )
#define RGBA_TO_COLOR(r,g,b,a)  ( (((s32)(a))<<24) | (((s32)(r))<<16) | (((s32)(g))<<8) | ((s32)(b)) )

#define COLOR_TO_A(c)           ( (u8)(((c) & 0xFF000000) >> 24) )
#define COLOR_TO_R(c)           ( (u8)(((c) & 0x00FF0000) >> 16) )
#define COLOR_TO_G(c)           ( (u8)(((c) & 0x0000FF00) >>  8) )
#define COLOR_TO_B(c)           ( (u8)(((c) & 0x000000FF) >>  0) )

#define COLOR_SET_A(c,a)        ( (c) = (((c) & 0x00FFFFFF) | (((a) & 0xFF) << 24)) )
#define COLOR_SET_R(c,r)        ( (c) = (((c) & 0xFF00FFFF) | (((r) & 0xFF) << 16)) )
#define COLOR_SET_G(c,g)        ( (c) = (((c) & 0xFFFF00FF) | (((g) & 0xFF) <<  8)) )
#define COLOR_SET_B(c,b)        ( (c) = (((c) & 0xFFFFFF00) | (((b) & 0xFF) <<  0)) )

#define COLOR_TO_RGB(c,r,g,b)       ( ((r) = COLOR_TO_R(c)),    \
                                      ((g) = COLOR_TO_G(c)),    \
                                      ((b) = COLOR_TO_B(c)) )

#define COLOR_TO_RGBA(c,r,g,b,a)    ( ((r) = COLOR_TO_R(c)),    \
                                      ((g) = COLOR_TO_G(c)),    \
                                      ((b) = COLOR_TO_B(c)),    \
                                      ((a) = COLOR_TO_A(c)) )

#define COLOR_TO_RGBU5551(c)        ( (((c)&0x00F80000) >> 8) |   \
                                      (((c)&0x0000F800) >> 5) |   \
                                      (((c)&0x000000F8) >> 2) |   \
                                      (     0x00000001      ) )

#define COLOR_TO_RGBA5551(c)        ( (((c)&0x00F80000) >> 8) |   \
                                      (((c)&0x0000F800) >> 5) |   \
                                      (((c)&0x000000F8) >> 2) |   \
                                      (((c)&0x80000000) >>31) )

#define RGB_TO_RGBU5551(r,g,b)      ( (((r)&0xF8)<<8) |         \
                                      (((g)&0xF8)<<3) |         \
                                      (((b)&0xF8)>>2) |         \
                                      (     0x01    ) )

#define RGBA_TO_RGBA5551(r,g,b,a)   ( (((r)&0xF8)<<8) |         \
                                      (((g)&0xF8)<<3) |         \
                                      (((b)&0xF8)>>2) |         \
                                      (((a)&0x80)>>7) )

#define RGBA_TO_ABGR1555(r,g,b,a)   ( (((a)&0x80)<<8) |     \
                                      (((b)&0xF8)<<7) |     \
                                      (((g)&0xF8)<<2) |     \
                                      (((r)&0xF8)>>3) )


// Dreamcast color format conversions
#define RGBA_TO_ARGB1555(r,g,b,a)   ( (((a)&0x80)<<8) |     \
                                      (((r)&0xF8)<<7) |     \
                                      (((g)&0xF8)<<2) |     \
                                      (((b)&0xF8)>>3) )


#define RGBA_TO_RGBA1555(r,g,b,a) (u16)(a<<15 | (r)<<11 | (g)<<6 | (b))

#define COLOR_TO_RGBU8888(c)    ( ((c)<<8) | 0xFF )
#define RGB_TO_RGBU8888(r,g,b)  ( ((r)<<24) | ((g)<<16) | ((b)<<8) | 0xFF )

// macros for ts_bRGBA conversions
#define	TS_RGBA_TO_RGBA8888(c) ( c.A << 24 | c.R << 16 | c.G <<8 | c.B )
#define	RGBA8888_TO_TS_RGBA(src, dst) ( ((dst.R) = COLOR_TO_R(src)), \
													  ((dst.G) = COLOR_TO_G(src)), \
												 	  ((dst.B) = COLOR_TO_B(src)), \
													  ((dst.A) = COLOR_TO_A(src)) )



#endif