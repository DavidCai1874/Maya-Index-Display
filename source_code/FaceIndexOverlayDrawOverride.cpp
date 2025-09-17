#include "FaceIndexOverlayDrawOverride.h"
#include "FaceIndexOverlayNode.h"
#include "FaceIndexOverlayData.h"

#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPoint.h>
#include <maya/MColor.h>
#include <maya/MGlobal.h>
#include <maya/MAngle.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MSelectionList.h>
#include <maya/MFnNumericData.h>
#include <maya/MItSelectionList.h>
#include <unordered_set>


using namespace MHWRender;

FaceIndexOverlayDrawOverride::~FaceIndexOverlayDrawOverride()
{

}

DrawAPI FaceIndexOverlayDrawOverride::supportedDrawAPIs() const
{
    return kAllDevices; // Support all draw APIs
}

MPxDrawOverride* FaceIndexOverlayDrawOverride::creator(const MObject& obj)
{
    return new FaceIndexOverlayDrawOverride(obj); // Create a new instance of the draw override
}

bool FaceIndexOverlayDrawOverride::hasUIDrawables() const
{
    return true; // Indicate that this override has UI drawables
}

void FaceIndexOverlayDrawOverride::addUIDrawables(
    const MDagPath& objPath,
    MUIDrawManager& drawManager,
    const MFrameContext& frameContext,
    const MUserData* userData)
{

    const FaceIndexOverlayData* data = dynamic_cast<const FaceIndexOverlayData*>(userData); 
    // const for avoiding modification of userData

    if (!data)
        return;

    MVector camDir = data->camDir;//get camera direction values

    MObject node = objPath.node(); //get the node from objPath

    MPlug plug(node, FaceIndexOverlayNode::inputMeshAttr);
    MObject meshObj;
    plug.getValue(meshObj); //give input mesh value to meshObj

    if (!meshObj.hasFn(MFn::kMesh))
        return;

    //retrieve data from the attributes
    bool showFace = MPlug(node, FaceIndexOverlayNode::showFaceAttr).asBool();
    bool showEdge = MPlug(node, FaceIndexOverlayNode::showEdgeAttr).asBool();
    bool showVertex = MPlug(node, FaceIndexOverlayNode::showVertexAttr).asBool();
    bool cullBack = MPlug(node, FaceIndexOverlayNode::cullBackFacingAttr).asBool();
    int facingAngle = MPlug(node, FaceIndexOverlayNode::facingAngleAttr).asInt();
    float textSize = MPlug(node, FaceIndexOverlayNode::textSizeAttr).asFloat();
    bool highlightSelected = MPlug(node, FaceIndexOverlayNode::highlightSelectedAttr).asBool();

    //sets for hightlighting selected vert/face/edge
    std::unordered_set<int> selectedFaceIndices;
    std::unordered_set<int> selectedEdgeIndices;
    std::unordered_set<int> selectedVertexIndices;

    if (highlightSelected)
    {
        MSelectionList selection;
        MGlobal::getActiveSelectionList(selection); //everything selected in the scene
    
        // 1. Polygon components
        MItSelectionList polyIter(selection, MFn::kMeshPolygonComponent); //loop only selected polygons
        for (; !polyIter.isDone(); polyIter.next())
        {
            MDagPath dagPath;
            MObject component;
            polyIter.getDagPath(dagPath, component);
            //current selected item, selected component in current selection
    
            MFnSingleIndexedComponent compFn(component); //component stores indices of selected polygons
            MIntArray indices;
            compFn.getElements(indices); // take our the indices
            for (unsigned int i = 0; i < indices.length(); ++i)
                selectedFaceIndices.insert(indices[i]);//store indices in selectedFaceIndices set
        }
    
        // 2. Edge components
        MItSelectionList edgeIter(selection, MFn::kMeshEdgeComponent);//only loop selected edges
        for (; !edgeIter.isDone(); edgeIter.next())
        {
            MDagPath dagPath;
            MObject component;
            edgeIter.getDagPath(dagPath, component);
    
            MFnSingleIndexedComponent compFn(component);
            MIntArray indices;
            compFn.getElements(indices);
            for (unsigned int i = 0; i < indices.length(); ++i)
                selectedEdgeIndices.insert(indices[i]);
        }
    
        // 3. Vertex components
        MItSelectionList vertIter(selection, MFn::kMeshVertComponent);//only loop selected vertices
        for (; !vertIter.isDone(); vertIter.next())
        {
            MDagPath dagPath;
            MObject component;
            vertIter.getDagPath(dagPath, component);
    
            MFnSingleIndexedComponent compFn(component);
            MIntArray indices;
            compFn.getElements(indices);
            for (unsigned int i = 0; i < indices.length(); ++i)
                selectedVertexIndices.insert(indices[i]);
        }
    }

    MItMeshPolygon itPoly(meshObj);//get the mesh object


    //Draw Manager Starts here
    drawManager.beginDrawable();

    drawManager.setFontSize(textSize);

    for (; !itPoly.isDone(); itPoly.next()) //loops through each polygon
    {
        if (cullBack)
        {
            MVector normal;
            MStatus status = itPoly.getNormal(normal, MSpace::kWorld);
            if (!status) continue;
            double dot = normal * camDir; //a · b = |a| * |b| * cosθ, dot production
            //normalized normal and camDir, so dot is cosθ
            double cosThreshold = cos(MAngle(facingAngle, MAngle::kDegrees).asRadians());
            if (dot < cosThreshold) continue;

        }



        if (showFace)
        {
            MPoint center = itPoly.center(MSpace::kWorld);
            int faceIndex = itPoly.index();

            if (highlightSelected && selectedFaceIndices.count(faceIndex))
            {
                drawManager.setColor(MColor(1.0f, 0.1f, 0.16f)); //red
            }
            else
            {
                drawManager.setColor(MColor(0.96f, 0.93f, 0.23f)); //yellow
            }

            drawManager.text(center, std::to_string(faceIndex).c_str(), MUIDrawManager::kCenter);
            //draw the face index at the center of the polygon
        }

        if (showEdge) 
        {
            MIntArray edgeList;
            itPoly.getEdges(edgeList);
            int edgeCount = edgeList.length();
            //get the edges of the face


            for (int i = 0; i < edgeCount; ++i) 
            {
                int edgeId = edgeList[i];

                if (highlightSelected && selectedEdgeIndices.count(edgeId))
                    drawManager.setColor(MColor(1.0f, 0.1f, 0.16f));
                else
                    drawManager.setColor(MColor(0.305f, 1.0f, 0.165f)); //green

                MPoint edgeCenter = (itPoly.point(i, MSpace::kWorld) + itPoly.point((i + 1) % edgeCount, MSpace::kWorld)) * 0.5;
                // center = (point1 + point2) / 2

                drawManager.text(edgeCenter, std::to_string(edgeId).c_str(), MUIDrawManager::kCenter);
            }
        }

        if (showVertex) 
        {
            int vtxCount = itPoly.polygonVertexCount();
            for (int i = 0; i < vtxCount; ++i) 
            {
                int vertexId = itPoly.vertexIndex(i);
                MPoint vtxPos = itPoly.point(i, MSpace::kWorld);

                if (highlightSelected && selectedVertexIndices.count(vertexId))
                    drawManager.setColor(MColor(1.0f, 0.1f, 0.16f));
                else
                    drawManager.setColor(MColor(0.388f, 1.0f, 1.0f)); //cyan

                drawManager.text(vtxPos, std::to_string(vertexId).c_str(), MUIDrawManager::kCenter);
            }
        }
    }

    drawManager.endDrawable();
    //Draw Manager Ends here
}



MUserData* FaceIndexOverlayDrawOverride::prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* oldData)
{
    FaceIndexOverlayData* data = dynamic_cast<FaceIndexOverlayData*>(oldData); //try to transform oldData to FaceIndexOverlayData
    if (!data) //if oldData is not FaceIndexOverlayData, create a new one
        data = new FaceIndexOverlayData(); //data is a pointer, because it's newed out
    
    MMatrix camMatrix = cameraPath.inclusiveMatrix();
    MVector dir(camMatrix[2][0], camMatrix[2][1], camMatrix[2][2]); //z-axis is the camera looking direction
    dir.normalize(); 
    //normalize so when times with the normal(which is also a normalized vector), it will not be affected by the length of the vector
    data->camDir = dir;//point the camera direction to camDir data
    return data;
}

FaceIndexOverlayDrawOverride::FaceIndexOverlayDrawOverride(const MObject& obj) :
    MHWRender::MPxDrawOverride(obj, nullptr)
{
    //constructor method, initializes father class MPxDrawOverride too
}
