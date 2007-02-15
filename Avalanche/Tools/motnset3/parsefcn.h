#include "Game/Animation/MotionDB.h"
#include "Game/Animation/MotCtrl.h"
#include "parse.h"


#define EOF_INDEX				0
#define UNRECOGNIZED_INDEX 1
#define PARSE_START_INDEX	2


extern ts_parseListInf	parseInfArray[];
extern ts_motnSet			glbMotnSet;


mpostream & operator << ( mpostream &str, ts_MFileHdr &Hdr);
mpostream & operator << ( mpostream &str, ts_Quatvw &Quat);
mpostream & operator << ( mpostream &str, Vector3 &Vct);
mpostream & operator << (mpostream &str, ts_MHdr &MHdr);
mpostream & operator << (mpostream &str, ts_motDbEntry &mDbEntry);

mpostream & operator << (mpostream &str, ts_MKeyFrameEngine &MKeyFrame);
mpostream & operator << (mpostream &str, ts_MChannelItem &MChannelItem);
mpostream & operator << (mpostream &str, ts_MChannel &MChannel);



