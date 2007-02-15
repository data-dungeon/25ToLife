//============================================================================
//= BootLogo - display a splash logo before anything loads
//============================================================================

#ifndef _BOOT_LOGO_H
#define _BOOT_LOGO_H

class PS2DisplaySimple;

class BootLogo
{
public:
	static void DisplayImage( PS2DisplaySimple* display, unsigned char* compressedImage );
};

#endif //_BOOT_LOGO_H
