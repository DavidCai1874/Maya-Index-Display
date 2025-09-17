#include <maya/MDrawRegistry.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

#include "FaceIndexOverlayNode.h"
#include "FaceIndexOverlayDrawOverride.h"

MStatus initializePlugin(MObject pluginObj)
{
    const char* vendor = "David Cai";
    const char* version = "1.0.0";
    const char* reqioredApiVersion = "2022"; //all versions should work, but this is the one I used to test it
    //compile with VS 2019 if using maya 2022

    MStatus status;

    MFnPlugin pluginFn(pluginObj, vendor, version, reqioredApiVersion, &status);

    if (!status)
    {
        MGlobal::displayError("Failed to initialize plugin: " + status.errorString());
        return status;
    }

    MString drawDbClassification = FaceIndexOverlayNode::GetDrawDbClassification();

    status = pluginFn.registerNode( 
        FaceIndexOverlayNode::GetTypeName(),
        FaceIndexOverlayNode::GetTypeId(),
        FaceIndexOverlayNode::creator,
        FaceIndexOverlayNode::Initialize,
        MPxNode::kLocatorNode,
        &drawDbClassification);

    status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
        FaceIndexOverlayNode::GetDrawDbClassification(),
        FaceIndexOverlayNode::GetDrawRegistrationId(),
        FaceIndexOverlayDrawOverride::creator);

    if (!status)
    {
        MGlobal::displayError("Failed to register draw override: " + status.errorString());
        return status;
    }

    return MS::kSuccess;
}

MStatus uninitializePlugin(MObject pluginObj)
{
    MStatus status;

    MFnPlugin pluginFn(pluginObj);

    status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
        FaceIndexOverlayNode::GetDrawDbClassification(),
        FaceIndexOverlayNode::GetDrawRegistrationId()
    );
    if (!status)
    {
        MGlobal::displayError("Failed to deregister draw override: " + status.errorString());
        return status;
    }

    status = pluginFn.deregisterNode(FaceIndexOverlayNode::GetTypeId());
    if (!status)
    {
        MGlobal::displayError("Failed to deregister node: " + status.errorString());
        return status;
    }

    return MS::kSuccess;
}
