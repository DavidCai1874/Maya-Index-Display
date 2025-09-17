#include "FaceIndexOverlayNode.h"
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

static const MTypeId TYPE_ID = MTypeId(0x00123); //random ID, change as needed
static const MString TYPE_NAME = "FaceIndexOverlayNode";

static const MString DRAW_DB_CLASSIFICATION = "drawdb/geometry/FaceIndexOverlayNode";
static const MString DRAW_REGISTRATION_ID = "FaceIndexOverlayNode";

MObject FaceIndexOverlayNode::inputMeshAttr;
MObject FaceIndexOverlayNode::showFaceAttr;
MObject FaceIndexOverlayNode::showEdgeAttr;
MObject FaceIndexOverlayNode::showVertexAttr;
MObject FaceIndexOverlayNode::cullBackFacingAttr;
MObject FaceIndexOverlayNode::facingAngleAttr;
MObject FaceIndexOverlayNode::textSizeAttr;
MObject FaceIndexOverlayNode::highlightSelectedAttr;

FaceIndexOverlayNode::FaceIndexOverlayNode() : MPxLocatorNode()
{

}

FaceIndexOverlayNode::~FaceIndexOverlayNode()
{

}

void* FaceIndexOverlayNode::creator() //factory method to create an instance of this node
{
    return new FaceIndexOverlayNode();
}

MStatus FaceIndexOverlayNode::Initialize()
{
    MFnTypedAttribute tAttr; // use to store complex data types like mesh
    inputMeshAttr = tAttr.create("inputMesh", "inMesh", MFnData::kMesh);
    tAttr.setStorable(true);
    tAttr.setConnectable(true);
    addAttribute(inputMeshAttr);

    MFnNumericAttribute nAttr;

    showFaceAttr = nAttr.create("showFace", "sf", MFnNumericData::kBoolean, true);
    nAttr.setStorable(true); 
	nAttr.setKeyable(true);
    addAttribute(showFaceAttr);

    showEdgeAttr = nAttr.create("showEdge", "se", MFnNumericData::kBoolean, false);
    nAttr.setStorable(true); 
	nAttr.setKeyable(true);
    addAttribute(showEdgeAttr);

    showVertexAttr = nAttr.create("showVertex", "sv", MFnNumericData::kBoolean, false);
    nAttr.setStorable(true); 
	nAttr.setKeyable(true);
    addAttribute(showVertexAttr);

    cullBackFacingAttr = nAttr.create("cullBack", "cb", MFnNumericData::kBoolean, false);
    nAttr.setStorable(true); 
	nAttr.setKeyable(true);
    addAttribute(cullBackFacingAttr);

    facingAngleAttr = nAttr.create("facingAngle", "fa", MFnNumericData::kInt, 60);
    nAttr.setKeyable(true);
    nAttr.setMin(0);
    nAttr.setMax(90);
    addAttribute(facingAngleAttr);

    textSizeAttr = nAttr.create("textSize", "ts", MFnNumericData::kInt, 12);
    nAttr.setKeyable(true);
    nAttr.setMin(1);
    nAttr.setMax(30);
    addAttribute(textSizeAttr);

    highlightSelectedAttr = nAttr.create("highlightSelected", "hlSel", MFnNumericData::kBoolean, false);
    nAttr.setKeyable(true);
    addAttribute(highlightSelectedAttr);

    return MS::kSuccess;
}

MTypeId FaceIndexOverlayNode::GetTypeId()
{
	return TYPE_ID;
}

MString FaceIndexOverlayNode::GetTypeName()
{
	return TYPE_NAME;
}

MString FaceIndexOverlayNode::GetDrawDbClassification()
{
	return DRAW_DB_CLASSIFICATION;
}

MString FaceIndexOverlayNode::GetDrawRegistrationId()
{
	return DRAW_REGISTRATION_ID;
}