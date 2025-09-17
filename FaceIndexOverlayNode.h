#pragma once

#include <maya/MPxLocatorNode.h>

class FaceIndexOverlayNode : public MPxLocatorNode
{
public:

    FaceIndexOverlayNode();
	virtual ~FaceIndexOverlayNode() override;

	static void* creator();
	static MStatus Initialize();

	static MTypeId GetTypeId();
	static MString GetTypeName();

	static MString GetDrawDbClassification();
	static MString GetDrawRegistrationId();

	// Attributes
	static MObject inputMeshAttr;
	static MObject showFaceAttr;
	static MObject showEdgeAttr;
	static MObject showVertexAttr;
	static MObject cullBackFacingAttr;
	static MObject facingAngleAttr;
	static MObject textSizeAttr;
	static MObject highlightSelectedAttr;

};
