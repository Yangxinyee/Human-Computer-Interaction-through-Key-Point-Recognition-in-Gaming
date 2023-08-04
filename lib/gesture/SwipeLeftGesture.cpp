#include "stdafx.h"
#include "SwipeLeftGesture.h"


SwipeLeftGesture::SwipeLeftGesture(void)
{
    mStepCount = 3;
}


SwipeLeftGesture::~SwipeLeftGesture(void)
{
}

GestureParseResult SwipeLeftGesture::CheckGesture(Joint* pJoints, int nStepIndex)
{
    switch (nStepIndex)
    {
    case 0:
        return Step1(pJoints);
    case 1:
        return Step2(pJoints);
    case 2:
        return Step3(pJoints);
    default:
        break;
    }
    return GestureParseResult::Fail;
}

GestureParseResult SwipeLeftGesture::Step1(Joint* pJoints)
{
    Joint handRight = pJoints[JointType_HandRight];
    Joint handLeft = pJoints[JointType_HandLeft];
    Joint head = pJoints[JointType_Head];
    Joint elbowRight = pJoints[JointType_ElbowRight];
    Joint spinBase = pJoints[JointType_SpineBase];
    Joint spinShoulder = pJoints[JointType_SpineShoulder];
    Joint shoulderRight = pJoints[JointType_ShoulderRight];

    // 右手在手肘之前，且左手低于肩膀
    if (handRight.Position.Z < elbowRight.Position.Z 
        && handLeft.Position.Y < spinShoulder.Position.Y)
    {
        // 右手低于头部，且高于臀部
        if (//handRight.Position.Y < head.Position.Y &&
            handRight.Position.Y > spinBase.Position.Y)
        {
            if (handRight.Position.X > shoulderRight.Position.X)
            {
                return GestureParseResult::Succeed;
            }

            return GestureParseResult::Pausing;
        }
    }

    return GestureParseResult::Fail;
}

GestureParseResult SwipeLeftGesture::Step2(Joint* pJoints)
{
    Joint handRight = pJoints[JointType_HandRight];
    Joint handLeft = pJoints[JointType_HandLeft];
    Joint head = pJoints[JointType_Head];
    Joint elbowRight = pJoints[JointType_ElbowRight];
    Joint spinBase = pJoints[JointType_SpineBase];
    Joint spinShoulder = pJoints[JointType_SpineShoulder];
    Joint shoulderRight = pJoints[JointType_ShoulderRight];
    Joint shoulderLeft = pJoints[JointType_ShoulderLeft];

    // 右手在手肘之前，且左手低于肩膀
    if (handRight.Position.Z < elbowRight.Position.Z 
        && handLeft.Position.Y < spinShoulder.Position.Y)
    {
        // 右手低于头部，且高于臀部
        if (//handRight.Position.Y < head.Position.Y &&
            handRight.Position.Y > spinBase.Position.Y)
        {
            // 右手在右肩膀左侧，且在左肩膀右侧
            if (handRight.Position.X < shoulderRight.Position.X  && 
                //handRight.Position.X > shoulderLeft.Position.X)
                    handRight.Position.X > spinShoulder.Position.X)
            {
                return GestureParseResult::Succeed;
            }

            return GestureParseResult::Pausing;
        }
    }

    return GestureParseResult::Fail;
}

GestureParseResult SwipeLeftGesture::Step3(Joint* pJoints)
{
    Joint handRight = pJoints[JointType_HandRight];
    Joint handLeft = pJoints[JointType_HandLeft];
    Joint head = pJoints[JointType_Head];
    Joint elbowRight = pJoints[JointType_ElbowRight];
    Joint spinBase = pJoints[JointType_SpineBase];
    Joint spinShoulder = pJoints[JointType_SpineShoulder];
    Joint shoulderRight = pJoints[JointType_ShoulderRight];
    Joint shoulderLeft = pJoints[JointType_ShoulderLeft];

    // 右手在手肘之前，且左手低于肩膀
    if (handRight.Position.Z < elbowRight.Position.Z 
        && handLeft.Position.Y < spinShoulder.Position.Y)
    {
        // 右手低于头部，且高于臀部
        if (//handRight.Position.Y < head.Position.Y &&
            handRight.Position.Y > spinBase.Position.Y)
        {
            // 右手在左肩膀左侧
            //if (handRight.Position.X < shoulderLeft.Position.X)
            if (handRight.Position.X < spinShoulder.Position.X)
            {
                return GestureParseResult::Succeed;
            }

            return GestureParseResult::Pausing;
        }
    }

    return GestureParseResult::Fail;
}
