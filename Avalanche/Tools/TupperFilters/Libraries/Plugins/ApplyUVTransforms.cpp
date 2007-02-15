////////////////////////////////////////////////////////////////////////////
//
// ApplyUVTransforms
//
// Class to handle applying a uv transform to a set of uv coordinates
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ApplyUVTransforms.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 6/23/03    Time: 3:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * math fix
 * 
 * *****************  Version 6  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/03/02   Time: 1:53p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * reversed image - not rotated image
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/02/02   Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Changed UV transform classes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/13/02    Time: 11:58a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/13/02    Time: 11:56a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * apply and build stuff
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_APPLY_UV_TRANSFORMS_H
#define TUPPERFILTERS_LIB_PLUGINS_APPLY_UV_TRANSFORMS_H


#include "PluginsPch.h"
#include "ApplyUVTransforms.h"
#include "UVTransforms.h"

void ApplyUVTransform::Apply(const UVTransform &uvTransform,Vector3 &value)
{
	switch (uvTransform.m_flags & UVTransform::FLAGS_UV_TRANSFORM_TYPE_MASK)
	{
		case UVTransform::FLAGS_UV_TRANSFORM_TYPE_MAX:
			{
				value.y( 1 - value.y( ) ); // adjust back to max original value (tupexport does 1-y)
				value.x( value.x( ) - (uvTransform.m_UOffset + 0.5f));
				value.y( value.y( ) - (uvTransform.m_VOffset + 0.5f));
				value = value*(uvTransform.m_rotationMatrix);

				if (uvTransform.m_VTiling!=1.0)
					float tiling = uvTransform.m_VTiling;

				value.x( value.x( ) * uvTransform.m_UTiling );
				value.y( value.y( ) * uvTransform.m_VTiling );
				value.x( value.x( ) + 0.5f );
				value.y( value.y( ) + 0.5f );

				value.y( 1 - value.y( ) ); // adjust back again
				// swap results if flag set
				if (uvTransform.m_flags & UVTransform::FLAG_REVERSED_IMAGE)
				{
					float temp = value.x( );
					value.x( value.y( ) );
					value.y( temp );
				}
			}
			break;
		case UVTransform::FLAGS_UV_TRANSFORM_TYPE_MAYA:
			{
				value.y( 1 - value.y( ) ); // adjust back to max original value (tupexport does 1-y)
				value.x( value.x( ) - 0.5f); // offset to center
				value.y( value.y( ) - 0.5f);

				value = value*(uvTransform.m_rotationMatrix);

				value.x( value.x( ) + 0.5f ); // move from center
				value.y( value.y( ) + 0.5f );

				value.x( value.x( ) - uvTransform.m_UOffset); // apply offset
				value.y( value.y( ) - uvTransform.m_VOffset);

				value.x( value.x( ) * uvTransform.m_UTiling ); // apply scale
				value.y( value.y( ) * uvTransform.m_VTiling );

				value.y( 1 - value.y( ) ); // adjust back again
				// swap results if flag set
				if (uvTransform.m_flags & UVTransform::FLAG_REVERSED_IMAGE)
				{
					float temp = value.x( );
					value.x( value.y( ) );
					value.y( temp );
				}
			}
			break;
		case UVTransform::FLAGS_UV_TRANSFORM_TYPE_HASH:
			{
				value.x( value.x( ) * uvTransform.m_UTiling ); // apply scale
				value.y( value.y( ) * uvTransform.m_VTiling );

				// swap results if flag set
				if (uvTransform.m_flags & UVTransform::FLAG_REVERSED_IMAGE)
				{
					float temp = value.x( );
					value.x( value.y( ) );
					value.y( temp );
				}

			}
			break;
	}
}
