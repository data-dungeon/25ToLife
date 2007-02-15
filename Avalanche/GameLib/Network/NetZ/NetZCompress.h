#ifndef _NETZCOMPRESS_H
#define _NETZCOMPRESS_H


#include <plugins/compression/interface/compressiondecl.h>
#include <plugins/compression/interface/compressionalgorithm.h>
#include <platform/core/criticalsection.h>
#include <platform/core/type.h>

USING_NAMESPACE_QUAZAL;

//group=Compression


class CNetZCompress: public CompressionAlgorithm
{
public:
	//group=Methods

	CNetZCompress();
	virtual ~CNetZCompress();

	void CompressImpl(const Buffer &oInBuffer, Buffer *pOutBuffer); 	
	void DecompressImpl(const Buffer &oInBuffer, Buffer *pOutBuffer);

private:
};

#endif
