#include "ExportPch.h"
#include <assert.h>
#include "DMAControl.h"

ByteAligner DMAControl::stdAlign( 16);

DMAControl::DMAControl( ostream &defstream, bool defTTEEnabled, bool defAlign) :
	d_DMAStart( 0),
	d_DMAEnd( 0),
	d_DMATagPosition( 0),
	d_DMATagPadPosition( 0),
	d_str( defstream)
{
	b_TagWritten = false;
	b_TTEEnabled = defTTEEnabled;
	b_AlignChunk = defAlign;

	p_Align = &stdAlign;
}

void DMAControl::setStreamAligner( OStreamAligner *pAlign)
{
	if( pAlign)
		p_Align = pAlign;
	else
		p_Align = &stdAlign;

}

void DMAControl::writeDMATag( DMATag &Tag, bool bWritePad)
{

	p_DMATag = &Tag;
		
	d_DMATagPosition = d_str.tellp( );

	d_str << Tag;

	d_DMATagPadPosition = d_str.tellp( );

	if( b_TTEEnabled  &&  bWritePad)
	{
		char pad[8] = { 0 } ;
		d_str.write( pad, 8);
	}

	b_TagWritten = true;


}	

void DMAControl::updateDMATag( DMATag *pDMATag)
{
	assert( b_TagWritten);
	assert( p_DMATag);

	streampos CurrentPos = d_str.tellp( );

	d_str.seekp( d_DMATagPosition);

	if( pDMATag)
		p_DMATag = pDMATag;

	d_str << *p_DMATag;

	d_str.seekp( CurrentPos);

}

void DMAControl::seekPadPosition( void)
{

	if( b_TagWritten)
		d_str.seekp( d_DMATagPadPosition);
		
}

void DMAControl::startDMAChunk( void)
{
	assert( p_Align);

	d_DMAStart = d_str.tellp( );

	p_Align->mark( d_str);

}


unsigned int DMAControl::endDMAChunk( void) 
{

	if( b_AlignChunk  &&  p_Align)
		p_Align->align( d_str);

	p_Align = &stdAlign;

	d_DMAEnd = d_str.tellp( );
	
	return (d_DMAEnd - d_DMAStart) / 16;
}


void DMAControl::QWAlign( void)
{
	assert( p_Align);

	p_Align->align( d_str);
}

