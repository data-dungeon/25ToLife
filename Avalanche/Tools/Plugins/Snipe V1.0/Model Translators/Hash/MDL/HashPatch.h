
class HPatch;
class CP;
class Model;

class HashPatch 
{
private:
   HPatch      *m_pPatch;
   CArray<CP *> m_cparray;

public:
   HashPatch( Model *pModel );

   HPatch *GetHPatch() { return m_pPatch; }
   UINT GetID();
   UINT GetNumSides();
   void SetNumSides( UINT nNumSides );
   CP *GetCPAt( UINT nIndex );
   void SetCPAt( UINT nIndex, CP *pCP );
};