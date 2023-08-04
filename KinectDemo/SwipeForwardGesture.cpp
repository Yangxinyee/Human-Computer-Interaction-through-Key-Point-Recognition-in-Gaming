#include "stdafx.h"
#include "SwipeForwardGesture.h"
#include <iostream>

using namespace std;

SwipeForwardGesture::SwipeForwardGesture(void)
{
    mStepCount = 3;
}


SwipeForwardGesture::~SwipeForwardGesture(void)
{
}

GestureParseResult SwipeForwardGesture::CheckGesture(Joint* pJoints, int nStepIndex)
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

GestureParseResult SwipeForwardGesture::Step1(Joint* pJoints)
{
    return GestureParseResult::Succeed;
}

GestureParseResult SwipeForwardGesture::Step2(Joint* pJoints)
{
    Joint spineShoulder = pJoints[JointType_SpineShoulder];
    Joint head = pJoints[JointType_Head];


    if (abs(spineShoulder.Position.Z - head.Position.Z) >= 0.01 && spineShoulder.Position.Z > head.Position.Z)
        return GestureParseResult::Succeed;
    return GestureParseResult::Fail;
}

GestureParseResult SwipeForwardGesture::Step3(Joint* pJoints)
{
    Joint spineShoulder = pJoints[JointType_SpineShoulder];
    Joint head = pJoints[JointType_Head];


    if (abs(spineShoulder.Position.Z - head.Position.Z) >= 0.01 && spineShoulder.Position.Z > head.Position.Z)
        return GestureParseResult::Succeed;
    return GestureParseResult::Fail;
}
