#include "stdafx.h"
#include "SwipeRightGesture.h"


SwipeRightGesture::SwipeRightGesture(void) : Gesture()
{
    mStepCount = 3;
}


SwipeRightGesture::~SwipeRightGesture(void)
{
}

//************************************
    // Desc:      检测手势。向右摆手手势，分三步进。
    // Method:    CheckGesture
    // FullName:  SwipeRightGesture::CheckGesture
    // Access:    public 
    // Returns:   GestureParseResult
    // Qualifier:
    // Parameter: Joint * pJoints 骨骼点
    // Parameter: int nStepIndex 检测步骤
    //************************************
GestureParseResult SwipeRightGesture::CheckGesture(Joint* pJoints, int nStepIndex)
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

//************************************
// Desc:      检测步骤1。左手在左肩膀左侧。
// Method:    Step1
// FullName:  SwipeRightGesture::Step1
// Access:    private 
// Returns:   GestureParseResult 检测结果
// Qualifier:
// Parameter: Joint * pJoints 骨骼点
//************************************
GestureParseResult SwipeRightGesture::Step1(Joint* pJoints)
{
    Joint handLeft = pJoints[JointType_HandLeft];
    Joint handRight = pJoints[JointType_HandRight];
    Joint elbowLeft = pJoints[JointType_ElbowLeft];
    Joint shoulderLeft = pJoints[JointType_ShoulderLeft];
    Joint head = pJoints[JointType_Head];
    Joint spineBase = pJoints[JointType_SpineBase];

    // 左手在肘部之前，右手垂下
    if (handLeft.Position.Z < elbowLeft.Position.Z &&
        handRight.Position.Y < spineBase.Position.Y)
    {
        // 左手低于头部，但高于臀部
        if (//handLeft.Position.Y  < head.Position.Y &&
            handLeft.Position.Y > spineBase.Position.Y)
        {
            // 左手位于肩膀左侧
            if (handLeft.Position.X < shoulderLeft.Position.X)
            {
                return GestureParseResult::Succeed;
            }

            return GestureParseResult::Pausing;
        }

    }

    return GestureParseResult::Fail;
}

GestureParseResult SwipeRightGesture::Step2(Joint* pJoints)
{
    Joint handLeft = pJoints[JointType_HandLeft];
    Joint handRight = pJoints[JointType_HandRight];
    Joint elbowLeft = pJoints[JointType_ElbowLeft];
    Joint shoulderLeft = pJoints[JointType_ShoulderLeft];
    Joint shoulderRight = pJoints[JointType_ShoulderRight];
    Joint head = pJoints[JointType_Head];
    Joint spineBase = pJoints[JointType_SpineBase];
    Joint spineShoulder = pJoints[JointType_SpineShoulder];

    // 左手在肘部之前，右手垂下
    if (handLeft.Position.Z < elbowLeft.Position.Z &&
        handRight.Position.Y < spineBase.Position.Y)
    {
        // 左手低于头部，但高于臀部
        if (//handLeft.Position.Y < head.Position.Y &&
            handLeft.Position.Y > spineBase.Position.Y)
        {
            // 左手位于左肩膀右侧，但位于右肩膀左侧
            if (handLeft.Position.X < shoulderRight.Position.X &&
                //handLeft.Position.X > shoulderLeft.Position.X)
                    handLeft.Position.X > spineShoulder.Position.X)
            {
                return GestureParseResult::Succeed;
            }

            return GestureParseResult::Pausing;
        }
    }

    return GestureParseResult::Fail;
}

//************************************
// Desc:      检测步骤3.左手位于右肩膀右侧。
// Method:    Step3
// FullName:  SwipeRightGesture::Step3
// Access:    private 
// Returns:   GestureParseResult 检测结果
// Qualifier:
// Parameter: Joint * pJoints 骨骼点
//************************************
GestureParseResult SwipeRightGesture::Step3(Joint* pJoints)
{
    Joint handLeft = pJoints[JointType_HandLeft];
    Joint handRight = pJoints[JointType_HandRight];
    Joint elbowLeft = pJoints[JointType_ElbowLeft];
    Joint shoulderRight = pJoints[JointType_ShoulderRight];
    Joint head = pJoints[JointType_Head];
    Joint spineBase = pJoints[JointType_SpineBase];
    Joint spineShoulder = pJoints[JointType_SpineShoulder];

    // 左手在肘部之前，右手垂下
    if (handLeft.Position.Z < elbowLeft.Position.Z &&
        handRight.Position.Y < spineBase.Position.Y)
    {
        // 左手低于头部，但高于臀部
        if (//handLeft.Position.Y < head.Position.Y &&
            handLeft.Position.Y > spineBase.Position.Y)
        {
            // 左手位于肩膀右侧
            //if (handLeft.Position.X > shoulderRight.Position.X)
            if (handLeft.Position.X > spineShoulder.Position.X )
            {
                return GestureParseResult::Succeed;
            }

            return GestureParseResult::Pausing;
        }
    }

    return GestureParseResult::Fail;
}
