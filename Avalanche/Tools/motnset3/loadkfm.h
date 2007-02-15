
#include "mpostr.h"
#include "motnset.h"

extern int grandtotalChannelBytes;
extern int grandtotalChannelItemBytes;
extern int grandtotalKeyframeBytes;
extern int grandtotalKeyframeBytesRemoved;
extern int grandtotalInputKeyframes;
extern int grandtotalInputTransFrames;
extern int grandtotal2FrameChannels;

void LoadAndWriteKFM( mpostream &str, char *fileName, ts_motnInf *pMotnInf);
void CleanUpKFMLoads();
