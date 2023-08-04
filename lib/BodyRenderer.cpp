#include "stdafx.h"
#include "BodyRenderer.h"


BodyRenderer::BodyRenderer(void) :
    mHwnd(NULL),
    pD2DFactory(NULL),
    pRenderTarget(NULL),
    pBrushJointTracked(NULL),
    pBrushJointInferred(NULL),
    pBrushBoneTracked(NULL),
    pBrushBoneInferred(NULL),
    pBrushHandClosed(NULL),
    pBrushHandOpen(NULL),
    pBrushHandLasso(NULL)
{
}


BodyRenderer::~BodyRenderer(void)
{
    DiscardResources();
    SafeRelease(pD2DFactory);
}

HRESULT BodyRenderer::Initialize(HWND hwnd, ID2D1Factory* pD2DFactory, ICoordinateMapper* pCoordinateMapper)
{
    if (NULL == pD2DFactory)
    {
        return E_INVALIDARG;
    }

    mHwnd = hwnd;
    this->pD2DFactory = pD2DFactory;
    this->pCoordinateMapper = pCoordinateMapper;
    this->pD2DFactory->AddRef();

    return S_OK;
}

HRESULT BodyRenderer::EnsureResources()
{
    HRESULT hr = S_OK;

    if (pD2DFactory && !pRenderTarget)
    {
        RECT rc;
        GetWindowRect(mHwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        D2D1_SIZE_U size = D2D1::SizeU(width, height);
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

        hr = pD2DFactory->CreateHwndRenderTarget(
            rtProps,
            D2D1::HwndRenderTargetProperties(mHwnd, size),
            &pRenderTarget
            );

        if (FAILED(hr))
        {
            return hr;
        }

        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.27f, 0.75f, 0.27f), &pBrushJointTracked);

        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &pBrushJointInferred);
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &pBrushBoneTracked);
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 1.0f), &pBrushBoneInferred);

        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 0.5f), &pBrushHandClosed);
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 0.5f), &pBrushHandOpen);
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 0.5f), &pBrushHandLasso);
    }

    return hr;
}

void BodyRenderer::DiscardResources()
{
    SafeRelease(pRenderTarget);
    SafeRelease(pBrushJointTracked);
    SafeRelease(pBrushJointInferred);
    SafeRelease(pBrushBoneTracked);
    SafeRelease(pBrushBoneInferred);
    SafeRelease(pBrushHandClosed);
    SafeRelease(pBrushHandOpen);
    SafeRelease(pBrushHandLasso);
}

HRESULT BodyRenderer::Draw(IBody** ppBodies, int bodyCount)
{
    if (!mHwnd)
        return E_FAIL;

    HRESULT hr = EnsureResources();
    if (SUCCEEDED(hr) && pRenderTarget && pCoordinateMapper)
    {
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear();

        RECT rc;
        GetClientRect(mHwnd, &rc);
        int width = rc.right;
        int height = rc.bottom;

        for (int i = 0; i < bodyCount; i ++)
        {
            IBody* pBody = ppBodies[i];
            if (pBody)
            {
                BOOLEAN bTracked = false;
                hr = pBody->get_IsTracked(&bTracked);
                if (SUCCEEDED(hr) && bTracked)
                {
                    Joint joints[JointType_Count];
                    D2D1_POINT_2F jointPoints[JointType_Count];
                    HandState leftHandState = HandState_Unknown;
                    HandState rightHandState = HandState_Unknown;

                    pBody->get_HandLeftState(&leftHandState);
                    pBody->get_HandRightState(&rightHandState);

                    hr = pBody->GetJoints(_countof(joints), joints);
                    if (SUCCEEDED(hr))
                    {
                        for (int j = 0; j < _countof(joints); j++)
                        {
                            jointPoints[j] = BodyToScreen(joints[j].Position, width, height);
                        }
                        DrawBody(joints, jointPoints);

                        DrawHand(leftHandState, jointPoints[JointType_HandLeft]);
                        DrawHand(rightHandState, jointPoints[JointType_HandRight]);
                    }
                }
            }
        }

        hr = pRenderTarget->EndDraw();

        if (D2DERR_RECREATE_TARGET == hr)
        {
            hr = S_OK;
            DiscardResources();
        }
    }

    return hr;
}

D2D1_POINT_2F BodyRenderer::BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height)
{
    // 计算body在屏幕中的位置

    DepthSpacePoint depthPoint = {0};
    pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);
    float screenPointX = static_cast<float> (depthPoint.X * width) / cDepthWidth;
    float screenPointY = static_cast<float> (depthPoint.Y * height) / cDepthHeight;

    return D2D1::Point2F(screenPointX, screenPointY);
}

void BodyRenderer::DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1)
{
    TrackingState joint0State = pJoints[joint0].TrackingState;
    TrackingState joint1State = pJoints[joint1].TrackingState;

    if ((joint0State == TrackingState_NotTracked) ||
        (joint1State == TrackingState_NotTracked))
    {
        return;
    }

    if (joint0State == TrackingState_Inferred && 
        joint1State == TrackingState_Inferred)
    {
        return;
    }

    if (joint0State == TrackingState_Tracked &&
        joint1State == TrackingState_Tracked)
    {
        pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], pBrushBoneTracked, TRACKED_BONE_THICKNESS);
    }
    else 
    {
        pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], pBrushBoneInferred, INFERRED_BONE_THICKNESS);
    }
}

void BodyRenderer::DrawHand(HandState handState, const D2D1_POINT_2F& handPosition)
{
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(handPosition, HAND_SIZE, HAND_SIZE);
    switch (handState)
    {
    case HandState_Closed:
        pRenderTarget->FillEllipse(ellipse, pBrushHandClosed);
        break;
    case HandState_Open:
        pRenderTarget->FillEllipse(ellipse, pBrushHandOpen);
        break;
    case HandState_Lasso:
        pRenderTarget->FillEllipse(ellipse, pBrushHandLasso);
        break;
    default:
        break;
    }
}

void BodyRenderer::DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints)
{
    // Torso
    DrawBone(pJoints, pJointPoints, JointType_Head, JointType_Neck);
    DrawBone(pJoints, pJointPoints, JointType_Neck, JointType_SpineShoulder);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_SpineMid);
    DrawBone(pJoints, pJointPoints, JointType_SpineMid, JointType_SpineBase);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderRight);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderLeft);
    DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipRight);
    DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipLeft);

    // Right Arm    
    DrawBone(pJoints, pJointPoints, JointType_ShoulderRight, JointType_ElbowRight);
    DrawBone(pJoints, pJointPoints, JointType_ElbowRight, JointType_WristRight);
    DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_HandRight);
    DrawBone(pJoints, pJointPoints, JointType_HandRight, JointType_HandTipRight);
    DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_ThumbRight);
    
    // Left Arm
    DrawBone(pJoints, pJointPoints, JointType_ShoulderLeft, JointType_ElbowLeft);
    DrawBone(pJoints, pJointPoints, JointType_ElbowLeft, JointType_WristLeft);
    DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_HandLeft);
    DrawBone(pJoints, pJointPoints, JointType_HandLeft, JointType_HandTipLeft);
    DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_ThumbLeft);

    // Right Leg
    DrawBone(pJoints, pJointPoints, JointType_HipRight, JointType_KneeRight);
    DrawBone(pJoints, pJointPoints, JointType_KneeRight, JointType_AnkleRight);
    DrawBone(pJoints, pJointPoints, JointType_AnkleRight, JointType_FootRight);

    // Left Leg
    DrawBone(pJoints, pJointPoints, JointType_HipLeft, JointType_KneeLeft);
    DrawBone(pJoints, pJointPoints, JointType_KneeLeft, JointType_AnkleLeft);
    DrawBone(pJoints, pJointPoints, JointType_AnkleLeft, JointType_FootLeft);

    // Draw the joints
    for (int i = 0; i < JointType_Count; i ++)
    {
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(pJointPoints[i], JOINT_THICKNESS, JOINT_THICKNESS);

        if (pJoints[i].TrackingState == TrackingState_Inferred)
        {
            pRenderTarget->FillEllipse(ellipse, pBrushJointInferred);
        }
        else if (pJoints[i].TrackingState == TrackingState_Tracked)
        {
            pRenderTarget->FillEllipse(ellipse, pBrushJointTracked);
        }
    }
}
