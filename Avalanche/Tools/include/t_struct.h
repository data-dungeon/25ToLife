#ifndef __T_STRUCT_H
#define __T_STRUCT_H

/*-----------------8/18/98 10:26AM------------------
 * Includes
 * --------------------------------------------------*/
#include <t_types.h>

/*-----------------8/18/98 10:26AM------------------
 * Defines
 * --------------------------------------------------*/
typedef struct bRBG
{
	t_Byte				R;
	t_Byte				G;
	t_Byte				B;
} ts_bRGB;

typedef struct bRGBA
{
	t_Byte				R;
	t_Byte				G;
	t_Byte				B;
	t_Byte				A;
} ts_bRGBA;

typedef struct	fRGB
{
	float					R;
	float					G;
	float					B;
} ts_fRGB;

typedef struct fRGBA
{
	float					R;
	float					G;
	float					B;
	float					A;
} ts_fRGBA;

#define RGBF_TO_B(flt) ((t_Byte) (flt * 255.0))
#define RGBB_TO_F(byt) (((float) byt) / 255.0)

/*-----------------9/2/98 11:00AM-------------------
 * 3D Coordinate
 * --------------------------------------------------*/
typedef struct d3DCoord
{
	double				X;
   double				Y;
   double				Z;
} ts_d3DCoord;

typedef struct f3DCoord
{
	float			  		X;
	float					Y;
	float					Z;
} ts_f3DCoord;

typedef struct s3DCoord
{
	short					X;
	short					Y;
	short					Z;
} ts_s3DCoord;

/*-----------------9/2/98 11:00AM-------------------
 * 2D Coordinate
 * --------------------------------------------------*/
typedef struct d2DCoord
{
	double				X;
	double				Y;
} ts_d2DCoord;

typedef struct f2DCoord
{
	float					X;
	float					Y;
} ts_f2DCoord;

typedef struct s2DCoord
{
	short					X;
	short					Y;
} ts_s2DCoord;

/*-----------------9/2/98 11:01AM-------------------
 * UV
 * --------------------------------------------------*/
typedef struct fUV
{
	float					U;
	float					V;
} ts_fUV;

typedef struct sUV
{
	short			 		U;
	short					V;
} ts_sUV;


typedef struct Sphere
{
	ts_f3DCoord			C;
	float					R;
} ts_Sphere;

/*-----------------9/25/98 3:42PM-------------------
 * Axis Aligned Bounding Box
 * --------------------------------------------------*/
typedef 	ts_f3DCoord		t_AABB[2];


/*-----------------9/25/98 3:42PM-------------------
 * Basic Sized List
 * --------------------------------------------------*/

typedef struct CSList
{
	unsigned long	ulCount;
	unsigned long	ulSize;
	void 				*pList;
} ts_CSList;

typedef struct CList
{
	unsigned long	ulCount;
	void				*pList;
} ts_CList;

/*-----------------9/29/98 6:05PM-------------------
 * Flat RGB Image structure
 * --------------------------------------------------*/
#define freeRBImage(img)		\
{	if( (img)->pImage)			\
		free( (img)->pImage);	\
	free( (img)); }

// IFlags
#define RB_IMG_RGB			0x00000000
#define RB_IMG_RGBA			0x00000001
#define RB_IMG_PACKED555	0x00000002
#define RB_IMG_PACKED565	0x00000003

typedef struct RBImage
{
	t_lflagbits			IFlags;
	unsigned short		usWidth;
	unsigned short		usHeight;
	union
	{
		ts_bRGB			*pRGBImage;
		ts_bRGBA			*pRGBAImage;
		void				*pImage;
	} ;
} ts_RBImage;


#endif //__T_STRUCT_H
