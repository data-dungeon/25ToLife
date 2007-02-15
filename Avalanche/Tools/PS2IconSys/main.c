#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sjis.h"

/*NOTE: For some unknown reason, this program only works in a debug build!!!!!*/

/*
Title "Rugrats Backyard"
LF 7
Trans 0x60
ULColor 255,0,0
URColor 255,0,0
LLColor 255,0,0
LRColor 255,0,0
L1Dir 1.0,0.0,0.0
L2Dir 0.0,1.0,0.0
L3Dir 1.0,0.0,1.0
L1Color 128,0,0
L2Color 0,128,0
L3Color 0.0.255
AColor 0.0.255
ViewIcon "rrlogo.ico"
CopyIcon "rrlogo.ico"
DelIcon "rrlogo.ico"
*/

typedef int	_iconVu0IVECTOR[4];
typedef float	_iconVu0FVECTOR[4];
typedef struct {
	unsigned char Head[4];
	unsigned short Reserv1;
	unsigned short OffsLF;
	unsigned Reserv2;
	unsigned TransRate;
	_iconVu0IVECTOR BgColor[4];
	_iconVu0FVECTOR LightDir[3];
	_iconVu0FVECTOR LightColor[3];
	_iconVu0FVECTOR Ambient;
	unsigned char TitleName[68];
	unsigned char FnameView[64];
	unsigned char FnameCopy[64];
	unsigned char FnameDel[64];
	unsigned char Reserve3[512];
	} sceMcIconSys;


int mc_IconSysGen(char *inFilename, char *outFilename)
{
    FILE *infd;
	 FILE *outfd;
	 int lf, trans, r, g, b;
	 float x,y,z;
    sceMcIconSys icon_sys;
	 char line[256], asciiTitle[256], iconName[256];

    infd = fopen(inFilename, "r" );
	 if(infd == NULL)
	 {
		 printf("Cant open infile for reading: %s - exiting\n", inFilename);
		 return(0);
	 }

    // Set entire structure to 0
    memset(&icon_sys, 0, sizeof(sceMcIconSys));

	 // Get ASCII Title
	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "Title %[^\n]%[\n]", &asciiTitle);
    strtosjis(asciiTitle, icon_sys.TitleName);

    // icon_sys.header = "PS2D";
    icon_sys.Head[0] = 'P';
    icon_sys.Head[1] = 'S';
    icon_sys.Head[2] = '2';
    icon_sys.Head[3] = 'D';

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "LF %d", &lf);
    icon_sys.OffsLF = lf*2;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "Trans %d", &trans);
    icon_sys.TransRate = trans;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "ULColor %d,%d,%d", &r, &g, &b);

    icon_sys.BgColor[0][0] = r;
    icon_sys.BgColor[0][1] = g;
    icon_sys.BgColor[0][2] = b;
    icon_sys.BgColor[0][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "URColor %d,%d,%d", &r, &g, &b);
    icon_sys.BgColor[1][0] = r;
    icon_sys.BgColor[1][1] = g;
    icon_sys.BgColor[1][2] = b;
    icon_sys.BgColor[1][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "LLColor %d,%d,%d", &r, &g, &b);
    icon_sys.BgColor[2][0] = r;
    icon_sys.BgColor[2][1] = g;
    icon_sys.BgColor[2][2] = b;
    icon_sys.BgColor[2][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "LRColor %d,%d,%d", &r, &g, &b);
    icon_sys.BgColor[3][0] = r;
    icon_sys.BgColor[3][1] = g;
    icon_sys.BgColor[3][2] = b;
    icon_sys.BgColor[3][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "L1Dir %f,%f,%f", &x, &y, &z);
    icon_sys.LightDir[0][0] = x;
    icon_sys.LightDir[0][1] = y;
    icon_sys.LightDir[0][2] = z;
    icon_sys.LightDir[0][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "L2Dir %f,%f,%f", &x, &y, &z);
    icon_sys.LightDir[1][0] = x;
    icon_sys.LightDir[1][1] = y;
    icon_sys.LightDir[1][2] = z;
    icon_sys.LightDir[1][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "L3Dir %f,%f,%f", &x, &y, &z);
    icon_sys.LightDir[2][0] = x;
    icon_sys.LightDir[2][1] = y;
    icon_sys.LightDir[2][2] = z;
    icon_sys.LightDir[2][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "L1Color %d,%d,%d", &r, &g, &b);
    icon_sys.LightColor[0][0] = r/255.0f;
    icon_sys.LightColor[0][1] = g/255.0f;
    icon_sys.LightColor[0][2] = b/255.0f;
    icon_sys.LightColor[0][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "L2Color %d,%d,%d", &r, &g, &b);
    icon_sys.LightColor[1][0] = r/255.0f;
    icon_sys.LightColor[1][1] = g/255.0f;
    icon_sys.LightColor[1][2] = b/255.0f;
    icon_sys.LightColor[1][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "L3Color %d,%d,%d", &r, &g, &b);
    icon_sys.LightColor[2][0] = r/255.0f;
    icon_sys.LightColor[2][1] = g/255.0f;
    icon_sys.LightColor[2][2] = b/255.0f;
    icon_sys.LightColor[2][3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "AColor %d,%d,%d", &r, &g, &b);
    icon_sys.Ambient[0] = r/255.0f;
    icon_sys.Ambient[1] = g/255.0f;
    icon_sys.Ambient[2] = b/255.0f;
    icon_sys.Ambient[3] = 0x0000;

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "ViewIcon %s", iconName);
    strcpy( icon_sys.FnameView, iconName);

	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "CopyIcon %s", iconName);
    strcpy( icon_sys.FnameCopy, iconName);
	 
	 fscanf(infd, "%[^\n]%[\n]", line);
	 sscanf(line, "DelIcon %s", iconName);
    strcpy( icon_sys.FnameDel, iconName);

    outfd = fopen(outFilename, "wb" );
    if(outfd == NULL)
	 {
		 printf("Cant open outfile for writing (write protected?): %s - exiting\n", outFilename);
        return (0);
	}

    fclose(infd);

    if( fwrite(&icon_sys, 1, sizeof(icon_sys), outfd) != sizeof(icon_sys))
    {
		 printf("Cant write outfile: %s - exitting\n", outFilename);
        fclose(outfd);
        return (0);
    }
    fclose(outfd);

	 printf("Successful completion\n");

    return (1);
}

int main(int argc, char *argv[])
{
	int ret;

	if(argc != 3)
	{
		printf("Usage: ps2iconsys input.txt outputicon.sys\n\n");
		printf("The output.ico should then be combined with the logoicon.ico to create the\ncombo file the PS2 memcard code expects.\n");
		printf("Combine with the DOS copy command, e.g.,\n copy /b outputicon.sys+logo.ico logodat.ico\n");
		printf("The input.txt should contain the following lines in the following order:\n\n");

		printf("Title The Title of the Game\n");
		printf("LF 7\n");
		printf("Trans 100\n");
		printf("ULColor 255,0,0\n");
		printf("URColor 255,0,0\n");
		printf("LLColor 255,0,0\n");
		printf("LRColor 255,0,0\n");
		printf("L1Dir 1.0,0.0,0.0\n");
		printf("L2Dir 0.0,1.0,0.0\n");
		printf("L3Dir 1.0,0.0,1.0\n");
		printf("L1Color 128,0,0\n");
		printf("L2Color 0,128,0\n");
		printf("L3Color 0,0,255\n");
		printf("AColor 0,0,255\n");
		printf("ViewIcon logodat.ico\n");
		printf("CopyIcon logodat.ico\n");
		printf("DelIcon logodat.ico\n");
	
		exit(1);
	}

	ret = mc_IconSysGen(argv[1], argv[2]);

	exit(ret);
}

