#ifndef BYTESWAP_H
#define BYTESWAP_H

//template functions to do byte swapping for big endian output
template <class T> inline T & ByteSwap(T &val)
{
	T					in, out;
	unsigned char	*pIn,	*pOut;

	in = val;
	
	pIn = (unsigned char *)&in;
	pOut = (unsigned char *)&out;

	switch (sizeof(T))
	{
		case 2:
			pOut[0] = pIn[1];
			pOut[1] = pIn[0];
			break;

		case 4:
			pOut[0] = pIn[3];
			pOut[1] = pIn[2];
			pOut[2] = pIn[1];
			pOut[3] = pIn[0];
			break;

		default:
			ASSERT(0);
	}

	val = out;
	return val;
}

template <class T> inline T * ByteSwap( T **ptr)
{
	T					*in, *out;
	unsigned char	*pIn,	*pOut;

	in = *ptr;
	pIn = (unsigned char *)&in;
	pOut = (unsigned char *)&out;

	pOut[0] = pIn[3];
	pOut[1] = pIn[2];
	pOut[2] = pIn[1];
	pOut[3] = pIn[0];

	*ptr = out;

	return out;
}

#endif