#ifndef __VIF_CONTROL_H
#define __VIF_CONTROL_H

#include <ostream>

#include "DMAControl.h"
#include "Display/PS2/Renderer/Common/VIFCreator.h"
#include "VIFDMAAligner.h"

using namespace		std;
using namespace		VIFCreator;

class VIFControl
{
	bool							d_UNPACKinProgress;
	ByteAligner					d_WAlign;
	VIFDMAAligner				d_DMAAlign;

	ostream &					d_str;
	DMAControl &				d_Control;

public:
	VIFControl( DMAControl &defControl);

	ostream &		stream( void) { return d_str; }
	void 				startVIFChunk( void);
	void 				endVIFChunk( void);

	void 				vifSTCYCL( VCNum wl, VCNum cl, VCFlag intFlag = 0 );
	void 				vifMSCAL( VCAddr vuaddr, VCFlag intFlag = 0 );
	void				vifMSCALF( VCAddr vuaddr, VCFlag intFlag = 0);
	void 				vifMSCNT( VCFlag intFlag = 0 );
	void 				vifNOP( VCFlag intFlag = 0 );
	void 				vifUNPACK( VCAddr vuaddr, VIFCreator::VIFUnpackCode cmd, VCNum num, VCFlag useTOPS = 0, 
								VCFlag mask = 0,	VCFlag isUnsigned = 0, VCFlag intFlag = 0  );
	void 				vifFLUSH( VCFlag intFlag = 0 );
	void 				vifFLUSHE( VCFlag intFlag = 0 );
	void 				vifFLUSHA( VCFlag intFlag = 0 );
	void 				vifDIRECT( VCNum size, VCFlag flush = 0, VCFlag intFlag = 0 );
	void 				vifOFFSET( VCNum offset, VCFlag intFlag = 0 );
	void 				vifBASE( VCNum base, VCFlag intFlag = 0 );
	void 				vifITOP( VCAddr vuaddr, VCFlag intFlag = 0 );
	void				vifMODE( VCNum mode, VCFlag intFlag = 0);

	// helper functions to control padding of UNPACK source data to 32bits

	// this startUNPACK simply marks the start of the UNPACK source data
	void				startUNPACK( void );

	// this startUNPACK ends previous UNPACK, writes the vifUNPACK and marks the start of the source data
	void 				startUNPACK( VCAddr vuaddr, VIFCreator::VIFUnpackCode cmd, VCNum num, VCFlag useTOPS = 0, 
								VCFlag mask = 0,	VCFlag isUnsigned = 1, VCFlag intFlag = 0  );

	// pads the UNPACK source data
	void				endUNPACK( void );								

} ;

#endif