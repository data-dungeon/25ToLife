
#include "MotionSetPch.h"
#include "kfm.h"

static char *channelItemNames[] = 
{
	"XTRANS","YTRANS","ZTRANS","XSCALE","YSCALE","ZSCALE","XQUAT","YQUAT","ZQUAT","WQUAT"
};


// KFM::Channel Methods
void KFM::Channel::ComputeParentToBone()
{
	if ( m_parentBoneDirection != HashVector3( 0 ))
	{
		HashMatrix3x3 matA = HashMatrix3x3::VectorsToStandard(m_parentBoneDirection, m_parentRollDirection);
		HashMatrix3x3 matB = HashMatrix3x3::VectorsToStandard(m_boneDirection,m_rollDirection);
		matB = matB.Transpose();
		m_parentToBoneMatrix = matA * matB;
	}
	else
		m_parentToBoneMatrix.SetIdentity();
}

// KFM::ChannelItem Methods
void KFM::ChannelItem::CalcTangents(int index)
{
	HashVector2 pp;

	bool  noit = false;
	double magnitude, itmagnitude, otmagnitude;

	if (index && index < (int)m_keyframes.GetSize()-1) {
		pp.Set( m_keyframes[index-1].m_time.GetSeconds()-m_keyframes[index+1].m_time.GetSeconds(),
			m_keyframes[index-1].m_value-m_keyframes[index+1].m_value );
		if ((magnitude = fabs(pp.x)) == 0) {
			m_keyframes[index].m_it.Set( 0,0 );
			m_keyframes[index].m_ot.Set( 0,0 );
			return;
		}
		pp /= magnitude;
		itmagnitude = m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds();
		otmagnitude = m_keyframes[index].m_time.GetSeconds() - m_keyframes[index-1].m_time.GetSeconds();
		m_keyframes[index].m_it = pp;
		m_keyframes[index].m_it *= itmagnitude;
		m_keyframes[index].m_ot = pp;
		m_keyframes[index].m_ot *= otmagnitude;
	}
	else {
		if (index < (int)m_keyframes.GetSize()-1) {
			m_keyframes[index].m_it.Set( m_keyframes[index].m_time.GetSeconds()-m_keyframes[index+1].m_time.GetSeconds(), m_keyframes[index].m_value-m_keyframes[index+1].m_value );
			m_keyframes[index].m_it *= .5;
		} else 
			noit = true;

		if (index) {
			m_keyframes[index].m_ot.Set( m_keyframes[index-1].m_time.GetSeconds()-m_keyframes[index].m_time.GetSeconds(), m_keyframes[index-1].m_value-m_keyframes[index].m_value );
			m_keyframes[index].m_ot *= .5;
			if (noit)
				m_keyframes[index].m_it = m_keyframes[index].m_ot;
		} else
			m_keyframes[index].m_ot = m_keyframes[index].m_it;
	}
}

double KFM::ChannelItem::InterpolateValue( const HashTime &time, int index) const
{
	switch (m_keyframes[index].m_interpmethod) 
	{
	case Keyframe::HOLD:
		return m_keyframes[index].m_value;

	case Keyframe::LINEAR:
		return m_keyframes[index].m_value + (time.GetSeconds() - m_keyframes[index].m_time.GetSeconds())
			/(m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds()) 
			*(m_keyframes[index+1].m_value - m_keyframes[index].m_value);

	case Keyframe::SPLINE:
	default: 
		{
			double evalx = time.GetSeconds();
			CubicCoefficients1D xcoeff(m_keyframes[index].m_time.GetSeconds(), m_keyframes[index+1].m_time.GetSeconds(),-m_keyframes[index].m_it.x,-m_keyframes[index+1].m_ot.x ),
				ycoeff(m_keyframes[index].m_value,m_keyframes[index+1].m_value,-m_keyframes[index].m_it.y,-m_keyframes[index+1].m_ot.y );

			double xspan = m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds();
			double guesst = (evalx - m_keyframes[index].m_time.GetSeconds()) / xspan;
			double diffx = evalx - xcoeff.Evaluate( guesst );
			double error = fabs( diffx );
			double lasterror = error;
			//         double maxerror = MAXFRAMEERROR / CMDLViewerDoc::m_fps;
			double maxerror = MAXFRAMEERROR / 300.0f;

			while (error > maxerror) 
			{
				guesst = guesst + diffx / xcoeff.Derivative(guesst);
				diffx = evalx - xcoeff.Evaluate( guesst );

				error = fabs( diffx );
				if ( error > lasterror ) 
				{  // NOT CONVERGING, PROBABLY BOGUS CHANNEL DATA, WALK USING BUMP
					maxerror *= 10;  // DON'T BE AS ACCURATE BECAUSE THIS IS MUCH SLOWER
					int steps = (int)(xspan / maxerror);
					double deltat = 1.0f / steps;
					xcoeff.InitFD( steps );
					int i;
					diffx = error;
					for ( i=0, guesst=0.0; diffx>maxerror && i<steps; guesst+=deltat, i++ )
						diffx = fabs(evalx - xcoeff.BumpFD());
					break;
				}
				lasterror = error;
			}
			return ycoeff.Evaluate( guesst );
		}
	}
}

double KFM::ChannelItem::Interpolate(const HashTime &time) const
{
	int	s = m_keyframes.GetSize();
	HashTime t = m_keyframes[s-1].m_time;

	if (m_keyframes[m_keyframes.GetSize()-1].m_time <= time)
		return m_keyframes[m_keyframes.GetSize()-1].m_value;

	if (m_keyframes[0].m_time >= time)
		return m_keyframes[0].m_value;

	if (time == m_dtime)
		return m_dvalue;

	if (m_keyframes[m_dindex].m_time <= time) 
	{
		for (int index = m_dindex; index < (int)m_keyframes.GetSize()-1; index++) 
		{
			if (m_keyframes[index].m_time == time) 
			{
				m_dindex = index;
				m_dvalue = m_keyframes[index].m_value;
				break;
			} 
			else if (m_keyframes[index+1].m_time == time ) 
			{
				m_dindex = index;
				m_dvalue = m_keyframes[index+1].m_value;
				break;
			} 
			else if (m_keyframes[index+1].m_time > time) 
			{
				m_dindex = index;
				m_dvalue = InterpolateValue( time, index );
				break;
			}
		}
	}
	else 
	{
		for (int index = m_dindex-1; index>=0; index--) 
		{
			if (m_keyframes[index].m_time == time) 
		 {
			 m_dindex = index;
			 m_dvalue = m_keyframes[index].m_value;
			 break;
		 }
			else if (m_keyframes[index+1].m_time == time) 
		 {
			 m_dindex = index;
			 m_dvalue = m_keyframes[index+1].m_value;
			 break;
		 }
			else if (m_keyframes[index].m_time < time) 
		 {
			 m_dindex = index;
			 m_dvalue = InterpolateValue( time, index );
			 break;
		 }
		}
	}
	m_dtime = time;
	return m_dvalue;
}

#define IF_ERROR(ERR)						\
	if (!ERR)								\
{										\
	fclose(fp);							\
	return false;							\
}

#define EOF_ERROR()								\
	if (feof(fp))							\
{										\
	fclose(fp);							\
	return false;							\
}

static bool ReadHashVector3(FILE *fp,HashVector3 &vect)
{
	float x,y,z;
	fread(&x,sizeof(float),1,fp);
	if (feof(fp))
		return false;
	fread(&y,sizeof(float),1,fp);
	if (feof(fp))
		return false;
	fread(&z,sizeof(float),1,fp);
	if (feof(fp))
		return false;
	vect.x = x;
	vect.y = y;
	vect.z = z;
	return true;
}

static bool ReadHashVector2(FILE *fp,HashVector2 &vect)
{
	float x,y;
	fread(&x,sizeof(float),1,fp);
	if (feof(fp))
		return false;
	fread(&y,sizeof(float),1,fp);
	if (feof(fp))
		return false;
	vect.x = x;
	vect.y = y;
	return true;
}

static bool ReadString(FILE *fp,char *str,int len)
{
	char c;
	int cnt = 0;
	do 
	{
		c = fgetc(fp);
		str[cnt++] = c;
		if (cnt == len)
			break;
		if (feof(fp))
			return false;
	} while (c!='\0');
	return true;
}


// KFM Methods
bool KFM::ReadKFMFile(const char *filename)
{
	int version;
	unsigned short channelcount;
	float seconds;
	char line[256];
	FILE *fp;
	int err;

	fp = fopen(filename,"rb");

	if (fp == NULL)
		return false;

	fread(&version,sizeof(int),1,fp);
	EOF_ERROR();

	if (version != KFMVERSION)
		return 0;

	fread(&seconds,sizeof(float),1,fp);
	EOF_ERROR();

	m_length.SetSeconds(seconds);

	fread(&channelcount,sizeof(unsigned short),1,fp);
	EOF_ERROR();

	m_channels.RemoveAll();

	m_channels.SetSize(channelcount);

	for (int channelIndex = 0; channelIndex < channelcount; channelIndex++)
	{
		KFM::Channel &channel = m_channels[channelIndex];
		channel.m_channelItems.SetSize(KFM::ChannelItem::NUMCHANNELITEMS);

		err = ReadString(fp,line,256);
		IF_ERROR(err);

		channel.m_name = line;

		err = ReadHashVector3(fp,channel.m_boneDirection);
		IF_ERROR(err);
		err = ReadHashVector3(fp,channel.m_rollDirection);
		IF_ERROR(err);
		err = ReadHashVector3(fp,channel.m_parentBoneDirection);
		IF_ERROR(err);
		err = ReadHashVector3(fp,channel.m_parentRollDirection);
		IF_ERROR(err);

		channel.ComputeParentToBone();

		for (int channelItemIndex = 0; channelItemIndex < KFM::ChannelItem::NUMCHANNELITEMS; channelItemIndex++)
		{
			KFM::ChannelItem &channelItem = channel.m_channelItems[channelItemIndex];
			unsigned short numKeyFrames;
			fread(&numKeyFrames,sizeof(unsigned short),1,fp);// Number of keyframes
			EOF_ERROR();

			channelItem.m_keyframes.SetSize(numKeyFrames);

			if ( numKeyFrames == 0 )
			{
				printf( "WARNING: bone '%s' channel %d has no key frames\n", channel.m_name, channelItemIndex );
			}
			
			float floatValue;
			int keyFrameIndex;
			for (keyFrameIndex = 0; keyFrameIndex < numKeyFrames; keyFrameIndex++)
			{
				Keyframe &keyframe = channelItem.m_keyframes[keyFrameIndex];

				fread(&seconds,sizeof(float),1,fp);
				EOF_ERROR();

				keyframe.m_time.SetSeconds(seconds);

				fread(&floatValue,sizeof(float),1,fp);
				EOF_ERROR();
				keyframe.m_value = floatValue;

				fread(&keyframe.m_flags,sizeof(unsigned char),1,fp);
				EOF_ERROR();

				if (!keyframe.m_defaulttangents)
				{
					err = ReadHashVector2(fp,keyframe.m_it);
					IF_ERROR(err);
					err = ReadHashVector2(fp,keyframe.m_ot);
					IF_ERROR(err);
				}
			}
         channelItem.CalcTangents();

			// build default tangents now
//			for (keyFrameIndex = 0; keyFrameIndex < numKeyFrames; keyFrameIndex++)
//			{
//				Keyframe &keyframe = channelItem.m_keyframes[keyFrameIndex];
//				if (keyframe.m_defaulttangents)
//					channelItem.CalcTangents(keyFrameIndex);
//			}
		}
#if 0
		if (!stricmp(channel.m_name,"main"))
		{
			printf("Main bone KFM data\n");
			printf("Bone Direction %g %g %g\n",boneDirection.x,boneDirection.y,boneDirection.z);
			printf("Roll Direction %g %g %g\n",rollDirection.x,rollDirection.y,rollDirection.z);
			printf("Parent Bone Direction %g %g %g\n",parentBoneDirection.x,parentBoneDirection.y,parentBoneDirection.z);
			printf("Parent Roll Direction %g %g %g\n",parentRollDirection.x,parentRollDirection.y,parentRollDirection.z);
			printf("Parent To Bone Matrix\n");
			printf("%g %g %g\n%g %g %g\n%g %g %g\n",
				channel.m_boneToParentMatrix.GetXAxis().x,
				channel.m_boneToParentMatrix.GetXAxis().y,
				channel.m_boneToParentMatrix.GetXAxis().z,
				channel.m_boneToParentMatrix.GetYAxis().x,
				channel.m_boneToParentMatrix.GetYAxis().y,
				channel.m_boneToParentMatrix.GetYAxis().z,
				channel.m_boneToParentMatrix.GetZAxis().x,
				channel.m_boneToParentMatrix.GetZAxis().y,
				channel.m_boneToParentMatrix.GetZAxis().z);

			for (int channelItemIndex = 0; channelItemIndex < KFM::ChannelItem::NUMCHANNELITEMS; channelItemIndex++)
			{
				KFM::ChannelItem &channelItem = channel.m_channelItems[channelItemIndex];
				int numKeyFrames = channelItem.m_keyframes.GetSize();

				printf("Channel Item %s - Number of Keys %d\n",channelItemNames[channelItemIndex],numKeyFrames);

				for (int keyFrameIndex = 0; keyFrameIndex < numKeyFrames; keyFrameIndex++)
				{
					Keyframe &keyframe = channelItem.m_keyframes[keyFrameIndex];
					TupString flagString;
					if (keyframe.m_defaulttangents)
						flagString += "Default Tangents ";
					switch (keyframe.m_interpmethod)
					{
						case Keyframe::HOLD:
							flagString+="Hold";
							break;
						case Keyframe::LINEAR:
							flagString+="Linear";
							break;
						case Keyframe::SPLINE:
							flagString+="Spline";
							break;
					}
					printf("Key %d Time %g Value %g In %g,%g out %g,%g %s\n",
						keyFrameIndex,
						keyframe.m_time.GetSeconds(),
						keyframe.m_value,
						keyframe.m_it.x,
						keyframe.m_it.y,
						keyframe.m_ot.x,
						keyframe.m_ot.y,
						(const char *) flagString);
				}
			}
		}
#endif
	}

	fclose(fp);

	return true;
}

void KFM::Clear(void)
{
	m_length.m_ticks = 0;
	m_channels.RemoveAll();
}

HashTime KFM::ChannelItem::GetStartTime()
{
   HashTime time;
   if (m_keyframes.GetSize()>0)
      time = m_keyframes[0].m_time;
   return time;
}

void KFM::ChannelItem::CalcTangents()
{
   for (int i = 0; i < m_keyframes.GetSize(); i++)
      if (m_keyframes[i].m_defaulttangents)
         CalcTangents(i);
}

HashTime KFM::ChannelItem::GetEndTime()
{
   HashTime time;
   if (m_keyframes.GetSize()>0)
      time = m_keyframes[m_keyframes.GetSize()-1].m_time;
   return time;
}

HashTime KFM::FindStartTime()
{
   HashTime minStartTime;
   for (int nChannelIndex = 0; nChannelIndex < m_channels.GetSize(); nChannelIndex++)
   {
      Channel *pChannel = &m_channels[nChannelIndex];
      for (int nChannelItemIndex = 0; nChannelItemIndex < pChannel->m_channelItems.GetSize(); nChannelItemIndex++)
      {
         ChannelItem *pChannelItem = &pChannel->m_channelItems[nChannelItemIndex];
         HashTime startTime = pChannelItem->GetStartTime();
         if ((nChannelIndex==0)&&(nChannelItemIndex))
            minStartTime = startTime;
         else
         {
            if (startTime < minStartTime)
               minStartTime = startTime;
         }
      }
   }
   return minStartTime;
}

HashTime KFM::FindEndTime()
{
   HashTime maxEndTime;
   for (int nChannelIndex = 0; nChannelIndex < m_channels.GetSize(); nChannelIndex++)
   {
      Channel *pChannel = &m_channels[nChannelIndex];
      for (int nChannelItemIndex = 0; nChannelItemIndex < pChannel->m_channelItems.GetSize(); nChannelItemIndex++)
      {
         ChannelItem *pChannelItem = &pChannel->m_channelItems[nChannelItemIndex];
         HashTime endTime = pChannelItem->GetEndTime();
         if ((nChannelIndex==0)&&(nChannelItemIndex))
            maxEndTime = endTime;
         else
         {
            if (endTime> maxEndTime)
               maxEndTime = endTime;
         }
      }
   }
   return maxEndTime;
}

