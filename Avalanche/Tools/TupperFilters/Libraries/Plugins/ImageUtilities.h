////////////////////////////////////////////////////////////////////////////
//
// ImageUtilities
//
// Classes to handle format convertion & manipulation of TupImageData objects
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_IMAGE_UTILITIES_H
#define TUPPERFILTERS_LIB_PLUGINS_IMAGE_UTILITIES_H

/* system includes */
class ImageUtilities
{
public:
	static void ConvertToFormat(TupImageData &tupImageData,int newFormat);
	static void Scale(TupImageData &tupImageData,int newWidth,int newHeight);
	static unsigned long GetPixelColor(TupImageDataFrame &tupImageDataFrame,int sourceMapIndex,float x,float y,bool bClampX,bool bClampY); 	// get bilinear 
	static unsigned long GetPixelColor(TupImageDataFrame &tupImageDataFrame,int sourceMapIndex,int x,int y);
	static unsigned long GetCRC(TupImageData &tupImageData);
	static unsigned long GetCRC(TupImageDataFrame &tupImageDataFrame);
	static int GetUsedColors(TupImageDataFrame &tupImageDataFrame,TupArray<int> &usedColors);
	static bool MergePalettes(TupImageDataFrame &frame1,TupArray<int> &usedColors1,
							  TupImageDataFrame &frame2,TupArray<int> &usedColors2,
							  TupArray<int> &remapTo1Array,int &colorsAdded,bool bTestOnly);
	static void RemapPalettedFrame(TupImageDataFrame &tupImageDataFrame,TupArray<int> &remapArray);
	static void CopyPalette(TupImageDataFrame &dstFrame,TupImageDataFrame &srcFrame);
	static int GetUsedColors(const TupArray<int> &usedColors);
	static void ConvertLuminanceToAlpha(TupImageData &tupImageData);
	static int BuildMipMaps(TupImageData &tupImageData,int numSubMaps); // set up the mip map information

	static void Reverse(TupImageData &tupImageData);

	static void MarkAsFixTexture(TupImageData &tupImageData);


private:
	static void Scale8888(TupImageDataFrame &tupImageDataFrame,int newWidth,int newHeight,int sourceMapIndex,int destMapIndex);

	static void Convert888to8888(TupImageDataFrame &tupImageDataFrame);
	static void Convert565to8888(TupImageDataFrame &tupImageDataFrame);
	static void Convert555to8888(TupImageDataFrame &tupImageDataFrame);
	static void Convert1555to8888(TupImageDataFrame &tupImageDataFrame);
	static void Convert8to8888(TupImageDataFrame &tupImageDataFrame);
	static void Convert4to8888(TupImageDataFrame &tupImageDataFrame);
	static void Convert8888to888(TupImageDataFrame &tupImageDataFrame);
	static void Convert8888to565(TupImageDataFrame &tupImageDataFrame);
	static void Convert8888to555(TupImageDataFrame &tupImageDataFrame);
	static void Convert8888to1555(TupImageDataFrame &tupImageDataFrame);
	static void Convert8888to8(TupImageDataFrame &tupImageDataFrame);
	static void Convert8888to4(TupImageDataFrame &tupImageDataFrame);

	static void Reverse32(TupImageDataFrame &tupImageDataFrame);
	static void Reverse24(TupImageDataFrame &tupImageDataFrame);
	static void Reverse16(TupImageDataFrame &tupImageDataFrame);
	static void Reverse8(TupImageDataFrame &tupImageDataFrame);
	static void Reverse4(TupImageDataFrame &tupImageDataFrame);

	static void MarkAsFixTexture8888(TupImageDataFrame &tupImageDataFrame);

};

#endif

