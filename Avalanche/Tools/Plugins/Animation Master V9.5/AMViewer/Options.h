//
class CAMViewerView;
class COptionsDialog : public CDialog
{
   DECLARE_DYNAMIC(COptionsDialog)
public:
   CAMViewerView *m_view;
   BOOL m_bbilinear, m_bskipframes;
   float m_focallength;

   COptionsDialog( CAMViewerView *view, CWnd* pParentWnd=NULL );
private:

   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void DoDataExchange(CDataExchange* pDX);

   DECLARE_MESSAGE_MAP()
};
