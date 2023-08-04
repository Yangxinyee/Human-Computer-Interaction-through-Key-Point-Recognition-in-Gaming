#pragma once
#include <d2d1.h>
#include <Kinect.h>

#define TRACKED_BONE_THICKNESS          6.0F
#define INFERRED_BONE_THICKNESS         1.0F
#define HAND_SIZE                       30.0F
#define JOINT_THICKNESS                 3.0F

class BodyRenderer
{
    static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;
public:
    BodyRenderer(void);
    ~BodyRenderer(void);

    HRESULT Initialize(HWND hwnd, ID2D1Factory* pD2DFactory, ICoordinateMapper* pCoordinateMapper);
    HRESULT Draw(IBody** ppBodies, int bodyCount);

private:
    HWND mHwnd;
    ICoordinateMapper* pCoordinateMapper;

    ID2D1Factory* pD2DFactory;
    ID2D1HwndRenderTarget* pRenderTarget;
    ID2D1SolidColorBrush* pBrushJointTracked;
    ID2D1SolidColorBrush* pBrushJointInferred;
    ID2D1SolidColorBrush* pBrushBoneTracked;
    ID2D1SolidColorBrush* pBrushBoneInferred;
    ID2D1SolidColorBrush* pBrushHandClosed;
    ID2D1SolidColorBrush* pBrushHandOpen;
    ID2D1SolidColorBrush* pBrushHandLasso;

    HRESULT EnsureResources();
    void DiscardResources();

    D2D1_POINT_2F BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height);
    void DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1);
    void DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);
    void DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints);
};

