////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Plugins written by Antony Jones for use at Avalanche Software
//
//	UV METHODS:
//		rotateUVs	bool(1 - clockwise, 0 - counter clockwise)
//		scaleUVs	bool(1 - tile up, 0 - tile down), bool(1 - scale in U, 0 - scale in V)
//		flipUVs		bool(1 - flip in U, 0 - flip in V)
//		tileUVs		bool(1 - proportional mapping, 0 - uniform mapping), bool(1 - weld uvs, 0 - don't weld uvs)
//
//	MANIPULATOR METHODS:
//		selectManipHandleX;
//		selectManipHandleY;
//		selectManipHandleZ;
//
//	OBJECT/COMPONENT SELECTION METHODS:
//		invertSelect	bool( 1 - include invisible, 0 - don't include invisible)
//		complimentSelect;
//
//	OBJECT/COMPONENT VISIBLITY METHODS:
//		toggleVisibility;
//
//	POLYGON METHODS:
//		extractPolygons;
//
//	TEXTURE METHODS:
//		highlightShader;
//
////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <maya/MFnPlugin.h>

#include "tileUVs.h"
#include "rotateUVs.h"
#include "scaleUVs.h"
#include "flipUVs.h"
#include "complimentSelect.h"
#include "invertSelect.h"
#include "toggleVisibility.h"
#include "selectManipHandleX.h"
#include "selectManipHandleY.h"
#include "selectManipHandleZ.h"
#include "extractPolygons.h"
#include "highlightShader.h"
#include "test.h"
#include "mergeEdge.h"
#include "splitEdge.h"
#include "consolidateShaders.h"
#include "multilisterToBoxes.h"
#include "createReference.h"
#include "instanceObject.h"
#include "replaceInstances.h"
#include "updateInstances.h"
#include "makeRefLayer.h"
#include "addObjectsToLayer.h"
#include "replaceObjectsWithLayerRef.h"
#include "smoothNormals.h"
#include "makeRefGroup.h"
#include "addObjectsToRefGroup.h"
#include "copyattributes.h"
#include "addkeyvaluetonotes.h"
#include "delKeyValueFromNotes.h"
#include "duplicateObject.h"
#include "selectInstances.h"
#include "removeReference.h"
#include "updateReference.h"
#include "getKeyValueFromNotes.h"
#include "listKeysFromNotes.h"
#include "selectmaniphandleview.h"
#include "attenuateLight.h"
#include "bakeTransform.h"
#include "setBoneOrientToChild.h"
#include "vertexLight.h"
#include "bakeBoneScaleIntoTranslation.h"
#include "makeObjectsNamesSimilar.h"
#include "makeObjectsNamesDifferent.h"
#include "setCameraToView.h"
#include "copyVertexColors.h"
#include "AverageUVs.h"
#include "selectAllByType.h"
#include "bakeAnimation.h"
#include "applyCvColorShader.h"
#include "copyClips.h"
#include "copyCharacter.h"
#include "copyTransform.h"
#include "copyKeys.h"
#include "pasteKeys.h"
#include "gotobindpose.h"
#include "displaceAlongNormals.h"
#include "snapverticestoclosest.h"
#include "rebuildMeshes.h"
#include "duplicatePolygons.h"

MStatus initializePlugin(MObject obj)
{
	MStatus status = MS::kFailure;
	MFnPlugin plugin(obj, "Avalanche Software", "5.0", "Any");

	status = plugin.registerCommand("tileUVs", tileUVs::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("rotateUVs", rotateUVs::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("scaleUVs", scaleUVs::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("flipUVs", flipUVs::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("complimentSelect", complimentSelect::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("invertSelect", invertSelect::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("toggleVisibility", toggleVisibility::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("selectManipHandleX", selectManipHandleX::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("selectManipHandleY", selectManipHandleY::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("selectManipHandleZ", selectManipHandleZ::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("extractPolygons", extractPolygons::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("highlightShader", highlightShader::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("test", test::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("mergeEdge", mergeEdge::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

//	status = plugin.registerCommand("splitEdge", splitEdge::creator);
//	if (!status) {
//		status.perror("registerCommand");
//		return(status);
//	}

	status = plugin.registerCommand("consolidateShaders", consolidateShaders::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("multilisterToBoxes", multilisterToBoxes::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("createReference", createReference::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("instanceObject", instanceObject::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("replaceInstances", replaceInstances::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("updateInstances", updateInstances::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

//	status = plugin.registerCommand("showRefOnly", showRefOnly::creator);
//	if (!status) {
//		status.perror("registerCommand");
//		return(status);
//	}

	status = plugin.registerCommand("makeRefLayer", makeRefLayer::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("addObjectsToLayer", addObjectsToLayer::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("replaceObjectsWithLayerRef", replaceObjectsWithLayerRef::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("smoothNormals", smoothNormals::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("makeRefGroup", makeRefGroup::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("addObjectsToRefGroup", addObjectsToRefGroup::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("copyAttributes", copyAttributes::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("addKeyValueToNotes", addKeyValueToNotes::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("delKeyValueFromNotes", delKeyValueFromNotes::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("duplicateObject", duplicateObject::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("selectInstances", selectInstances::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("copyVertexColors", copyVertexColors::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("removeReference", removeReference::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("updateReference", updateReference::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("getKeyValueFromNotes", getKeyValueFromNotes::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("listKeysFromNotes", listKeysFromNotes::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("selectManipHandleView", selectManipHandleView::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("attenuateLight", attenuateLight::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("bakeTransform", bakeTransform::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("setBoneOrientToChild", setBoneOrientToChild::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("vertexLight", vertexLight::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("bakeBoneScaleIntoTranslation", bakeBoneScaleIntoTranslation::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("makeObjectsNamesSimilar", makeObjectsNamesSimilar::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("makeObjectsNamesDifferent", makeObjectsNamesDifferent::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("setCameraToView", setCameraToView::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("averageUVs", AverageUVs::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("selectAllByType", selectAllByType::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("bakeAnimation", bakeAnimation::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("applyCvColorShader", applyCvColorShader::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("copyClips", copyClips::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("copyCharacter", copyCharacter::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("copyTransform", copyTransform::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("copyKeys", copyKeys::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("pasteKeys", pasteKeys::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("goToBindPose", goToBindPose::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("displaceAlongNormals", displaceAlongNormals::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("snapVerticesToClosest", snapVerticesToClosest::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("rebuildMeshes", rebuildMeshes::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	status = plugin.registerCommand("duplicatePolygons", duplicatePolygons::creator);
	if (!status) {
		status.perror("registerCommand");
		return(status);
	}

	return status;
}

//////////////////////////////////////////////////////////////

MStatus uninitializePlugin(MObject obj)
{
	MStatus status = MS::kFailure;
	MFnPlugin plugin(obj);

	status = plugin.deregisterCommand("tileUVs");
	if (!status)
		return status;

	status = plugin.deregisterCommand("rotateUVs");
	if (!status)
		return status;

	status = plugin.deregisterCommand("scaleUVs");
	if (!status)
		return status;

	status = plugin.deregisterCommand("flipUVs");
	if (!status)
		return status;

	status = plugin.deregisterCommand("complimentSelect");
	if (!status)
		return status;

	status = plugin.deregisterCommand("invertSelect");
	if (!status)
		return status;

	status = plugin.deregisterCommand("toggleVisibility");
	if (!status)
		return status;

	status = plugin.deregisterCommand("selectManipHandleX");
	if (!status)
		return status;

	status = plugin.deregisterCommand("selectManipHandleY");
	if (!status)
		return status;

	status = plugin.deregisterCommand("selectManipHandleZ");
	if (!status)
		return status;

	status = plugin.deregisterCommand("extractPolygons");
	if (!status)
		return status;

	status = plugin.deregisterCommand("highlightShader");
	if (!status)
		return status;

	status = plugin.deregisterCommand("test");
	if (!status)
		return status;
	
	status = plugin.deregisterCommand("mergeEdge");
	if (!status)
		return status;

//	status = plugin.deregisterCommand("splitEdge");
//	if (!status)
//		return status;

	status = plugin.deregisterCommand("consolidateShaders");
	if (!status)
		return status;

	status = plugin.deregisterCommand("multilisterToBoxes");
	if (!status)
		return status;

	status = plugin.deregisterCommand("createReference");
	if (!status)
		return status;

	status = plugin.deregisterCommand("instanceObject");
	if (!status)
		return status;

	status = plugin.deregisterCommand("replaceInstances");
	if (!status)
		return status;

	status = plugin.deregisterCommand("updateInstances");
	if (!status)
		return status;

//	status = plugin.deregisterCommand("showRefOnly");
//	if (!status)
//		return status;

	status = plugin.deregisterCommand("makeRefLayer");
	if (!status)
		return status;

	status = plugin.deregisterCommand("addObjectsToLayer");
	if (!status)
		return status;

	status = plugin.deregisterCommand("replaceObjectsWithLayerRef");
	if (!status)
		return status;

	status = plugin.deregisterCommand("smoothNormals");
	if (!status)
		return status;

	status = plugin.deregisterCommand("makeRefGroup");
	if (!status)
		return status;

	status = plugin.deregisterCommand("addObjectsToRefGroup");
	if (!status)
		return status;

	status = plugin.deregisterCommand("copyAttributes");
	if (!status)
		return status;

	status = plugin.deregisterCommand("addKeyValueToNotes");
	if (!status)
		return status;

	status = plugin.deregisterCommand("delKeyValueFromNotes");
	if (!status)
		return status;

	status = plugin.deregisterCommand("duplicateObject");
	if (!status)
		return status;

	status = plugin.deregisterCommand("selectInstances");
	if (!status)
		return status;

	status = plugin.deregisterCommand("copyVertexColors");
	if (!status)
		return(status);

	status = plugin.deregisterCommand("removeReference");
	if (!status)
		return status;

	status = plugin.deregisterCommand("updateReference");
	if (!status)
		return status;

	status = plugin.deregisterCommand("getKeyValueFromNotes");
	if (!status)
		return status;

	status = plugin.deregisterCommand("listKeysFromNotes");
	if (!status)
		return status;

	status = plugin.deregisterCommand("selectManipHandleView");
	if (!status)
		return status;

	status = plugin.deregisterCommand("attenuateLight");
	if (!status)
		return status;

	status = plugin.deregisterCommand("bakeTransform");
	if (!status)
		return status;

	status = plugin.deregisterCommand("setBoneOrientToChild");
	if (!status)
		return status;

	status = plugin.deregisterCommand("vertexLight");
	if (!status)
		return status;
	
	status = plugin.deregisterCommand("bakeBoneScaleIntoTranslation");
	if (!status)
		return status;

	status = plugin.deregisterCommand("makeObjectsNamesSimilar");
	if (!status)
		return status;

	status = plugin.deregisterCommand("makeObjectsNamesDifferent");
	if (!status)
		return status;

	status = plugin.deregisterCommand("setCameraToView");
	if (!status)
		return status;

	status = plugin.deregisterCommand("averageUVs");
	if (!status)
		return status;

	status = plugin.deregisterCommand("selectAllByType");
	if (!status)
		return status;

	status = plugin.deregisterCommand("bakeAnimation");
	if (!status)
		return status;

	status = plugin.deregisterCommand("applyCvColorShader");
	if (!status)
		return status;

	status = plugin.deregisterCommand("copyClips");
	if (!status)
		return status;

	status = plugin.deregisterCommand("copyCharacter");
	if (!status)
		return status;

	status = plugin.deregisterCommand("copyTransform");
	if (!status)
		return status;

	status = plugin.deregisterCommand("copyKeys");
	if (!status)
		return status;

	status = plugin.deregisterCommand("pasteKeys");
	if (!status)
		return status;

	status = plugin.deregisterCommand("goToBindPose");
	if (!status)
		return status;

	status = plugin.deregisterCommand("displaceAlongNormals");
	if (!status)
		return status;

	status = plugin.deregisterCommand("snapVerticesToClosest");
	if (!status)
		return status;

	status = plugin.deregisterCommand("rebuildMeshes");
	if (!status)
		return status;

	status = plugin.deregisterCommand("duplicatePolygons");
	if (!status)
		return status;

	return status;
}
