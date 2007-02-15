//
#include "stdafx.h"
#include "BasicCommandsPlugin.h"
#include "HSnipePlugin.h"
#include "Select/SingleSelectUI.h"
#include "Select/SelectionMethodUI.h"
#include "Select/BoundSelectUI.h"
#include "Select/LassoSelectUI.h"
#include "Select/PolyLassoSelectUI.h"
#include "Select/PaintSelectUI.h"
#include "Select/SelectCommand.h"
#include "Select/SelectManipulator.h"
#include "Select/SelectAllCommand.h"
#include "Select/SelectAllUI.h"
#include "Select/SelectInverseCommand.h"
#include "Select/SelectInverseUI.h"
#include "Select/SelectConnectedCommand.h"
#include "Select/SelectConnectedUI.h"
#include "Select/SelectContinuousCommand.h"
#include "Select/SelectContinuousUI.h"
#include "Select/SelectRingCommand.h"
#include "Select/SelectRingUI.h"
#include "Select/RotateFocusCommand.h"
#include "Select/RotateFocusUI.h"
#include "Select/DeselectAllCommand.h"
#include "Select/DeselectAllUI.h"
#include "Select/LoadSelectionCommand.h"
#include "Select/LoadSelectionUI.h"
#include "Select/SaveSelectionCommand.h"
#include "Select/SaveSelectionUI.h"
#include "Select/GrowSelectionCommand.h"
#include "Select/GrowSelectionUI.h"
#include "Select/SelectionTypeUI.h"
#include "Delete/DeleteCommand.h"
#include "Delete/DeleteUI.h"
#include "Hide/ShowOnlyCommand.h"
#include "Hide/ShowOnlyUI.h"
#include "Hide/HideSelectionCommand.h"
#include "Hide/HideSelectionUI.h"
#include "Hide/ShowSelectionCommand.h"
#include "Hide/ShowSelectionUI.h"
#include "Hide/ShowAllCommand.h"
#include "Hide/ShowAllUI.h"
#include "Lock/LockCommand.h"
#include "Lock/LockUI.h"
#include "SplitPoly/SplitPolyManipulator.h"
#include "AddVertex/AddVertexCommand.h"
#include "AddVertex/AddVertexManipulator.h"
#include "Translate/TranslateCommand.h"
#include "Translate/TranslateManipulator.h"
#include "Scale/ScaleCommand.h"
#include "Scale/ScaleManipulator.h"
#include "Rotate/RotateCommand.h"
#include "Rotate/RotateManipulator.h"
#include "Subdivide/SubdivideCommand.h"
#include "Subdivide/SubdivideUpUI.h"
#include "Subdivide/SubdivideDownUI.h"
#include "ConstrainVertex/ConstrainVertexCommand.h"
#include "ConstrainVertex/ConstrainVertexUI.h"
#include "StoreValue/StoreValueCommand.h"
#include "Redirect/RedirectCommand.h"
#include "Redirect/TerminateUI.h."
#include "Redirect/RedirectUI.h."
#include "Attach/AttachCommand.h"
#include "Collapse/CollapseCommand.h"
#include "Connect/ConnectCommand.h"
#include "Connect/ConnectUI.h"
#include "CreateFace/CreateFaceCommand.h"
#include "CreateFace/CreateFaceUI.h"
#include "CreateFace/DeleteFaceCommand.h"
#include "CreateFace/DeleteFaceUI.h"
#include "Normals/FlipNormalUI.h"
#include "Normals/FlipNormalCommand.h"
#include "Surfaces/CreateSurface/CreateSurfaceCommand.h"
#include "Surfaces/CreateSurface/CreateSurfaceUI.h"
#include "Surfaces/CreateUVMap/CreateUVMapCommand.h"
#include "Surfaces/CreateUVMap/CreateUVMapUI.h"
#include "Surfaces/CreateClampMap/CreateClampMapCommand.h"
#include "Surfaces/CreateClampMap/CreateClampMapUI.h"
#include "Surfaces/CreateProjectionMap/CreateProjectionMapCommand.h"
#include "Surfaces/CreateProjectionMap/CreateProjectionMapUI.h"
#include "Surfaces/CreateEnvironmentMap/CreateEnvironmentMapCommand.h"
#include "Surfaces/CreateEnvironmentMap/CreateEnvironmentMapUI.h"
#include "Motions/CreateMotionInstance/CreateMotionInstanceCommand.h"
#include "Motions/ImportMotion/ImportMotionCommand.h"
#include "Motions/ImportMotion/ImportMotionCommandUI.h"
#include "AddModifiedMotion/AddModifiedMotionCommand.h"
#include "AddModifiedMotion/AddModifiedMotionCommandUI.h"
#include "Convert/ConvertModelCommand.h"
#include "Convert/ConvertModelCommandUI.h"
#include "ReduceMotionKeys/ReduceMotionKeysCommand.h"
#include "ReduceMotionKeys/ReduceMotionKeysCommandUI.h"
#include "AssignVertexToBone/AssignVertexToBoneCommand.h"
#include "AssignVertexToBone/AssignVertexToBoneManipulator.h"
#include "CreateRotoscope/CreateRotoscopeCommand.h"
#include "CreateRotoscope/CreateRotoscopeUI.h"
#include "AddBone/AddBoneCommand.h"
#include "AddBone/AddBoneManipulator.h"
#include "CreateModel/CreateModelCommand.h"
#include "Add/AddManipulatorUI.h"
#include "Parent/ParentCommand.h"
#include "Peak/PeakCommand.h"
#include "Peak/PeakUI.h"
#include "SplitEdge/SplitEdgeCommand.h"
#include "SplitEdge/SplitEdgeUI.h"

extern "C" __declspec(dllexport) void InitSnipePlugin( HSnipePlugin *pSnipePlugin )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   pSnipePlugin->SetName( "SNIPEBASICCOMMANDS" );

///////////////////////////////////////////////////////////////////////////////////
// Commands
///////////////////////////////////////////////////////////////////////////////////

   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SelectCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, TranslateCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ScaleCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, RotateCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SelectAllCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, DeselectAllCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SelectInverseCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SelectConnectedCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SelectContinuousCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SelectRingCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, RotateFocusCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, LoadSelectionCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SaveSelectionCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, StoreValueCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, DeleteCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, HideSelectionCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ShowSelectionCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ShowOnlyCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ShowAllCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, LockCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, AddVertexCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SubdivideCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ConstrainVertexCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, GrowSelectionCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ShrinkSelectionCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, RedirectCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, AttachCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, WeldCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateFaceCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, DeleteFaceCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, FlipNormalCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateSurfaceCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateUVMapCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateClampMapCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateProjectionMapCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateEnvironmentMapCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateMotionInstanceCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ImportMotionCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ConvertModelCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ReduceMotionKeysCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, AssignVertexToBoneCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ConnectCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateRotoscopeCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, CreateModelCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, AddBoneCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, ParentCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, PeakCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, SplitEdgeCommandPlugin );
   ADD_SNIPE_COMMAND_PLUGIN( pSnipePlugin, AddModifiedMotionCommandPlugin );

///////////////////////////////////////////////////////////////////////////////////
// Manipulators
///////////////////////////////////////////////////////////////////////////////////

   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, SelectManipulatorPlugin );
   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, TranslateManipulatorPlugin );
   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, RotateManipulatorPlugin );
   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, ScaleManipulatorPlugin );
   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, TransformManipulatorPlugin );
   // SEPARATOR
   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, AddVertexManipulatorPlugin );
   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, SplitPolyManipulatorPlugin );
   // SEPARATOR
   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, AddBoneManipulatorPlugin );
   ADD_SNIPE_MANIPULATOR_PLUGIN(pSnipePlugin, AssignVertexToBoneManipulatorPlugin );

///////////////////////////////////////////////////////////////////////////////////
// UI
///////////////////////////////////////////////////////////////////////////////////

  // Select toolbar ///////////////////////////////////
   ADD_SNIPE_COMMAND_UI_PLUGIN(pSnipePlugin, VertexSelectionCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN(pSnipePlugin, EdgeSelectionCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN(pSnipePlugin, FaceSelectionCommandUIPlugin );
   // Separator
   ADD_SNIPE_COMMAND_UI_PLUGIN(pSnipePlugin, SingleSelectCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN(pSnipePlugin, BoundSelectCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN(pSnipePlugin, LassoSelectCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN(pSnipePlugin, PolyLassoSelectCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN(pSnipePlugin, PaintSelectCommandUIPlugin );
   // SEPARATOR
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SelectAllCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, DeselectAllCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SelectInverseCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SelectConnectedCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SelectContinuousCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SelectRingCommandUIPlugin );
   // SEPARATOR
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, GrowSelectionCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ShrinkSelectionCommandUIPlugin );
   // SEPARATOR
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, RotateFocusPrevCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, RotateFocusNextCommandUIPlugin );
   // SEPARATOR
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, LoadSelectionReplaceCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, LoadSelectionAddCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, LoadSelectionSubtractCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, LoadSelectionToggleCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SaveSelectionCommandUIPlugin );

   // File Toolbar /////////////////////////////////////////////////////////////////
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ImportMotionCommandUIPlugin );

   // Edit Toolbar /////////////////////////////////////////////////////////////////
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, DeleteCommandUIPlugin );

   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, HideSelectionCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ShowSelectionCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ShowOnlyCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ShowAllCommandUIPlugin );

   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, LockSelectedCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, LockUnselectedCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, UnlockSelectedCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, UnlockAllCommandUIPlugin );

   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ConvertModelCommandUIPlugin );

   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, AddModifiedMotionCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ReduceMotionKeysCommandUIPlugin );

   // Modeling Toolbar
   // No Buttons
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ConstrainVertexCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, CreateSurfaceCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, CreateUVMapCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, CreateClampMapCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, CreateProjectionMapCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, CreateEnvironmentMapCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, CreateRotoscopeCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SplitEdge2CommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SplitEdge3CommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SplitEdge4CommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SplitEdge5CommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SplitEdge6CommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SplitEdge7CommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SplitEdge8CommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SplitEdge9CommandUIPlugin );

   // Beginning of toolbar
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SubdivideModelUpCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SubdivideModelDownCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SubdividePatchUpCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SubdividePatchDownCommandUIPlugin );
   // SEPARATOR
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, RedirectCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, TerminateCommandUIPlugin );
   // SEPARATOR
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, CreateFaceCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, DeleteFaceCommandUIPlugin );
   // SEPARATOR
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, SmoothEdgeCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, PeakEdgeCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, TogglePeakEdgeCommandUIPlugin );
   // SEPARATOR
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, FlipNormalCommandUIPlugin );
   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, ConnectCommandUIPlugin );

   ADD_SNIPE_COMMAND_UI_PLUGIN( pSnipePlugin, AddManipulatorUIPlugin );
}
