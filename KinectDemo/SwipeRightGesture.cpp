#include "stdafx.h"
#include "SwipeRightGesture.h"
#include <iostream>

using namespace std;

SwipeRightGesture::SwipeRightGesture(void)
{
    mStepCount = 3;
}


SwipeRightGesture::~SwipeRightGesture(void)
{
}

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

GestureParseResult SwipeRightGesture::Step1(Joint* pJoints)
{
    Joint spineShoulder = pJoints[JointType_SpineShoulder];
    Joint head = pJoints[JointType_Head];


    return GestureParseResult::Succeed;
}

GestureParseResult SwipeRightGesture::Step2(Joint* pJoints)
{
    Joint spineShoulder = pJoints[JointType_SpineShoulder];
    Joint head = pJoints[JointType_Head];


    if (abs(spineShoulder.Position.X - head.Position.X) >= 0.01 && spineShoulder.Position.X <= head.Position.X)
        return GestureParseResult::Succeed;
    return GestureParseResult::Fail;
}

GestureParseResult SwipeRightGesture::Step3(Joint* pJoints)
{
    Joint spineShoulder = pJoints[JointType_SpineShoulder];
    Joint head = pJoints[JointType_Head];


    if (abs(spineShoulder.Position.X - head.Position.X) >= 0.01 && spineShoulder.Position.X <= head.Position.X)
        return GestureParseResult::Succeed;
    return GestureParseResult::Fail;
}
