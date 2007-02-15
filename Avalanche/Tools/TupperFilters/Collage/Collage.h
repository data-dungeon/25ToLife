////////////////////////////////////////////////////////////////////////////
//
// Collage
//
// Class to handle the Collage Image Reader Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef COLLAGE_H
#define COLLAGE_H

extern HINSTANCE hInstance;

class TupperwareAggregate;

class CollageImageReader: public ImageReaderPlugin
{
	enum extensions
	{
		BMP, DIB, RLE,	// BMP FILE
		JPG, JPEG, JIF,	// JPEG FILE
		PNG,			// PNG FILE
		PCT,			// PICT FILE
		TIF, EPS,		// TIFF FILE
		TGA,			// TGA FILE
		WMF, EMF,		// WINDOW METAFILE
		PCX,			// PCX
		PGM,			// PORTABLE GREYMAP FILE
		PPM,			// PORTABLE PIXMAP FILE
		PSD,			// PHOTOSHOP FILE
		GIF,			// GIF
		TOTAL_EXTENSIONS // must be last
	};

	static char *m_extensionStrings[TOTAL_EXTENSIONS];

public:
	CollageImageReader(TupperChainLogBase &log) : ImageReaderPlugin(log) {}
	int GetTotalSupportedExtensions(void);
	const char *GetExtension(int extensionIndex);
	TupperwareAggregate *GetImageData(const char *filename,const FilterOptions &options,const char *pAttributeString,TupConfigRoot *pConfigRoot);
	TupperwareAggregate *GetDummyImageData(const char *filename,const FilterOptions &options,const char *pAttributeString,TupConfigRoot *pConfigRoot);
	void GetImageFileDependencies(const char *filename,const FilterOptions &options,FilePathArray &filePathArray);
private:
	void GetImageFromFile(const char *filename,TupImageData &tupImageData);
	void GetImagesNamesFromListFile(const char *pBaseFilename,const char *pListFilename,TupArray<TupString> &fileList);
	TupperwareAggregate *AddImageDataFrame(TupImageData &tupImageData,PLWinBmp &image);

};

#endif
