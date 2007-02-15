////////////////////////////////////////////////////////////////////////////
//
// uvgen.h
//
// File to the uv gen class (not part of the max headers)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: uvgen.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 11:34a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Materials, Maps complete - saving all information (except for plugin
 * shaders)
 */

#ifndef __TUP_EXPORT_UVGEN__H
#define __TUP_EXPORT_UVGEN__H

#define NPARAMS 13
#define STDUV_VERSION  6
#define PB_UOFFS	0	
#define PB_VOFFS	1
#define PB_USCL		2
#define PB_VSCL		3
//#define PB_ANGLE	4
#define PB_UANGLE	4
#define PB_VANGLE	5
#define PB_WANGLE	6
#define PB_BLUR		7
#define PB_NSAMT	8
#define PB_NSSIZ	9
#define PB_NSLEV	10
#define PB_NSPHS	11
#define PB_BLUROFFS	12


// OLD Flags
#define OLD_UV_WRAP		(1<<1)
#define OLD_UV_MIRROR	(1<<2)
#define OLD_UV_NOISE	(1<<3)

// NEW Flags
#define UV_NOISE (1<<4)
#define UV_NOISE_ANI (1<<5)

#define ROLL_OPEN  (1<<30)
#define NOISE_OPEN (1<<29)
#define NOCLIP     (1<<28)

extern HINSTANCE hInstance;

#include <arcdlg.h>

static Class_ID stdUVClassID(STDUV_CLASS_ID,0);

static Point3 theAxis[3] = { Point3(1.0f,0.0f,0.0f),Point3(0.0f,1.0f,0.0f),Point3(0.0f,0.0f,1.0f)};

class StdUV;
class StdUVDlg;

class MyArcballCB: public ArcballCallback {
	public:
	StdUVDlg *dlg;
	MyArcballCB() { dlg = NULL; }
	void Init( StdUVDlg *theDlg) { dlg = theDlg; }
	void StartDrag();
	void EndDrag();
	void Drag(Quat q, BOOL buttonUp);
	void CancelDrag();
	void BeingDestroyed();
	};

class StdUVDlg: public ParamDlg {
	public:
	HWND hwmedit;	 // window handle of the materials editor dialog
	IMtlParams *ip;
	StdUV *theGen;	 // current thing being edited.
	HWND hPanel; 	 // Coord pane
	HWND hNoise; 	 // Noise pane
	TimeValue curTime;
	BOOL valid,nvalid;
	BOOL isActive;
	ArcballDialog *arcb;
	MyArcballCB arcCallback;
	Quat qstart;
	ISpinnerControl *duSpin,*dvSpin;
	ISpinnerControl *suSpin,*svSpin;
	ISpinnerControl *uangSpin,*vangSpin,*wangSpin;
	ISpinnerControl *blurSpin,*blurOffsSpin;
	ISpinnerControl *nsAmtSpin,*nsLevSpin,*nsSizSpin;
	ISpinnerControl *mChanSpin;
#ifndef DESIGN_VER
	ISpinnerControl *nsPhsSpin;
#endif // !DESIGN_VER

	StdUVDlg(HWND hwMtlEdit, IMtlParams *imp, StdUV *g); 
	~StdUVDlg();

	BOOL PanelProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	BOOL NoisePanelProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void UpdtMappingList(HWND hDlg);
	void UpdateEnvType();
	void EnableButtons();
	void LoadDialog(BOOL draw);  // stuff params into dialog
	void ActivateDlg(BOOL onOff) { }
	void UpdateMtlDisplay(); 
	void UpdateTrans();	
 	void SetTile(int i);
	void SetMirror(int i);
	void RotateMap();
	void RemoveMapRotate();
	void ArcStartDrag();
	void ArcEndDrag();
	void ArcDrag(Quat q, BOOL buttonUp);
	void ArcCancelDrag();
	void SetRotAngs(Quat q);
	BOOL KeyAtCurTime(int id);
	
	void Invalidate();

	// methods inherited from ParamDlg:
	Class_ID ClassID() {return stdUVClassID;  }
	void SetThing(ReferenceTarget *m);
	ReferenceTarget* GetThing() { return (ReferenceTarget *)theGen; }
	void DeleteThis() { delete this; }	
	void SetTime(TimeValue t);
	void ReloadDialog();
	void UpdateControlFor(int np);
	};

#define NMAPPINGS 4

//--- THESE ARE DEFINED IN STDMAT.H
 //#define UVMAP_EXPLICIT   0
 //#define UVMAP_SPHERE_ENV 1
 //#define UVMAP_CYL_ENV  	2
 //#define UVMAP_SHRINK_ENV 3
 //#define UVMAP_SCREEN_ENV 4


#define TILING (U_WRAP|V_WRAP|U_MIRROR|V_MIRROR)

#ifdef DESIGN_VER
#define NUVWSRC 5

static int listNumFromSrc(int uvsrc) {
	switch(uvsrc) {
		case UVWSRC_EXPLICIT: return 0;
		case UVWSRC_EXPLICIT2: return 1;
		case UVWSRC_OBJXYZ: return 2;
		case UVWSRC_WORLDXYZ: return 3;
		case UVWSRC_GEOXYZ: return 4;
		default: return 0;
		}
	}
#else
#define NUVWSRC 4


static int listNumFromSrc(int uvsrc) {
	switch(uvsrc) {
		case UVWSRC_EXPLICIT: return 0;
		case UVWSRC_EXPLICIT2: return 1;
		case UVWSRC_OBJXYZ: return 2;
		case UVWSRC_WORLDXYZ: return 3;
		default: return 0;
		}
	}
#endif

class StdUV: public StdUVGen {
	public:
#ifdef DESIGN_VER
		IParamBlock *heldParams;
		DWORD oldFlags;
		int oldAxis;
#endif
		friend class StdUVDlg;
		IParamBlock *pblock;
		Interval ivalid;
		DWORD flags;
		float uAngle,vAngle,wAngle;
		float ang_cos,ang_sin;
		float uoffs,voffs;
		float uscl,vscl;
		float blur, blurOffs;
		BOOL hideMapBack;
		BOOL anyUVWRot;
		Matrix3 tmRot;
		Matrix3 tmRotInv;
		int uvAxis;
		int mapSlotType; //MAPSLOT_TEXTURE, MAPSLOT_ENVIRON  
		int envType;	 
		int uvwSrc, mapChannel;
		Matrix3 trans;

		float nsAmt; // noise amplitude
		int nsLev; // number of fractal levels
		float nsSiz; // noise size
		float nsPhs; // noise phase
		StdUVDlg *paramDlg;

		StdUV();
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void GetUV(ShadeContext& sc, Point2& UV,  Point2& dUV);
		void GetUVNoBlur(ShadeContext& sc, Point2& UV,  Point2& dUV);
		AColor EvalUVMap(ShadeContext &sc, MapSampler* samp,  BOOL filter=TRUE);
		float EvalUVMapMono(ShadeContext &sc, MapSampler* samp, BOOL filter=TRUE);
		Point2 EvalDeriv( ShadeContext& sc, MapSampler* samp, BOOL filter=TRUE);
		void GetBumpDP( ShadeContext& sc, Point3& dPdu, Point3& dPdv);
		void TransformUV(Point3& t, Point3& d, BOOL filt);
		int GetSlotType() { return mapSlotType; }
		void InitSlotType(int sType) { 
			SetMapSlotType(sType); 
			if (sType==MAPSLOT_ENVIRON)
				envType = UVMAP_SPHERE_ENV;
			if (sType==MAPSLOT_BACKGROUND)
				envType = UVMAP_SCREEN_ENV;
			if (paramDlg) 
				paramDlg->UpdateEnvType();
			} 

		int GetAxis() { return uvAxis; }  // returns AXIS_UV, AXIS_VW, AXIS_WU
		void SetAxis(int ax) { uvAxis = ax; }  //  AXIS_UV, AXIS_VW, AXIS_WU
		void SetClipFlag(BOOL b) { 
			SetFlag(NOCLIP,!b);
			}
		BOOL GetClipFlag() { 
			return !GetFlag(NOCLIP); 
			}
		void SetHideMapBackFlag(BOOL b) { 
			hideMapBack = b;
			}
		BOOL GetHideMapBackFlag() { 
			return hideMapBack; 
			}
		void SetFlag(ULONG f, ULONG val) { if (val) flags|=f; else flags &= ~f; }
	    int GetFlag(ULONG f) { return (flags&f)?1:0; }

		void SetRollupOpen(BOOL open) { SetFlag(ROLL_OPEN,open); }
		BOOL GetRollupOpen() { return GetFlag(ROLL_OPEN); }

		// Methods from StdUVGen
		void SetCoordMapping(int);
		void SetUOffs(float f, TimeValue t);
		void SetVOffs(float f, TimeValue t);
		void SetUScl(float f,  TimeValue t);
		void SetVScl(float f,  TimeValue t);
		void SetAng(float f,   TimeValue t);
		void SetUAng(float f,   TimeValue t);
		void SetVAng(float f,   TimeValue t);
		void SetWAng(float f,   TimeValue t);
		void SetBlur(float f,  TimeValue t);
		void SetBlurOffs(float f,  TimeValue t); 
		void SetNoiseAmt(float f,  TimeValue t); 
		void SetNoiseSize(float f,  TimeValue t); 
		void SetNoiseLev(int i,  TimeValue t); 
		void SetNoisePhs(float f,  TimeValue t); 
		void SetTextureTiling(int tiling) { flags = (flags&~TILING)| (tiling&TILING);}

		int GetCoordMapping(int) { return envType; }
		float GetUOffs( TimeValue t) { return pblock->GetFloat(PB_UOFFS,t); }
		float GetVOffs( TimeValue t) { return pblock->GetFloat(PB_VOFFS,t); }
		float GetUScl(  TimeValue t) { return pblock->GetFloat(PB_USCL,t); }
		float GetVScl(  TimeValue t) { return pblock->GetFloat(PB_VSCL,t); }
		float GetAng(   TimeValue t) { return pblock->GetFloat(PB_WANGLE,t); }
		float GetUAng(  TimeValue t) { return pblock->GetFloat(PB_UANGLE,t); }
		float GetVAng(  TimeValue t) { return pblock->GetFloat(PB_VANGLE,t); }
		float GetWAng(  TimeValue t) { return pblock->GetFloat(PB_WANGLE,t); }
		float GetBlur(  TimeValue t) { return pblock->GetFloat(PB_BLUR,t); }
		float GetBlurOffs(  TimeValue t) { return pblock->GetFloat(PB_BLUROFFS,t); } 
		float GetNoiseAmt(  TimeValue t) { return pblock->GetFloat(PB_NSAMT,t); } 
		float GetNoiseSize(  TimeValue t) { return pblock->GetFloat(PB_NSSIZ,t); } 
		int GetNoiseLev( TimeValue t) { return pblock->GetInt(PB_NSLEV,t); } 
		float GetNoisePhs(  TimeValue t) { return pblock->GetFloat(PB_NSPHS,t); } 
		int GetTextureTiling() { return flags&TILING; }
		int GetUVWSource() { return uvwSrc; }
		int GetMapChannel () { return mapChannel; }
		void SetUVWSource(int s) { uvwSrc = s; if (paramDlg) paramDlg->UpdateEnvType(); }
		void SetMapChannel (int s) { mapChannel = s; }

		void SetUVAxis(int axis);
		void SetMapSlotType(int n) 
			{
				n&= 1;
				if (mapSlotType==n) 
					return;
				mapSlotType = n; 
				if (mapSlotType==MAPSLOT_ENVIRON) {
					if (envType==UVMAP_EXPLICIT) {
						envType = UVMAP_SPHERE_ENV;
						}
					}
				NotifyChanged();
			}


		int ListNumFromMapType() {
			return (mapSlotType==MAPSLOT_TEXTURE)?listNumFromSrc(uvwSrc):envType-1;
			}

		ULONG Requirements(int subMtlNum){ 
			ULONG req = 0;
			if (mapSlotType==MAPSLOT_TEXTURE) {
				if (uvwSrc==UVWSRC_EXPLICIT&&mapChannel==1) {
			 		req |= MTLREQ_UV; 
					}
				else if (uvwSrc==UVWSRC_EXPLICIT2) {
					req |= MTLREQ_UV2; 
					}
				if (uvwSrc==UVWSRC_WORLDXYZ)
					req |= MTLREQ_WORLDCOORDS;
#ifdef DESIGN_VER
				if (uvwSrc==UVWSRC_GEOXYZ)
					req |= MTLREQ_WORLDCOORDS;
#endif
				}
			else {
				if (envType!=UVMAP_SCREEN_ENV) req |= MTLREQ_VIEW_DEP;
				}
			return req;
			}
		void MappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq) {
			if (mapSlotType==MAPSLOT_TEXTURE) {
				if (uvwSrc==UVWSRC_EXPLICIT) 
					mapreq.Set(mapChannel);
				else if (uvwSrc==UVWSRC_EXPLICIT2)
					mapreq.Set(0);
				}
			}
		int ShiftedAxisLabel(int i);
		int AxisLabel(int i);
		void UpdateTrans();
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t);

		void GetUVTransform(Matrix3 &uvtrans);

		int SymFlags() {return flags&15; }
		void SetSymFlags(int f) { flags&=~15; flags |=  f&15; }

		Class_ID ClassID() {	return stdUVClassID; }
		void GetClassName(TSTR& s);
		void DeleteThis() { delete this; }	

		int NumSubs() { return 1; }  
		Animatable* SubAnim(int i) { return pblock; }
		TSTR SubAnimName(int i);

		// From ref
 		int NumRefs() { return 1; }
		RefTargetHandle GetReference(int i) { return pblock; }
		void SetReference(int i, RefTargetHandle rtarg) { pblock = (IParamBlock*)rtarg; }

		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		void NotifyChanged() {	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);	}
		void RescaleWorldUnits(float f);

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
	};


class StdUVClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 0; }
	void *			Create(BOOL loading) { 	return new StdUV; 	}
	const TCHAR *	ClassName();
	SClass_ID 		SuperClassID() { return UVGEN_CLASS_ID; }
	Class_ID 		ClassID() { return stdUVClassID; }
	const TCHAR* 	Category() { return _T("");  }
	};


#endif