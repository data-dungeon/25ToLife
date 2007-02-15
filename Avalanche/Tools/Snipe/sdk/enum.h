#pragma once

// snipeobject types
enum SnipeObjectType
{
   SOT_Unknown                = 0,
   SOT_Project                = 1,
   SOT_Scene                  = 2,
   SOT_Camera                 = 3,
   SOT_SnipeObject            = 4,
   SOT_PolyModel              = 5,
   SOT_Poly                   = 6,
   SOT_Edge                   = 7,
   SOT_PatchModel  	         = 8,
   SOT_Patch                  = 9,
   SOT_Vertex     	         = 10,
   SOT_ContainerObject        = 11,
   SOT_ModelContainer         = 12,
   SOT_CameraContainer        = 13,
   SOT_LightContainer         = 14,
   SOT_SceneContainer         = 15,
   SOT_UV                     = 16,
   SOT_SetObject              = 17,
   SOT_TexCoords              = 18,
   SOT_UVMapSurfaceDriver     = 19,
   SOT_SubDModel              = 20,
   SOT_Unused1                = 21,
   SOT_ImageContainer         = 22,
   SOT_Image                  = 23,
   SOT_Light                  = 24,
   SOT_ParentableObject       = 25,
   SOT_UVMapSurfaceDriverContainer      = 26,
   SOT_HierObject             = 27,
   SOT_SurfaceContainer       = 28,
   SOT_GeometricObjectSurface = 29,
   SOT_PolyModelSurface       = 30,
   SOT_Unused                 = 31,
   SOT_Unused2                = 32,
   SOT_TexCoordsPointerProperty = 33,
   SOT_TexCoordsContainer     = 34,
   SOT_EnvironmentMapSurfaceDriver = 35,
   SOT_BoneContainer          = 36,
   SOT_Surface                = 37,
   SOT_BoolProperty           = 38,
   SOT_IntProperty            = 39,
   SOT_FloatProperty          = 40,
   SOT_Vector2Property        = 41,
   SOT_VectorProperty         = 42,
   SOT_ColorProperty          = 43,
   SOT_TranslateProperty      = 44,
   SOT_ScaleProperty          = 45,
   SOT_RotateProperty         = 46,
   SOT_TransformProperty      = 47,
   SOT_PropertyPointerProperty = 48,
   SOT_SurfaceDriver          = 49,
   SOT_TypeProperty           = 50,
   SOT_TransformableObject    = 51,
   SOT_MotionContainer        = 52,
   SOT_Motion                 = 53,
   SOT_Component              = 54,
   SOT_Bone                   = 55,
   SOT_StringProperty         = 56,
   SOT_PatchEdge              = 57,
   SOT_FileNameProperty       = 58,
   SOT_FileInfoProperty       = 59,
   SOT_InstanceObject         = 60,
   SOT_GeometricObject        = 61,
   SOT_Property               = 62,
   SOT_ClampMapSurfaceDriver  = 63,
   SOT_Transform2Property     = 64,
   SOT_PlaceholderObject      = 65,
   SOT_PatchVertex            = 66,
   SOT_Selection              = 67,
   SOT_SelectionContainer     = 68,
   SOT_SubDPoly               = 69,
   SOT_SubDVertex             = 70,
   SOT_Unused3                = 71,
   SOT_VertexNormal 	         = 72,
   SOT_UVPoly                 = 73,
   SOT_ImagePointerProperty   = 74,
   SOT_Unused4                = 75,
   SOT_MapSurfaceDriver       = 76,
   SOT_UVPatch                = 77,
   SOT_UVEdge                 = 78,
   SOT_UVPatchEdge            = 79,
   SOT_Unused5                = 80,
   SOT_PropertyDriver         = 81,
   SOT_ProjectionMapSurfaceDriver = 82,
   SOT_MotionChannel          = 83,
   SOT_MotionCurveDriver      = 84,
   SOT_MotionKey              = 85,
   SOT_ModelPropertyDriversContainer = 86,
   SOT_BoneDriversContainer = 87,
   SOT_CustomPropertyDriversContainer = 88,
   SOT_VertexMotionDriversContainer = 89,
   SOT_FloatMotionCurveDriver    = 90,
   SOT_BoolMotionCurveDriver     = 91,
   SOT_IntMotionCurveDriver      = 92,
   SOT_VectorMotionCurveDriver   = 93,
   SOT_Vector2MotionCurveDriver  = 94,
   SOT_Vector4MotionCurveDriver  = 95,
   SOT_EulerMotionCurveDriver    = 96,
   SOT_QuaternionMotionCurveDriver = 97,
   SOT_TimeProperty              = 98,
   SOT_PropertyObject            = 99,
   SOT_FloatMotionKey            = 100,
   SOT_SceneObject               = 101,
   SOT_MotionInstanceContainer   = 102,
   SOT_MotionInstance            = 103,
   SOT_RotoscopeContainer        = 104,
   SOT_Rotoscope                 = 105,
   SOT_MotionSet                 = 106,
   SOT_MotionSetContainer        = 107,
   SOT_MotionChannelBiasDriverContainer = 108,
   SOT_MotionChannelBiasObject   = 109,
   SOT_MotionPlaceholder         = 110,
   SOT_PropertySet                = 111,
   SOT_PropertyPoly               = 112,
   SOT_PropertySetContainer       = 113,
   SOT_MotionPlaceholderContainer = 114,
   SOT_VertexHandle               = 115,
   SOT_FloatMotionCurveDriverBiasObject = 116,
   SOT_Vector2MotionCurveDriverBiasObject = 117,
   SOT_VectorMotionCurveDriverBiasObject = 118,
   SOT_EulerMotionCurveDriverBiasObject = 119,
   SOT_QuaternionMotionCurveDriverBiasObject = 120,
   SOT_PropertyVertex           = 121,
   SOT_PropertyEdge                = 122,
   SOT_PropertyPatchComponent      = 123,
   SOT_PropertyPatch               = 124,
   SOT_PropertyPatchEdge           = 125,
   SOT_PatchUV                     = 126,
   SOT_ColorVertexSet              = 127,
   SOT_ColorVertex                 = 128,
   SOT_ColorPatchVertex            = 129,
   SOT_ColorEdge                   = 130,
   SOT_ColorPatchEdge              = 131,
   SOT_ColorPoly                   = 132,
   SOT_ColorPatch                  = 133,
   SOT_ColorVertexSetContainer     = 134,
   SOT_StringMotionKey             = 135,
   SOT_StringMotionCurveDriver     = 136,
   SOT_StringMotionCurveDriverBiasObject = 137,
   SOT_NUMOBJECTS             // Keep last
};

enum SnipeViewType
{
   SVT_Unknown               = 0,
   SVT_CGLBaseView           = 1,
   SVT_CGLHeaderView         = 2,
   SVT_CGL2DView             = 3,
   SVT_CGLImageView          = 4,
   SVT_CGL3DView             = 5,
   SVT_CGLModelView          = 6,
   SVT_CGLTexCoordView       = 7,
   SVT_CGLSceneView          = 8,
   SVT_CGLTimeLineView       = 9,
   SVT_CGLMotionView         = 10,
   SVT_CGLMotionSetView      = 11,
   SVT_CGLSchematicView      = 12,
   SVT_CGLExplorerView       = 13,
   SVT_NUMVIEWS             // Keep last
};

enum SnipeDocType
{
   SDT_Unknown              = 0,
   SDT_CBaseDoc             = 1,
   SDT_C3DDoc               = 2,
   SDT_CModelDoc            = 3,
   SDT_CTexCoordDoc         = 4,
   SDT_CImageDoc            = 5,
   SDT_CSceneDoc            = 6,
   SDT_CTimeLineDoc         = 7,
   SDT_CMotionDoc           = 8,
   SDT_CMotionSetDoc        = 9,
   SDT_NUMDOCS            // Keep last
};

enum ObjectNotificationMsg 
{
   NOTIFYOBJ_DELETE               = 1 << 0, // LPARAM = HSnipeObject *
   NOTIFYOBJ_ADDEDCHILD           = 1 << 1, // LPARAM = HSnipeObject *
   NOTIFYOBJ_REMOVEDCHILD         = 1 << 2, // LPARAM = HSnipeObject *
   NOTIFYOBJ_ADDEDDESCENDANT      = 1 << 3, // LPARAM = HSnipeObject *
   NOTIFYOBJ_REMOVEDDESCENDANT    = 1 << 4, // LPARAM = HSnipeObject *
   NOTIFYOBJ_SELECTIONCHANGED     = 1 << 5, // WPARAM = bSelected, LPARAM = HSnipeObject *
   NOTIFYOBJ_VISIBILITYCHANGED    = 1 << 6, // WPARAM = bVisible, LPARAM = HSnipeObject *
   NOTIFYOBJ_VISIBILITYINTREECHANGED = 1 << 7, // WPARAM = bVisible, LPARAM = HSnipeObject *
   NOTIFYOBJ_ICONCHANGED          = 1 << 8, // LPARAM = HSnipeObject *
   NOTIFYOBJ_ADDEDTOPROJECT       = 1 << 9, // LPARAM = HSnipeObject *
   NOTIFYOBJ_REMOVEDFROMPROJECT   = 1 << 10, // LPARAM = HSnipeObject *
   NOTIFYOBJ_SELECTIONLISTCHANGED = 1 << 11, // WPARAM = bTemporary Selection, LPARAM = HSelectionList *
   NOTIFYOBJ_LOCKEDCHANGED        = 1 << 12, // WPARAM = bLocked, LPARAM = HSnipeObject *

   // Properties
   NOTIFYOBJ_VALUECHANGED      = 1 << 20, // WPARAM = BOOL bStoreUndo, LPARAM = HProperty *
   NOTIFYOBJ_ADDPROPERTY       = 1 << 21, // LPARAM = HProperty *
   
   NOTIFYOBJ_ALL               = 0xFFFFFFFF
};

// LPARAM is always the CGLControl *
enum ControlNotificationMsg {
   NOTIFYCTRL_ADDCHILD,
   NOTIFYCTRL_REMOVECHILD,
   NOTIFYCTRL_SWAPCHILD,

   NOTIFYCTRL_BN_CLICKED,  // WPARAM = ( ID, 0 )
   NOTIFYCTRL_CB_CLICKED,  // WPARAM = ( ID, Index )
   NOTIFYCTRL_CB_CHANGED,  // WPARAM = ( ID, Index )
   NOTIFYCTRL_TB_CHANGED,  // WPARAM = ( ID, Index )
   NOTIFYCTRL_TB_REMOVED,  // WPARAM = ( ID, Index )
   NOTIFYCTRL_EN_CHANGED,  // WPARAM = ( ID, 0 )
   NOTIFYCTRL_EN_KEYDOWN,  // WPARAM = ( ID, 0 )
   NOTIFYCTRL_SB_CHANGED,  // WPARAM = ( ID, nPos );
   NOTIFYCTRL_SB_ENDSCROLL,// WPARAM = ( ID, nPos );
   NOTIFYCTRL_LC_CARETCHANGED, // WPARAM = ( ID, 0 )
};

enum ViewAngleIndex { 
   VIEW_FRONT, 
   VIEW_BACK, 
   VIEW_LEFT, 
   VIEW_RIGHT,
   VIEW_TOP,
   VIEW_BOTTOM,
   VIEW_BIRDSEYE,
   NUM3DVIEWANGLEPRESETS,
   VIEW_FIRSTCAMERA=100,
   VIEW_LASTCAMERA=199,
   VIEW_FIRSTLIGHT=200, 
   VIEW_LASTLIGHT=999,
};

enum UndoType
{ 
   UNDO_SUPPORTED,   // You will support undo
   UNDO_UNNECESSARY, // The command does not support undo, and it does not do anything destructive so undo is unnecessary.
   UNDO_UNSUPPORTED  // Undo is not supported and the command does destructive things, thus the history list needs purged
};

