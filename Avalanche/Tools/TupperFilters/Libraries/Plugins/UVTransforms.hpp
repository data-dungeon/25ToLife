////////////////////////////////////////////////////////////////////////////
//
// UVTransforms
//
// Class to for handling the uv transforms
//
////////////////////////////////////////////////////////////////////////////

inline UVTransform::UVTransform(void) :
	m_mapRef(-1),m_imageRef(-1),m_flags(0),
	m_UOffset(0.0f),m_VOffset(0.0f),m_UTiling(1.0f),m_VTiling(1.0f),
	m_imageWidth(0),m_imageHeight(0),m_numFrames(0),m_mapChannel(0)
{
	m_rotationMatrix.Identity();
}

inline UVTransform::UVTransform(const UVTransform &s) :
	m_mapRef(s.m_mapRef),m_imageRef(s.m_imageRef),m_flags(s.m_flags),
	m_UOffset(s.m_UOffset),m_VOffset(s.m_VOffset),
	m_UTiling(s.m_UTiling),m_VTiling(s.m_VTiling),
	m_imageWidth(s.m_imageWidth),m_imageHeight(s.m_imageHeight),m_numFrames(s.m_numFrames),
	m_rotationMatrix(s.m_rotationMatrix),m_mapChannel(s.m_mapChannel)
{
}
	
inline bool UVTransform::operator==(const UVTransform &s) const
{
	return (m_mapRef==s.m_mapRef &&
			m_imageRef==s.m_imageRef &&
			m_flags==s.m_flags &&
			m_UOffset==s.m_UOffset &&
			m_VOffset==s.m_VOffset &&
			m_UTiling==s.m_UTiling &&
			m_VTiling==s.m_VTiling &&
			m_imageWidth==s.m_imageWidth &&
			m_imageHeight==s.m_imageHeight &&
			m_numFrames==s.m_numFrames &&
			m_rotationMatrix==s.m_rotationMatrix &&
			m_mapChannel==s.m_mapChannel);
}

inline bool UVTransform::operator!=(const UVTransform &s) const
{
	return (m_mapRef!=s.m_mapRef ||
			m_imageRef!=s.m_imageRef ||
			m_flags!=s.m_flags ||
			m_UOffset!=s.m_UOffset ||
			m_VOffset!=s.m_VOffset ||
			m_UTiling!=s.m_UTiling ||
			m_VTiling!=s.m_VTiling ||
			m_imageWidth!=s.m_imageWidth ||
			m_imageHeight!=s.m_imageHeight ||
			m_numFrames!=s.m_numFrames ||
			m_rotationMatrix!=s.m_rotationMatrix ||
			m_mapChannel!=s.m_mapChannel);
}

inline UVTransform &UVTransform::operator=(const UVTransform &s)
{
	m_mapRef=s.m_mapRef;
	m_imageRef=s.m_imageRef;
	m_flags=s.m_flags;
	m_UOffset=s.m_UOffset;
	m_VOffset=s.m_VOffset;
	m_UTiling=s.m_UTiling;
	m_VTiling=s.m_VTiling;
	m_imageWidth=s.m_imageWidth;
	m_imageHeight=s.m_imageHeight;
	m_numFrames=s.m_numFrames;
	m_rotationMatrix=s.m_rotationMatrix;
	m_mapChannel=s.m_mapChannel;
	return *this;
}

inline void UVTransform::Init(void)
{
	m_mapRef = -1,
	m_imageRef = -1;
	m_flags = 0;
	m_UOffset = 0.0f;
	m_VOffset = 0.0f;
	m_UTiling = 1.0f;
	m_VTiling = 1.0f;
	m_imageWidth = 0;
	m_imageHeight = 0;
	m_numFrames = 0;
	m_mapChannel = 0;
	m_rotationMatrix.Identity();
}
