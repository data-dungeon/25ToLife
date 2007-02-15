inline QuadPatch::QuadPatch()
{
}

inline QuadPatch::~QuadPatch()
{
}

inline void QuadPatch::SetCVs(Vector *pCVs)
{
   memcpy(m_cvs,pCVs,sizeof(Vector)*16);
}
