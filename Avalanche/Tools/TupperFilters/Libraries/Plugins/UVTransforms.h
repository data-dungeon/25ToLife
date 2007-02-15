////////////////////////////////////////////////////////////////////////////
//
// UVTransforms
//
// Class to for handling the uv transforms
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_UV_TRANSFORMS_H
#define TUPPERFILTERS_LIB_PLUGINS_UV_TRANSFORMS_H

#ifndef P_BASETYPE_H
#include <Platform/BaseType.h>
#endif
#ifndef __M_MATRIX_H
#include <Math/Matrix.h>
#endif

class UVTransform
{
public:
	enum UVTransformEnums
	{
		FLAGS_U_WRAP						= 0x00000010,
		FLAGS_V_WRAP						= 0x00000020,
		FLAGS_USE_IMAGE_ALPHA			= 0x00000040, // 0=use rgb intensity 1=use alpha value
		FLAGS_INVERT_ALPHA				= 0x00000080,
		FLAG_REVERSED_IMAGE				= 0x00000100, // swap u & v results (due to reversed image)
		FLAG_IMAGE_HAS_ALPHA				= 0x00000200, // if image has alpha or not
		FLAG_IMAGE_HAS_8BIT_ALPHA		= 0x00000400, // if image has 8 bit alpha or not
		FLAGS_UV_TRANSFORM_TYPE_MASK	= 0x00007000, // how we need to do the transform
		FLAGS_UV_TRANSFORM_TYPE_MAX	= 0x00000000,
		FLAGS_UV_TRANSFORM_TYPE_MAYA	= 0x00001000,
		FLAGS_UV_TRANSFORM_TYPE_HASH	= 0x00002000,
	};

	int m_mapRef;
	int m_imageRef;
	int m_flags;
	float m_UOffset;
	float m_VOffset;
	float m_UTiling;
	float m_VTiling;
	Matrix3x3 m_rotationMatrix;
	int m_imageWidth;
	int m_imageHeight;
	int m_numFrames;
	int m_mapChannel;

	UVTransform(void); // default constructor
	UVTransform(const UVTransform &s); // copy constructor
	bool operator==(const UVTransform &s) const; // equals operator
	bool operator!=(const UVTransform &s) const; // not equals operator
	UVTransform &operator=(const UVTransform &s); // assignment operator
	void Init(void); // initialize method
};

#include "UVTransforms.hpp"

#endif