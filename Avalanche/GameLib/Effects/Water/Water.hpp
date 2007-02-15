

///////////////////////////////////////////////////////////////////////////////////////
// Water - Get information
///////////////////////////////////////////////////////////////////////////////////////

inline float Water::GetHeight(float x, float z)
{
	Vector3 pos;
	pos.Set(x, 0.0f, z);
	pos = pos * mWorldToLocal;
	return GetHeightLocal(pos.x(), pos.z());
}

inline float Water::GetRawDepthLocal(float x, float z)
{
	return mDepthGrid(x, z);
}

inline float Water::GetRawDepth(float x, float z)
{
	Vector3 pos(x, 0.0f, z);
	pos = pos * mWorldToLocal;
	return GetRawDepthLocal(pos.x(), pos.z());
}

inline float Water::GetDepthLocal(float x, float z)
{
	return GetRawDepthLocal(x, z) + GetHeightLocal(x, z);
}

inline float Water::GetDepth(float x, float z)
{
	Vector3 pos(x, 0.0f, z);
	pos = pos * mWorldToLocal;
	return GetDepthLocal(pos.x(), pos.z());
}

///////////////////////////////////////////////////////////////////////////////////////
// Water - Dampening field
///////////////////////////////////////////////////////////////////////////////////////


inline float Water::DampenFieldValue(float x, float z)
{
	return 
		mDampeningGrid(x, z)
		// this corrects for the 0-255 nature of the dampening values...
		* (1.0f / 255.0f);
}

