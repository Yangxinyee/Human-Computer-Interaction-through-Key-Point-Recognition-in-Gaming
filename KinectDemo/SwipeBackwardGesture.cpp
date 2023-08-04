#include "stdafx.h"
#include "SwipeBackwardGesture.h"
#include <iostream>

using namespace std;

SwipeBackwardGesture::SwipeBackwardGesture(void)
{
    mStepCount = 3;
}


SwipeBackwardGesture::~SwipeBackwardGesture(void)
{

}

GestureParseResult SwipeBackwardGesture::CheckGesture(Joint* pJoints, int nStepIndex)
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

GestureParseResult SwipeBackwardGesture::Step1(Joint* pJoints)
{
    return GestureParseResult::Succeed;
}

GestureParseResult SwipeBackwardGesture::Step2(Joint* pJoints)
{
    Joint spineShoulder = pJoints[JointType_SpineShoulder];
    Joint head = pJoints[JointType_Head];


    if (abs(spineShoulder.Position.Z - head.Position.Z) >= 0.01 && spineShoulder.Position.Z < head.Position.Z)
        return GestureParseResult::Succeed;
    return GestureParseResult::Fail;
}

GestureParseResult SwipeBackwardGesture::Step3(Joint* pJoints)
{
    Joint spineShoulder = pJoints[JointType_SpineShoulder];
    Joint head = pJoints[JointType_Head];


    if (abs(spineShoulder.Position.Z - head.Position.Z) >= 0.01 && spineShoulder.Position.Z < head.Position.Z)
        return GestureParseResult::Succeed;
    return GestureParseResult::Fail;
}
