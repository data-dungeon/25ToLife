TGA/Targa file viewer

--------------------------------------------------------------------------
Purpose:	Loads and views a TGA (Targa) file
Author:	James Russell
Date:		22 August 2000
Updated:

Copyright (C) 2000 Sony Computer Entertainment Europe.,
          All Rights Reserved.  
Notes:
	- Last compiled using Library 1.6.5
-------------------------------------------------------------------------- 

	---------------
	- Description -
	---------------

	This program will load and display a TGA (Truevision Targa) graphics file on the
	screen in 640/512 PAL mode.

	Also implements 2 circuit anti-aliasing, if AA_ON is #define'd in draw.c
		 
	---------
	- Files -
	---------

	readme.txt - This textfile
	main.elf - the executable
	rdtarga.c - stuff for reading a targa
	draw.c	- stuff for setting up the screen
	main.c - the main program

	------------------
	- How to Execute - 
	------------------

	dsedb -r run main.elf /path/to/my/tgafile.tga

	e.g. dsedb -r run main.elf ~/bananas.tga
