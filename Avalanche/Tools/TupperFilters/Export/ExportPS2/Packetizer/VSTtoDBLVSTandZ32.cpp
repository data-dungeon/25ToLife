#include "ExportPch.h"
#include "VSTtoDblVSTandZ32.h"

VSTtoDblVSTandZ32::VSTtoDblVSTandZ32( unsigned int _buildOpts) :
	VSTtoDblBuffer( 12, 4, 2)
{
	_buildOpts = 0;

	setWriters( );
}

void VSTtoDblVSTandZ32::adjustVUMemUsage( unsigned int _VUMemBase, unsigned int _VUMemAvailable)
{
	setVUMemUsage( _VUMemBase, _VUMemAvailable / 2);
}

