#pragma once
#include <maya/MUserData.h>
#include <maya/MVector.h>

class FaceIndexOverlayData : public MUserData
{
public:
    FaceIndexOverlayData() : MUserData() {}; // Constructor, both this class and father class
    MVector camDir; //store camera direction vector
};