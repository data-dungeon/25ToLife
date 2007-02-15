#pragma once

#define KFM_H

#define KFMVERSION  0x12345678+1

class HashTime;
class HashVector2;
class HashMatrix3x3;

#define MAXFRAMEERROR      0.003f

class KFM
{
public:

	class Keyframe
	{
	public:
		Keyframe()
		{
			m_flags = 0;
		}

		enum INTERPOLATIONMETHOD { HOLD, LINEAR, SPLINE };
		HashTime  m_time;
		double m_value;
		HashVector2 m_it, m_ot;
		union 
		{
			unsigned char m_flags;
			struct 
			{
				unsigned char  m_defaulttangents : 1;
				unsigned char  m_interpmethod    : 2;
			};
		};

		Keyframe &operator = (const Keyframe &other)
		{
			m_time = other.m_time;
			m_value = other.m_value;
			m_it = other.m_it;
			m_ot = other.m_ot;
			m_flags = other.m_flags;
			return *this;
		}
	};

	class ChannelItem
	{
	public:
		enum ChannelIndex
		{
			XTRANS,YTRANS,ZTRANS,XSCALE,YSCALE,ZSCALE,XQUAT,YQUAT,ZQUAT,WQUAT,NUMCHANNELITEMS
		};

	public:
		TupArray<Keyframe> m_keyframes;
		mutable int m_dindex;
		mutable double m_dvalue;
		mutable HashTime m_dtime;

		ChannelItem()
		{
			m_dindex = 0;
			m_dtime.SetTagged(true);
		}

		ChannelItem &operator = (const ChannelItem &other)
		{
			m_keyframes = other.m_keyframes;
			m_dindex = other.m_dindex;
			m_dvalue = other.m_dvalue;
			m_dtime = other.m_dtime;
			return *this;
		}
		double Interpolate(const HashTime &time) const;
      void CalcTangents();

		double InterpolateValue( const HashTime &time, int index) const;

      HashTime GetStartTime();
      HashTime GetEndTime();

	private:
      void CalcTangents(int index);

	};

	class Channel
	{
	public:
		TupString m_name;
		HashMatrix3x3 m_parentToBoneMatrix;
		TupArray<ChannelItem> m_channelItems;
		HashVector3 m_boneDirection,m_rollDirection,m_parentBoneDirection,m_parentRollDirection;

		void ComputeParentToBone();
	}; 

	HashTime m_length;
	TupArray<Channel> m_channels;

	bool ReadKFMFile(const char *pFilename);
	void Clear(void);

   HashTime FindStartTime();
   HashTime FindEndTime();
};

class CubicCoefficients1D 
{
public:
	double a, b, c, d;             // CUBIC COEFFICIENTS f(t) = a*t^3 + b*t^2 + c*t + d
	double fda, fdb, fdc, fdd;     // BUMP FD COEFFICIENTS

	CubicCoefficients1D( double p1, double p4, double r1, double r4 ) {
		//   a     2 -2  1  1   p1
		//   b =  -3  3 -2 -1 * p4
		//   c     0  0  1  0   r1
		//   d     1  0  0  0   r4
		a =  2*p1 + -2*p4 +    r1 +  r4;
		b = -3*p1 +  3*p4 + -2*r1 + -r4;
		c =                    r1      ;
		d =    p1                      ;
	}
	double Evaluate( double t ) { return t*(t*(a*t + b) + c) + d; }
	double Derivative( double t ) { return t*(3*a*t + 2*b) + c; }
	double InitFD( int steps ) {
		//   fda     0          0          0     1       a
		//   fdb  =  delta**3   delta**2   delta 0   *   b
		//   fdc     6*delta**3 2*delta**2 0     0       c
		//   fdd     6*delta**3 0          0     0       d
		double fd12 = 1.0f/steps;
		double fd11 = fd12 * fd12;
		double fd10 = fd12 * fd11;
		double fd20 = 6.0f * fd10;
		double fd21 = 2.0f * fd11;
		fda = d;
		fdb = a*fd10 + b*fd11 + c*fd12;
		fdc = a*fd20 + b*fd21;
		fdd = a*fd20;
		return fda;
	}   
	double BumpFD() { fda+=fdb; fdb+=fdc; fdc+=fdd; return fda; }
	double GetFDValue() { return fda; }
};




