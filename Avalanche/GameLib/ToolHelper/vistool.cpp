//*************************************************************************
// Simple visualization thingie
//*************************************************************************
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ToolHelper/vistool.h"

// Some nice colors
const VisColor VisRED(255, 0, 0);
const VisColor VisGREEN(0, 255, 0);
const VisColor VisBLUE(0, 0, 255);
const VisColor VisCYAN(0, 255, 255);
const VisColor VisMAGENTA(255, 0, 255);
const VisColor VisYELLOW(255, 255, 0);
const VisColor VisWHITE(255, 255, 255);
const VisColor VisBLACK(0, 0, 0);
const VisColor VisGRAY(127, 127, 127);

static bool WritePPM(const char* fileName, int width, int height, unsigned char* image );

//*************************************************************************
// Setup everything
//*************************************************************************
VisTool::VisTool(
uint wantWidth,
uint wantHeight,
const VisColor &clearColor)
{
	width = (int)wantWidth;
	height = (int)wantHeight;
	pixMap = new VisColor[width * height];
	uint temp = sizeof(VisColor);
	uint size = temp * width * height;
	Clear(clearColor);
}

//*************************************************************************
//*************************************************************************
VisTool::~VisTool(void)
{
	delete [] pixMap;
}

//*************************************************************************
// Set the coordinate system
//*************************************************************************
void VisTool::SetCoordinateSystem(
Vector3CRef minCoord,
Vector3CRef maxCoord)
{
	originX = minCoord.x();
	originZ = minCoord.z();
	scaleX = (float)(width - 1) / (maxCoord.x() - minCoord.x());
	scaleZ = (float)(height - 1) / (maxCoord.z() - minCoord.z());
}

//*************************************************************************
// Set the coordinate system
//*************************************************************************
void VisTool::SetCoordinateSystem(
const XZRect &rect)
{
	originX = rect.MinX();
	originZ = rect.MinZ();
	scaleX = (float)(width - 1) / rect.Width();
	scaleZ = (float)(height - 1) / rect.Height();
}

//*************************************************************************
// Clear the image
//*************************************************************************
void VisTool::Clear(
const VisColor &clearColor)
{
	uint pixels = width * height;
	for (uint i = 0; i < pixels; i++)
		pixMap[i] = clearColor;
}

//*************************************************************************
// plot an AABB
//*************************************************************************
void VisTool::Plot(
const AABB &aabb,
const VisColor &c)
{
	// Convert the coords
	int minx, minz, maxx, maxz;
	Convert(aabb.minV, minx, minz);
	Convert(aabb.maxV, maxx, maxz);

	// Draw the box
	Draw(minx, minz, maxx, minz, c);
	Draw(maxx, minz, maxx, maxz, c);
	Draw(maxx, maxz, minx, maxz, c);
	Draw(minx, maxz, minx, minz, c);
}

//*************************************************************************
// Plot an OBB
//*************************************************************************
void VisTool::Plot(
const OBB &obb,
const VisColor &c)
{
	Vector3 vert[8];
	obb.Transform(*obb.Matrix(), *obb.Center(), vert);

	// Get all the vertices
	int		v[8][2];
	for (uint i = 0; i < 8; i++)
		Convert(vert[i], v[i][0], v[i][1]);

	// Draw all the lines
	for (uint e = 0; e < 12; e++)
	{
		uint e1, e2;
		OBB::GetEdge(e, e1, e2);

		Draw(v[e1][0], v[e1][1], v[e2][0], v[e2][1], c);
	}
}

//*************************************************************************
// Plot an XZRect
//*************************************************************************
void VisTool::Plot(
const XZRect &rect,
const VisColor &c)
{
	// Convert the coords
	int minx, minz, maxx, maxz;
	Convert(rect.MinX(), rect.MinZ(), minx, minz);
	Convert(rect.MaxX(), rect.MaxZ(), maxx, maxz);

	// Draw the box
	Draw(minx, minz, maxx, minz, c);
	Draw(maxx, minz, maxx, maxz, c);
	Draw(maxx, maxz, minx, maxz, c);
	Draw(minx, maxz, minx, minz, c);
}

//*************************************************************************
// Plot a line
//*************************************************************************
void VisTool::Plot(
Vector3CRef start,
Vector3CRef end,
const VisColor &c)
{
	int x1, z1, x2, z2;
	Convert(start, x1, z1);
	Convert(end, x2, z2);
	Draw(x1, z1, x2, z2, c);
}

//*************************************************************************
//*************************************************************************
void VisTool::Plot(
const Segment &s,
const VisColor &c)
{
	Plot(s.start, s.end, c);
}

//*************************************************************************
// Save to a ppm file
//*************************************************************************
bool VisTool::Save(
const char *filename)
{
	// Use nate's stuff to write it out
	return(WritePPM(filename, width, height, (unsigned char *)&pixMap[0]));
}

////////////////////////////////////////////////////////////////////////////
//
// WritePPM: write a PPM raw (type P6) file.  The PPM file has a header
// that will look something like:
//
//     P6
//     # comment
//     width height max_value
//     rgbrgbrgb...
//
//   where "P6" is the magic cookie which identifies the file type and
//   should be the only characters on the first line followed by a
//   carriage return.  Any line starting with a # mark is treated as a
//   comment.  After the magic cookie, three integer values are written:
//   width, height of the image and the maximum value for a pixel
//   (max_value must be < 256 for PPM raw files).  The data section
//   consists of width*height rgb triplets (one byte each) in binary
//   format (i.e., such as that written with fwrite() or equivalent).
//
//   If an error occurs, an error message is sent to stderr.
//
////////////////////////////////////////////////////////////////////////////

bool WritePPM(const char* fileName, int width, int height, unsigned char* image )
{
	char *ppmName = (char *)malloc(sizeof(char) * (strlen(fileName) + 1 + 3 + 1));
	if (!ppmName)
		return(false);

	strcpy(ppmName, fileName);
	bool hasExt = false;
	for (int i = 0; i < ((int)strlen(ppmName) - 3) && !hasExt; i++)
		hasExt = (strcmpi(&ppmName[i], ".ppm") == 0);
	if (!hasExt)
		strcat(ppmName, ".ppm");

	FILE *file = fopen(ppmName, "wb");
	free(ppmName);
	if (file == NULL)
		return(false);

	// Spit out binary PPM header.
	fprintf(file, "P6\n# %s\n%d %d\n255\n", fileName, width, height);

	// Spit out all the image data.
	fwrite(image, sizeof(unsigned char), width * height * 3, file);

	fclose(file);

	return(true);
}

