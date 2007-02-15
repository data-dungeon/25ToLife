// KB  6/14/00  \Ken85\Plugin\Shaders\AGP\AGPShader.h

#include <afxwin.h>
#include <afxcmn.h>

#include "DLLProp.h"
#include "ShadeDLL.h"
#include "Resource.h"

static DLLPropertyPage shaderpages[] = {
 DLLPropertyPage(IDS_SHADERNAME, IDD_ATTRIBUTES_PAGE),
 DLLPropertyPage(IDS_SHADERNAME2, IDD_ATTRIBUTES_PAGE2)
};

// Avalanche Game Properties Shader (AGP)
class AGPShader : public Shader
{
public:
	int m_project;
	char m_projecttext[100];
	int m_collision;
	char m_collisiontext[100];
	int m_terrain;
	char m_terraintext[100];
	char m_environmappath[MAX_PATH];
	unsigned short int m_environmentmappercent;
	float m_pretess;
	unsigned short int m_tessadj;
	unsigned short int m_layer;
	unsigned int m_flat;
	unsigned int m_dnclip;
	unsigned int m_dnfog;
	unsigned int m_twosided;
	unsigned int m_zbias;
	unsigned int m_lod;
	unsigned int m_environmap;
	unsigned int m_deletepatch;
	unsigned int m_invisible;
	unsigned int m_private;
	unsigned int m_alpha;
	unsigned int m_32bitvc;
	unsigned int m_coloroverride;
	
// CONSTRUCTION/DESTRUCTION
                             AGPShader();
   virtual                  ~AGPShader();

   virtual void              Copy( const DLLPlugin &other ) { *this = (AGPShader &)other; }

// INTERFACE
   virtual UINT              GetIconID() { return NULL; }

   virtual UINT              GetNameID() { return IDS_SHADERNAME; }
   virtual char             *GetClassName() { return "Avalanche\\GameProps"; }

   virtual int               GetNumChannelInfos() { return 0; }
   virtual ChannelInfo      *GetChannelInfos() { return NULL; }

   virtual DLLPropertyPage  *GetPropertyPages(BOOL instance) { return shaderpages; }
   virtual UINT              GetNumPropertyPages() { return sizeof(shaderpages)/sizeof(DLLPropertyPage); }
   virtual void              InitPropertyPage( HWND hwnddlg, UINT dialogid, BOOL instance );
   virtual void              RefreshPropertyPage( HWND hwnddlg, UINT dialogidtoupdate, BOOL instance );

   virtual BOOL              OnCommand( HWND hwnddlg, UINT id );

// Render panel functions
   virtual void              DoDataExchange(CDialog *dialog, CDataExchange* pDX) {}
   virtual void              EnableControls() {}
   virtual BOOL              OnInitDialog(CDialog *dialog) { return FALSE; }
   virtual UINT              GetNumPanelPages() {return 1;}
   virtual UINT              GetPanelPage(int index);
   virtual UINT              GetPanelPageID() { return IDD_ATTRIBUTES_PAGE; }

// SERIALIZATION
   virtual BOOL              Save( char *&label, char *&value, int count );
   virtual BOOL              ParseArg( const char *label, const char *value );

// EVALUATION
   virtual const char       *GetShaderDiffuseFilters();
   virtual const char       *GetShaderSpecularFilters();
// RENDERING
   virtual BOOL              GetSpecular( Shading &shading, Vector &lightcolor );
   virtual void              GetDiffuse( Shading &shading );
};


