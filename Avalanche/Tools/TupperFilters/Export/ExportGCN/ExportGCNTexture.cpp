////////////////////////////////////////////////////////////////////////////
//
// ExportGCNTexture
//
// Export the textures for the GameCube
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportGCNTexture.cpp $
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:36a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * mip map support
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 4/07/03    Time: 1:34p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * oops on separate alpha plane
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/13/03    Time: 10:09a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * texture id bug
 * 
 * *****************  Version 11  *****************
 * User: Food         Date: 3/12/03    Time: 4:53p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 10/03/02   Time: 9:42a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * set animated texture flag in texture chunk record
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 9/11/02    Time: 2:19p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * GCN texture alpha separation
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 9/10/02    Time: 11:07a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * GCN palette sharing
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/16/02    Time: 3:35p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Palette alignment
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 8:53a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * use right part of texture name
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Animated Textures for Gamecube
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 6/10/02    Time: 9:27a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Texture Fixes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/04/02    Time: 2:07p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/16/02    Time: 12:43p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Texture Exports for GameCube
 */

#include "ExportPch.h"
#include "ExportTexture.h"
#include "s3_intrf.h"

#define GREEN_HIGH_MASK 0x07
#define GREEN_LOW_MASK	0xC0
#define BLUE_MASK		0x3E
#define ALPHA_MASK		0x01
#define GREEN_565_MASK  0x03

// Texture Block Writer
class TBlock
{
public:
	TupArray<u8> m_data;
	void WriteBlock(FILE* pDBLFile);
	void WriteBlock32Bit(FILE* pDBLFile);
};

void TBlock::WriteBlock(FILE* pDBLFile)
{
	int i;
	for(i=0;i<m_data.GetSize();i++)
	{
		DBLWrite::WriteRecord((const char *)&m_data[i],pDBLFile);
	}
}

void TBlock::WriteBlock32Bit(FILE* pDBLFile)
{
	int i;

	// For 32bit RGBA write out AR Block
	for(i=0;i<m_data.GetSize();i+=4)
	{
		DBLWrite::WriteRecord((const char *)&m_data[i],pDBLFile);
		DBLWrite::WriteRecord((const char *)&m_data[i+1],pDBLFile);
	}

	for(i=2;i<m_data.GetSize();i+=4)
	{
		DBLWrite::WriteRecord((const char *)&m_data[i],pDBLFile);
		DBLWrite::WriteRecord((const char *)&m_data[i+1],pDBLFile);
	}
}

void ExportTexture::WriteGCNTexture(TupperwareAggregate *pImageAgg,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation)
{
	ts_TextureChunkRecord &textureChunkRecord = textureChunkRecordArray[frameNum][mapIndex];

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	TupImage tupImage(pImageAgg);
	char *pImageFileName;
	// get filename for image
	tupImage.GetFilePathName(&pImageFileName);
	// get image data from image manager
	ImageManager &imageManager = m_dataManager.GetImageManager();
	TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
	assert(pImageDataAgg);
	// get the first frame from the image data
	TupImageData tupImageData(pImageDataAgg);
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>0);

	// if this is not when we save animations and it is one then get out
	if (!bAnimation && numFrames>1)
	{
		// if this is animated texture and were are dealing with the entry in the main texture set
		// we need to set some flags based on the textures within the image 
		// the frames get saved out later
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0);
		assert(pImageDataFrameAgg);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);
		switch(formatFlags&TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
				textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
				break;
			case TupImageDataFrame::INDEXED_8:
			case TupImageDataFrame::INDEXED_4:
				int numAlphasUsed = pImageAgg->FindScalarByKey("Annotate_PaletteAlphasUsed")->GetAsInt();
				if (numAlphasUsed>2)
					textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
				break;
		}
		return;
	}

	// which frame to get from the image (depends on if we are doing animation or not)
	int animationFrameNum = bAnimation ? frameNum : 0;

	TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(animationFrameNum);
	assert(pImageDataFrameAgg);
	TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	// get first map from frame
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	assert(numMaps>0);
	TupperwareAggregate *pImageDataMap = tupImageDataFrame.GetImageDataMap(mapIndex);
	assert(pImageDataMap);
	TupImageDataMap tupImageDataMap(pImageDataMap);
	// get the bitmap data
	int width;
	tupImageDataMap.GetWidth(width);
	int height;
	tupImageDataMap.GetHeight(height);

	int imageSize;
	u8 *pImageData;

	TBlock ***ppBlock;
	TBlock ***ppAlphaBlock;

	int nBlockWidth, nBlockHeight, nTotalBlockSize;  // in blocks of 32 Bytes
	int nAlphaBlockWidth, nAlphaBlockHeight, nAlphaTotalBlockSize;  // in blocks of 32 Bytes

	bool bCompressed = false;
	bool bSeparateAlpha = false;

	switch(formatFlags&TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::RGB_888:
		case TupImageDataFrame::RGBA_8888:
			nBlockWidth = width/4;
			nBlockHeight = height/4;
			if(width%4!=0)
				nBlockWidth++;
			if(height%4!=0)
				nBlockHeight++;
			nTotalBlockSize = 64;

			nAlphaBlockWidth = width/8;
			nAlphaBlockHeight = height/4;
			if(width%8!=0)
				nBlockWidth++;
			if(height%4!=0)
				nBlockHeight++;
			nAlphaTotalBlockSize = 32;
			break;
		case TupImageDataFrame::RGB_555:
		case TupImageDataFrame::RGB_565:
		case TupImageDataFrame::RGBA_1555:
			nBlockWidth = width/4;
			nBlockHeight = height/4;
			if(width%4!=0)
				nBlockWidth++;
			if(height%4!=0)
				nBlockHeight++;
			nTotalBlockSize = 32;
			nAlphaBlockWidth = nBlockWidth;
			nAlphaBlockHeight = nBlockHeight;
			break;
		case TupImageDataFrame::INDEXED_8:
			nBlockWidth = width/8;
			nBlockHeight = height/4;
			if(width%8!=0)
				nBlockWidth++;
			if(height%4!=0)
				nBlockHeight++;
			nTotalBlockSize = 32;
			nAlphaBlockWidth = nBlockWidth;
			nAlphaBlockHeight = nBlockHeight;
			break;
		case TupImageDataFrame::INDEXED_4:
			nBlockWidth = width/8;
			nBlockHeight = height/8;
			if(width%8!=0)
				nBlockWidth++;
			if(height%8!=0)
				nBlockHeight++;
			nTotalBlockSize = 32;
			nAlphaBlockWidth = nBlockWidth;
			nAlphaBlockHeight = nBlockHeight;
			break;
		default:
			assert(false);
	}
	int nLineSize = width<<2;
	u8 padChar = 0xff;
	int x,y;

	// Create 2D Block Pointer Array
	try
	{
		ppBlock = new TBlock**[nBlockWidth];
		for(x=0;x<nBlockWidth;x++)
		{
			ppBlock[x] = new TBlock*[nBlockHeight];
		}
		for(x=0;x<nBlockWidth;x++)
		{
			for(y=0;y<nBlockHeight;y++)
			{
				ppBlock[x][y] = new TBlock;
			}
		}
		ppAlphaBlock = new TBlock**[nAlphaBlockWidth];
		for( x = 0; x < nAlphaBlockWidth; x++)
		{
			ppAlphaBlock[x] = new TBlock*[nAlphaBlockHeight];
		}
		for( x = 0; x < nAlphaBlockWidth; x++)
		{
			for( y = 0; y < nAlphaBlockHeight; y++)
			{
				ppAlphaBlock[x][y] = new TBlock;
			}
		}
	}
	catch(...)
	{
		throw(PluginException(ERROR_FATAL,"Creating GCN Texture Block Array width=%d,height=%d",nBlockWidth,nBlockHeight));
	}

	switch(formatFlags&TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::RGBA_8888:
			{
				if(1) // nSourceDataType != SOURCE_DATA_TEXTURE_LIST)
				{
					imageSize = width*height*4;
					assert(imageSize);
					int actualSize;
					tupImageDataMap.GetData((void**)&pImageData,actualSize);
					assert(imageSize==actualSize);

					textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
					bSeparateAlpha = true;

					unsigned int	nBytesEncode = S3TC_GetEncodeSize( width, height, S3TC_ENCODE_RGB_FULL | S3TC_ENCODE_ALPHA_NONE);

					u8 *pEncodedImage;

					S3_TEXTURE s3texIn,s3texOut;

					bCompressed = true;
					nTotalBlockSize = 8;
					memset( &s3texIn, 0, sizeof(S3_TEXTURE));
					s3texIn.lWidth = width;
					s3texIn.lHeight = height;
					s3texIn.lPitch = width * sizeof(ts_bRGBA);
					s3texIn.pSurface = pImageData;
					s3texIn.PixelFormat.nARGBBitCount = 32;
					s3texIn.PixelFormat.nRedMask = 0xFF << 0;
					s3texIn.PixelFormat.nGreenMask = 0xFF << 8;
					s3texIn.PixelFormat.nBlueMask = 0xFF << 16;

					pEncodedImage = new u8[nBytesEncode];

					int row,col;
					u8 lo,hi;
					int pixelOffset = 0;

					S3TC_Encode( &s3texIn, &s3texOut, pEncodedImage, S3TC_ENCODE_RGB_FULL | S3TC_ENCODE_ALPHA_NONE, NULL, 0, 0);

					for( row = 0; row < nBlockHeight; row++)
					{
						for( col = 0; col < nBlockWidth; col++)
						{
							//output color0
							lo = pEncodedImage[pixelOffset++];
							hi = pEncodedImage[pixelOffset++];
							ppBlock[col][row]->m_data.Add(hi);
							ppBlock[col][row]->m_data.Add(lo);

							//output color1
							lo = pEncodedImage[pixelOffset++];
							hi = pEncodedImage[pixelOffset++];
							ppBlock[col][row]->m_data.Add(hi);
							ppBlock[col][row]->m_data.Add(lo);

							//output 4 bytes for indices, swap the order of the 4 2-bit indices
							for( int i = 0; i < 4; i++)
							{
								lo = pEncodedImage[pixelOffset++];
								hi =	( ( lo & 0x03) << 6 ) |
										( ( lo & 0x0C) << 2 ) |
										( ( lo & 0x30) >> 2 ) |
										( ( lo & 0xC0) >> 6 );
								ppBlock[col][row]->m_data.Add(hi);
							}
						}
					}
					delete pEncodedImage;

					pixelOffset = 0;
					int blockY,blockX;
					for(y=0;y<height;y++)
					{
						blockY = y>>2;
						for(x=0;x<width;x++)
						{
							blockX = x>>3;
							hi = pImageData[pixelOffset + 3];
							pixelOffset += 4;
							ppAlphaBlock[blockX][blockY]->m_data.Add(hi);
						}
						for(x=0;x<(nAlphaBlockWidth * 8-nLineSize);x++)
						{
							ppAlphaBlock[blockX][blockY]->m_data.Add(padChar);
						}
					}
				}
				else // do the 32 bits per pixel texture (here in case we need it back)
				{
					imageSize = width*height*4;
					assert(imageSize);
					int actualSize;
					tupImageDataMap.GetData((void**)&pImageData,actualSize);
					assert(imageSize==actualSize);
					int blockY,blockX;
					u8 r,g,b,a;
					int pixelOffset = 0;
					for (y=0;y<height;y++)
					{
						blockY = y>>2;
						for (x=0;x<width;x++)
						{
							blockX = x>>2;
							r = pImageData[pixelOffset++];
							g = pImageData[pixelOffset++];
							b = pImageData[pixelOffset++];
							a = pImageData[pixelOffset++];
							ppBlock[blockX][blockY]->m_data.Add(a);
							ppBlock[blockX][blockY]->m_data.Add(r);
							ppBlock[blockX][blockY]->m_data.Add(g);
							ppBlock[blockX][blockY]->m_data.Add(b);
						}
						for (x=0;x<(nBlockWidth*8-nLineSize);x++)
						{
							ppBlock[blockX][blockY]->m_data.Add(padChar);
						}
					}
				}
			}
			break;
		case TupImageDataFrame::RGB_888:
			{
				if(1) // nSourceDataType != SOURCE_DATA_TEXTURE_LIST)
				{
					imageSize = width*height*3;
					assert(imageSize);
					int actualSize;
					tupImageDataMap.GetData((void**)&pImageData,actualSize);
					assert(imageSize==actualSize);

					unsigned int	nBytesEncode = S3TC_GetEncodeSize( width, height, S3TC_ENCODE_RGB_FULL | S3TC_ENCODE_ALPHA_NONE);

					u8 *pEncodedImage;

					S3_TEXTURE s3texIn,s3texOut;

					bCompressed = true;
					nTotalBlockSize = 8;
					memset( &s3texIn, 0, sizeof(S3_TEXTURE));
					s3texIn.lWidth = width;
					s3texIn.lHeight = height;
					s3texIn.lPitch = width * sizeof(ts_bRGB);
					s3texIn.pSurface = pImageData;
					s3texIn.PixelFormat.nARGBBitCount = 24;
					s3texIn.PixelFormat.nRedMask = 0xFF << 0;
					s3texIn.PixelFormat.nGreenMask = 0xFF << 8;
					s3texIn.PixelFormat.nBlueMask = 0xFF << 16;

					pEncodedImage = new u8[nBytesEncode];

					int row,col;
					u8 lo,hi;
					int pixelOffset = 0;

					S3TC_Encode( &s3texIn, &s3texOut, pEncodedImage, S3TC_ENCODE_RGB_FULL | S3TC_ENCODE_ALPHA_NONE, NULL, 0, 0);

					for( row = 0; row < nBlockHeight; row++)
					{
						for( col = 0; col < nBlockWidth; col++)
						{
							//output color0
							lo = pEncodedImage[pixelOffset++];
							hi = pEncodedImage[pixelOffset++];
							ppBlock[col][row]->m_data.Add(hi);
							ppBlock[col][row]->m_data.Add(lo);

							//output color1
							lo = pEncodedImage[pixelOffset++];
							hi = pEncodedImage[pixelOffset++];
							ppBlock[col][row]->m_data.Add(hi);
							ppBlock[col][row]->m_data.Add(lo);

							//output 4 bytes for indices, swap the order of the 4 2-bit indices
							for( int i = 0; i < 4; i++)
							{
								lo = pEncodedImage[pixelOffset++];
								hi =	( ( lo & 0x03) << 6 ) |
										( ( lo & 0x0C) << 2 ) |
										( ( lo & 0x30) >> 2 ) |
										( ( lo & 0xC0) >> 6 );
								ppBlock[col][row]->m_data.Add(hi);
							}
						}
					}
					delete pEncodedImage;
				}
				else // do the 24 bits per pixel texture (here in case we need it back)
				{
					imageSize = width*height*3;
					assert(imageSize);
					int actualSize;
					tupImageDataMap.GetData((void**)&pImageData,actualSize);
					assert(imageSize==actualSize);
					int blockY,blockX;
					u8 r,g,b;
					int pixelOffset = 0;
					for (y=0;y<height;y++)
					{
						blockY = y>>2;
						for (x=0;x<width;x++)
						{
							blockX = x>>2;
							r = pImageData[pixelOffset++];
							g = pImageData[pixelOffset++];
							b = pImageData[pixelOffset++];
							ppBlock[blockX][blockY]->m_data.Add(0xff);
							ppBlock[blockX][blockY]->m_data.Add(r);
							ppBlock[blockX][blockY]->m_data.Add(g);
							ppBlock[blockX][blockY]->m_data.Add(b);
						}
						for (x=0;x<(nBlockWidth*8-nLineSize);x++)
						{
							ppBlock[blockX][blockY]->m_data.Add(padChar);
						}
					}
				}
			}
			break;
		case TupImageDataFrame::RGB_555:
		case TupImageDataFrame::RGB_565:
			{
				imageSize = width*height*2;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData((void**)&pImageData,actualSize);
				assert(imageSize==actualSize);
				unsigned int	nBytesEncode = S3TC_GetEncodeSize( width, height, S3TC_ENCODE_RGB_FULL | S3TC_ENCODE_ALPHA_NONE);

				u8	*pEncodedImage;

				S3_TEXTURE s3texIn, s3texOut;

				bCompressed = true;
				nTotalBlockSize = 8;
				memset( &s3texIn, 0, sizeof(S3_TEXTURE));
				s3texIn.lWidth = width;
				s3texIn.lHeight = height;
				s3texIn.lPitch = width * sizeof(u16);
				s3texIn.pSurface = pImageData;
				s3texIn.PixelFormat.nARGBBitCount = 16;
				if ((formatFlags&TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::RGB_555)
				{
					s3texIn.PixelFormat.nRedMask = 0x1F << 10;
					s3texIn.PixelFormat.nGreenMask = 0x1F << 5;
					s3texIn.PixelFormat.nBlueMask = 0x1F << 0;
				}
				else
				{
					s3texIn.PixelFormat.nRedMask = 0x1F << 11;
					s3texIn.PixelFormat.nGreenMask = 0x3F << 5;
					s3texIn.PixelFormat.nBlueMask = 0x1F << 0;
				}
				pEncodedImage = new u8[nBytesEncode];
				int row,col;
				u8 lo,hi;
				int pixelOffset = 0;

				S3TC_Encode( &s3texIn, &s3texOut, pEncodedImage, S3TC_ENCODE_RGB_FULL | S3TC_ENCODE_ALPHA_NONE, NULL, 0, 0);

				for( row = 0; row < nBlockHeight; row++)
				{
					for( col = 0; col < nBlockWidth; col++)
					{
						//output color0
						lo = pEncodedImage[pixelOffset++];
						hi = pEncodedImage[pixelOffset++];
						ppBlock[col][row]->m_data.Add(hi);
						ppBlock[col][row]->m_data.Add(lo);

						//output color1
						lo = pEncodedImage[pixelOffset++];
						hi = pEncodedImage[pixelOffset++];
						ppBlock[col][row]->m_data.Add(hi);
						ppBlock[col][row]->m_data.Add(lo);

						//output 4 bytes for indices, swap the order of the 4 2-bit indices
						for( int i = 0; i < 4; i++)
						{
							lo = pEncodedImage[pixelOffset++];
							hi =	( ( lo & 0x03) << 6 ) |
									( ( lo & 0x0C) << 2 ) |
									( ( lo & 0x30) >> 2 ) |
									( ( lo & 0xC0) >> 6 );
							ppBlock[col][row]->m_data.Add(hi);
						}
					}
				}
				delete pEncodedImage;
			}
			break;
		case TupImageDataFrame::RGBA_1555:
			{
				imageSize = width*height*2;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData((void**)&pImageData,actualSize);
				assert(imageSize==actualSize);
				int blockY,blockX;
				u8 lo,hi;
				int pixelOffset = 0;
				for (y=0;y<height;y++)
				{
					blockY = y>>2;
					for (x=0;x<width;x++)
					{
						blockX = x>>2;
						lo = pImageData[pixelOffset++];
						hi = pImageData[pixelOffset++];
						ppBlock[blockX][blockY]->m_data.Add(hi);
						ppBlock[blockX][blockY]->m_data.Add(lo);
					}
					for (x=0;x<(nBlockWidth*8-nLineSize);x++)
					{
						ppBlock[blockX][blockY]->m_data.Add(padChar);
					}
				}
			}
			break;
		case TupImageDataFrame::INDEXED_8:
			{
				imageSize = width*height;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData((void**)&pImageData,actualSize);
				assert(imageSize==actualSize);
				DBLUtil::AlignChunkData32Byte();
				u8 hi, lo;
				// save out palette
				bool bSavePalette = true;

				if (mapIndex!=0) // if we are a mip map
				{
					bSavePalette = false; // we are using a parent palette
					// use the same offset as my first map
					textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[frameNum][0].u32PaletteOffset;
					assert(textureChunkRecord.u32PaletteOffset); // this better be set
					// set the separate alpha plane flag the same as my parent
					if (textureChunkRecordArray[frameNum][0].u32Flags & TEX_FLAG_SEPARATE_ALPHA_PLANE)
						textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
				}
				else // first map
				{
					// check if we are doing palette sharing
					if (bAnimation)
					{
						TupperwareScalar *pParentPaletteFrameScalar = pImageDataFrameAgg->FindScalarByKey("Annotate_ParentPaletteFrame");
						if (pParentPaletteFrameScalar)
						{
							bSavePalette = false; // we are using a parent palette
							int parentPaletteFrame = pParentPaletteFrameScalar->GetAsInt();
							assert(parentPaletteFrame<frameNum); // this better be before me
							// use the same offset as my parent
							textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentPaletteFrame][0].u32PaletteOffset;
							assert(textureChunkRecord.u32PaletteOffset); // this better be set
							// set the separate alpha plane flag the same as my parent
							if (textureChunkRecordArray[parentPaletteFrame][0].u32Flags & TEX_FLAG_SEPARATE_ALPHA_PLANE)
								textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
						}
					}
					else
					{
						TupperwareScalar *pParentPaletteReferenceScalar = pImageAgg->FindScalarByKey("Annotate_ParentPaletteReference");
						if (pParentPaletteReferenceScalar)
						{
							bSavePalette = false; // we are using a parent palette
							int parentPaletteReference = pParentPaletteReferenceScalar->GetAsInt();
							TupperwareAggregate *pParentImageAgg = m_tupImagePool.GetImage(parentPaletteReference);
							// get texture id (this is the index into the pTextureChunkRecordArray
							int *pParentTextureIDs = pParentImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt();
							int parentTextureID = pParentTextureIDs[0];  // use first partition for now
							assert(parentTextureID!=-1);
							assert(parentTextureID<frameNum); // this better be before me
							// use the same offset as my parent
							textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentTextureID][0].u32PaletteOffset;
							assert(textureChunkRecord.u32PaletteOffset); // this better be set
							// set the separate alpha plane flag the same as my parent
							if (textureChunkRecordArray[parentTextureID][0].u32Flags & TEX_FLAG_SEPARATE_ALPHA_PLANE)
								textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
						}
					}
				}
				if (bSavePalette)
				{
					textureChunkRecord.u32PaletteOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
					TupperwareAggregate *pImageDataPaletteAgg = tupImageDataFrame.GetImageDataPalette();
					assert(pImageDataPaletteAgg);
					TupImageDataPalette tupImageDataPalette(pImageDataPaletteAgg);
					ts_bRGBA *pPaletteData;
					int numBytes;
					tupImageDataPalette.GetData((void **)&pPaletteData,numBytes);
					assert(pPaletteData);
					assert(numBytes==256*4);
					int numAlphasUsed = pImageAgg->FindScalarByKey("Annotate_PaletteAlphasUsed")->GetAsInt();

					if (numAlphasUsed>2)
					{
						textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
						//Write Out 16-bit GameCube Palette entries	565
						int i;
						for(i=0;i<256;i++)
						{
							u16 u16Pixel = ((pPaletteData[i].R >> 3) << 11) | ((pPaletteData[i].G >> 2) << 5) | ((pPaletteData[i].B >> 3) << 0);
							hi = (u8) (u16Pixel >> 8);
							lo = (u8) u16Pixel;
							DBLWrite::WriteRecord( &hi, pDBLFile);
							DBLWrite::WriteRecord( &lo, pDBLFile);
						}
						DBLUtil::AlignChunkData32Byte();
						for(i=0;i<256;i++)
						{
							u16 u16Pixel = ((pPaletteData[i].R >> 3) << 11) | ((pPaletteData[i].G >> 2) << 5) | ((pPaletteData[i].B >> 3) << 0);
							hi = pPaletteData[i].A;
							lo = 0xff;
							DBLWrite::WriteRecord( &hi, pDBLFile);
							DBLWrite::WriteRecord( &lo, pDBLFile);
						}
					}
					else
					{
						//Write Out 16-bit GameCube Palette entries	5551
						for(int i=0;i<256;i++)
						{
							u16 u16Pixel = ((pPaletteData[i].R >> 3) << 10) | ((pPaletteData[i].G >> 3) << 5) | ((pPaletteData[i].B >> 3) << 0);
							if( pPaletteData[i].A == 0)
							{
								u16Pixel = 0;
							}
							else
							{
								u16Pixel |= 0x8000;
							}
							hi = (u8) (u16Pixel >> 8);
							lo = (u8) u16Pixel;
							DBLWrite::WriteRecord( &hi, pDBLFile);
							DBLWrite::WriteRecord( &lo, pDBLFile);
						}
					}
					DBLUtil::AlignChunkData32Byte();
				}

				int blockY,blockX;
				int pixelOffset = 0;
				for (y=0;y<height;y++)
				{
					blockY = y>>2;
					for (x=0;x<width;x++)
					{
						blockX = x>>3;
						hi = pImageData[pixelOffset++];
						ppBlock[blockX][blockY]->m_data.Add(hi);
					}
					for (x=0;x<(nBlockWidth*8-nLineSize);x++)
					{
						ppBlock[blockX][blockY]->m_data.Add(padChar);
					}
				}
			}
			break;
		case TupImageDataFrame::INDEXED_4:
			{
				imageSize = (width>>1)*height;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData((void**)&pImageData,actualSize);
				assert(imageSize==actualSize);
				DBLUtil::AlignChunkData32Byte();
				u8 hi, lo;
				// save out palette
				bool bSavePalette = true;

				if (mapIndex!=0) // if we are a mip map
				{
					bSavePalette = false; // we are using a parent palette
					// use the same offset as my first map
					textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[frameNum][0].u32PaletteOffset;
					assert(textureChunkRecord.u32PaletteOffset); // this better be set
					// set the separate alpha plane flag the same as my parent
					if (textureChunkRecordArray[frameNum][0].u32Flags & TEX_FLAG_SEPARATE_ALPHA_PLANE)
						textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
				}
				else // first frame
				{
					// check if we are doing palette sharing
					if (bAnimation)
					{
						TupperwareScalar *pParentPaletteFrameScalar = pImageDataFrameAgg->FindScalarByKey("Annotate_ParentPaletteFrame");
						if (pParentPaletteFrameScalar)
						{
							bSavePalette = false; // we are using a parent palette
							int parentPaletteFrame = pParentPaletteFrameScalar->GetAsInt();
							assert(parentPaletteFrame<frameNum); // this better be before me
							// use the same offset as my parent
							textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentPaletteFrame][0].u32PaletteOffset;
							assert(textureChunkRecord.u32PaletteOffset); // this better be set
							// set the separate alpha plane flag the same as my parent
							if (textureChunkRecordArray[parentPaletteFrame][0].u32Flags & TEX_FLAG_SEPARATE_ALPHA_PLANE)
								textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
						}
					}
					else
					{
						TupperwareScalar *pParentPaletteReferenceScalar = pImageAgg->FindScalarByKey("Annotate_ParentPaletteReference");
						if (pParentPaletteReferenceScalar)
						{
							bSavePalette = false; // we are using a parent palette
							int parentPaletteReference = pParentPaletteReferenceScalar->GetAsInt();
							TupperwareAggregate *pParentImageAgg = m_tupImagePool.GetImage(parentPaletteReference);
							// get texture id (this is the index into the pTextureChunkRecordArray
							int *pParentTextureIDs = pParentImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt();
							int parentTextureID = pParentTextureIDs[0];  // use partition 0 for now
							assert(parentTextureID<frameNum); // this better be before me
							// use the same offset as my parent
							textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentTextureID][0].u32PaletteOffset;
							assert(textureChunkRecord.u32PaletteOffset); // this better be set
							// set the separate alpha plane flag the same as my parent
							if (textureChunkRecordArray[parentTextureID][0].u32Flags & TEX_FLAG_SEPARATE_ALPHA_PLANE)
								textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
						}
					}
				}
				if (bSavePalette)
				{
					textureChunkRecord.u32PaletteOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
					TupperwareAggregate *pImageDataPaletteAgg = tupImageDataFrame.GetImageDataPalette();
					assert(pImageDataPaletteAgg);
					TupImageDataPalette tupImageDataPalette(pImageDataPaletteAgg);
					ts_bRGBA *pPaletteData;
					int numBytes;
					tupImageDataPalette.GetData((void **)&pPaletteData,numBytes);
					assert(pPaletteData);
					assert(numBytes==16*4);
					int numAlphasUsed = pImageAgg->FindScalarByKey("Annotate_PaletteAlphasUsed")->GetAsInt();

					if (numAlphasUsed>2)
					{
						textureChunkRecord.u32Flags |= TEX_FLAG_SEPARATE_ALPHA_PLANE;
						//Write Out 16-bit GameCube Palette entries	565
						int i;
						for(i=0;i<16;i++)
						{
							u16 u16Pixel = ((pPaletteData[i].R >> 3) << 11) | ((pPaletteData[i].G >> 2) << 5) | ((pPaletteData[i].B >> 3) << 0);
							hi = (u8) (u16Pixel >> 8);
							lo = (u8) u16Pixel;
							DBLWrite::WriteRecord( &hi, pDBLFile);
							DBLWrite::WriteRecord( &lo, pDBLFile);
						}
						DBLUtil::AlignChunkData32Byte();
						for(i=0;i<16;i++)
						{
							u16 u16Pixel = ((pPaletteData[i].R >> 3) << 11) | ((pPaletteData[i].G >> 2) << 5) | ((pPaletteData[i].B >> 3) << 0);
							hi = pPaletteData[i].A;
							lo = 0xff;
							DBLWrite::WriteRecord( &hi, pDBLFile);
							DBLWrite::WriteRecord( &lo, pDBLFile);
						}
					}
					else
					{
						//Write Out 16-bit GameCube Palette entries	5551
						for(int i=0;i<16;i++)
						{
							u16 u16Pixel = ((pPaletteData[i].R >> 3) << 10) | ((pPaletteData[i].G >> 3) << 5) | ((pPaletteData[i].B >> 3) << 0);
							if( pPaletteData[i].A == 0)
							{
								u16Pixel = 0;
							}
							else
							{
								u16Pixel |= 0x8000;
							}
							hi = (u8) (u16Pixel >> 8);
							lo = (u8) u16Pixel;
							DBLWrite::WriteRecord( &hi, pDBLFile);
							DBLWrite::WriteRecord( &lo, pDBLFile);
						}
					}
					DBLUtil::AlignChunkData32Byte();
				}

				int blockY,blockX;
				int pixelOffset = 0;
				for (y=0;y<height;y++)
				{
					blockY = y>>3;
					for (x=0;x<width;x+=2)
					{
						blockX = x>>3;
						hi = pImageData[pixelOffset++];
						ppBlock[blockX][blockY]->m_data.Add(hi);
					}
					for (x=0;x<(nBlockWidth*8-nLineSize);x++)
					{
						ppBlock[blockX][blockY]->m_data.Add(padChar);
					}
				}
			}
			break;
		default:
			assert(false);
	}

	// fill in the rest of the blocks with the padChar value
	for(x=0;x<nBlockWidth;x++)
	{
		for(y=0;y<nBlockHeight;y++)
		{
			int nSize = ppBlock[x][y]->m_data.GetSize();
			if(nSize<nTotalBlockSize)
			{
				for(int p=0;p<(nTotalBlockSize-nSize);p++)
				{
					ppBlock[x][y]->m_data.Add(padChar);
				}
			}
		}
	}

	if(bSeparateAlpha)
	{
		for(x=0;x<nAlphaBlockWidth;x++)
		{
			for(y=0;y<nAlphaBlockHeight;y++)
			{
				int nSize = ppAlphaBlock[x][y]->m_data.GetSize();
				if(nSize<nAlphaTotalBlockSize)
				{
					for(int p=0;p<(nAlphaTotalBlockSize-nSize);p++)
					{
						ppAlphaBlock[x][y]->m_data.Add(padChar);
					}
				}
			}
		}
	}

	DBLUtil::AlignChunkData32Byte();
	textureChunkRecord.u32TextureOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();

	// Write the Data from each Block out to file
	if(bCompressed)
	{
		bool		bAddPadCol = ( nBlockWidth & 1) == 1,
					bAddPadRow = ( nBlockHeight & 1) == 1;

		nBlockWidth &= ~1;
		nBlockHeight &= ~1;

		int row,col;

		for( row = 0; row < nBlockHeight; row += 2)
		{
			for( col = 0; col < nBlockWidth; col += 2)
			{
				ppBlock[col][row]->WriteBlock(pDBLFile);
				ppBlock[col + 1][row]->WriteBlock(pDBLFile);
				ppBlock[col][row + 1]->WriteBlock(pDBLFile);
				ppBlock[col + 1][row + 1]->WriteBlock(pDBLFile);
			}
			if( bAddPadCol)
			{
				ppBlock[col][row]->WriteBlock(pDBLFile);
				ppBlock[col][row]->WriteBlock(pDBLFile);
				ppBlock[col][row + 1]->WriteBlock(pDBLFile);
				ppBlock[col][row + 1]->WriteBlock(pDBLFile);
			}
		}
		if( bAddPadRow)
		{
			for( col = 0; col < nBlockWidth; col += 2)
			{
				ppBlock[col][row]->WriteBlock(pDBLFile);
				ppBlock[col + 1][row]->WriteBlock(pDBLFile);
				ppBlock[col][row]->WriteBlock(pDBLFile);
				ppBlock[col + 1][row]->WriteBlock(pDBLFile);
			}
			if( bAddPadCol)
			{
				ppBlock[col][row]->WriteBlock(pDBLFile);
				ppBlock[col][row]->WriteBlock(pDBLFile);
				ppBlock[col][row]->WriteBlock(pDBLFile);
				ppBlock[col][row]->WriteBlock(pDBLFile);
			}
		}
	}
	else
	{
		switch(formatFlags&TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
			case TupImageDataFrame::RGB_888:
				{
					for(y=0;y<nBlockHeight;y++)
					{
						for(x=0;x<nBlockWidth;x++)
						{
							ppBlock[x][y]->WriteBlock32Bit(pDBLFile);
						}
					}
				}
				break;
			case TupImageDataFrame::RGBA_1555:
			case TupImageDataFrame::INDEXED_8:
			case TupImageDataFrame::INDEXED_4:
				{
					for(y=0;y<nBlockHeight;y++)
					{
						for(x=0;x<nBlockWidth;x++)
						{
							ppBlock[x][y]->WriteBlock(pDBLFile);
						}
					}
				}
				break;
			default:
				assert(false);
		}
	}

	if(bSeparateAlpha)
	{
		for(y=0;y<nAlphaBlockHeight;y++)
		{
			for(x=0;x<nAlphaBlockWidth;x++)
			{
				ppAlphaBlock[x][y]->WriteBlock(pDBLFile);
			}
		}
	}

	//Cleanup Blocks
	for(x=0;x<nBlockWidth;x++)
	{
		for(y=0;y<nBlockHeight;y++)
		{
			delete ppBlock[x][y];
		}
	}
	// CleanUp 2D Block Pointer Array
	for(x=0;x<nBlockWidth;x++)
	{
		delete[] ppBlock[x];
	}
	delete[] ppBlock;

	//Cleanup Alpha Blocks
	for(x=0;x<nAlphaBlockWidth;x++)
	{
		for(y=0;y<nAlphaBlockHeight;y++)
		{
			delete ppAlphaBlock[x][y];
		}
	}
	// CleanUp Alpha 2D Block Pointer Array
	for(x=0;x<nAlphaBlockWidth;x++)
	{
		delete[] ppAlphaBlock[x];
	}
	delete[] ppAlphaBlock;
}


void ExportTexture::SetGCNTextureRecord(TupperwareAggregate *pImageAgg,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation)
{
	ts_TextureChunkRecord &textureChunkRecord = textureChunkRecordArray[frameNum][mapIndex];

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	TupImage tupImage(pImageAgg);
	char *pImageFileName;
	// get filename for image
	tupImage.GetFilePathName(&pImageFileName);
	// get image data from image manager
	ImageManager &imageManager = m_dataManager.GetImageManager();
	TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
	assert(pImageDataAgg);
	// get the first frame from the image data
	TupImageData tupImageData(pImageDataAgg);
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>0);
	// mark texture as animated if using more than 1 frame and not part of an animated texture set)
	if (!bAnimation && numFrames>1)
		textureChunkRecord.u32Flags |= TEX_FLAG_ANIMATED_TEXTURE;
	TupperwareAggregate *pImageDataFrame = tupImageData.GetImageDataFrame(0);
	assert(pImageDataFrame);
	TupImageDataFrame tupImageDataFrame(pImageDataFrame);
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	// get first map from frame
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	assert(numMaps>0);
	TupperwareAggregate *pImageDataMap = tupImageDataFrame.GetImageDataMap(mapIndex);
	assert(pImageDataMap);
	TupImageDataMap tupImageDataMap(pImageDataMap);
	// get the bitmap data
	int width;
	tupImageDataMap.GetWidth(width);
	int height;
	tupImageDataMap.GetHeight(height);

	textureChunkRecord.s16TextureWidth = (s16) width;
	textureChunkRecord.s16TextureHeight = (s16) height;

	switch(formatFlags&TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::RGBA_8888:
			textureChunkRecord.u32Flags = TEX_FLAG_SIZE_32_BIT | TEX_FLAG_FMT_RGBA;
			break;
		case TupImageDataFrame::RGB_888:
			textureChunkRecord.u32Flags = TEX_FLAG_SIZE_32_BIT | TEX_FLAG_FMT_RGBA;
			break;
		case TupImageDataFrame::RGB_555:
		case TupImageDataFrame::RGB_565:
			textureChunkRecord.u32Flags = TEX_FLAG_SIZE_S3TC;
			break;
		case TupImageDataFrame::RGBA_1555:
			textureChunkRecord.u32Flags = TEX_FLAG_SIZE_16_BIT | TEX_FLAG_FMT_RGBA;
			break;
		case TupImageDataFrame::INDEXED_8:
			textureChunkRecord.u32Flags = TEX_FLAG_SIZE_8_BIT | TEX_FLAG_FMT_CI;
			break;
		case TupImageDataFrame::INDEXED_4:
			textureChunkRecord.u32Flags = TEX_FLAG_SIZE_4_BIT | TEX_FLAG_FMT_CI;
			break;
		default:
			assert(false);
	}

	// mark diffuse textures 
	TupperwareScalar *pDiffuseMapScalar = pImageAgg->FindScalarByKey("Annotate_DiffuseMap");
	if (pDiffuseMapScalar && pDiffuseMapScalar->GetAsInt() == TRUE)
		textureChunkRecord.u32Flags |= TEX_FLAG_DIFFUSE;
	
	// copy name for easier debugging in game engine 
	// copy right part of name
	int startingNameIndex = strlen(pImageFileName)-(TEXTURE_NAME_LENGTH-1);
	if (startingNameIndex<0)
		startingNameIndex=0;

	strncpy(textureChunkRecord.acTextureName, &pImageFileName[startingNameIndex], TEXTURE_NAME_LENGTH);

	/* set mipmap switch distance */
	textureChunkRecord.u16MipmapDistance = 0;
}

