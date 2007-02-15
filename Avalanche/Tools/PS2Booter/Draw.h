#ifndef DRAW_H
#define DRAW_H

#define SCREEN_WIDTH (640)
#define SCREEN_HEIGHT (512)

#define ORIGINX(xxx) ((((2048 - SCREEN_WIDTH/2) + (xxx)) << 4) & 0xffff)
#define ORIGINY(yyy) ((((2048 - SCREEN_HEIGHT/2) + (yyy)) << 4) & 0xffff)

#define NTSC_MODE		0
#define PAL_MODE		1


#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void DRAW_init(int mode);
void DRAW_flipScreen(void);
u_long *DRAW_getPacketBuffer(void);
void DRAW_addToDrawQueue(u_long *endOfPacket, int action);

void DRAW_line(u_long col, int sx, int sy, int dx, int dy);
void DRAW_setBGCol(u_int col);
void DRAW_clearScreen(void);

void DRAW_drawLetter(u_int col, u_char chr, int x, int y);
void DRAW_circle(u_long col, int cx, int cy, int radius, int fill);
void DRAW_filledRect(u_long col, int x1, int y1, int x2, int y2);
void DRAW_outlineRect(u_long col, int x1, int y1, int x2, int y2);
void DRAW_pixel(u_long col, int x, int y);

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif


#endif

