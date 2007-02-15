//

#include "stdafx.h"
#include "hxt.h"
#include "SDK/HHashObj.h"
#include "StrClass.h"
#include "F_Protos.h"
#include "Reduce.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(CReduceDialog, CDialog)

CReduceDialog::CReduceDialog( CWnd* pParentWnd ) :
 CDialog( IDD_REDUCE, pParentWnd )
{
   m_errortolerance = GetRegistryValue("Reduce", "Tolerance", 1.0f);
}

BOOL CReduceDialog::OnInitDialog()
{
   CDialog::OnInitDialog();
   return FALSE;
}

void CReduceDialog::DoDataExchange(CDataExchange* pDX)
{
   DDX_Text(pDX, IDC_ERRORTOLERANCE, m_errortolerance );
   DDV_MinMaxFloat(pDX, m_errortolerance, 0, 1000 );
}

void CReduceDialog::OnOK()
{
   CDialog::OnOK();
   SetRegistryValue("Reduce", "Tolerance", m_errortolerance );
}

void CHxtApp::ReduceBranch( HAnimObject *srcobj, float tolerance )
{
   for (HAnimObject *animob=srcobj; animob; animob=(HAnimObject *)animob->GetTreeSibling(HTreeObject::SKIPNONE)) {
      animob->Reduce( tolerance  );
      if (animob->IsPropertyDriver())
         continue;
      HAnimObject *progeny = (HAnimObject *)animob->GetTreeProgeny(HTreeObject::SKIPNONE);
      if (progeny)
         ReduceBranch( progeny, tolerance );
   }
}
