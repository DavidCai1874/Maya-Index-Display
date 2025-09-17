#pragma once

#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>

class FaceIndexOverlayDrawOverride : public MHWRender::MPxDrawOverride
{
public:

    virtual ~FaceIndexOverlayDrawOverride() override;

    virtual MHWRender::DrawAPI supportedDrawAPIs() const override;

    static MHWRender::MPxDrawOverride* creator(const MObject& obj);

    virtual bool hasUIDrawables() const override;
    virtual void addUIDrawables(const MDagPath& objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* data) override;

    virtual MUserData* prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* oldData);

private:
    FaceIndexOverlayDrawOverride(const MObject& obj);
};
