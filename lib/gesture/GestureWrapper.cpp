#include "stdafx.h"
#include "GestureWrapper.h"
#include "DebugConsole.h"

GestureWrapper::GestureWrapper(GestureType gestureType) :
    mGestureType(gestureType),
    pGesture(NULL),
    mStepIndex(0),
    mPuasedFrameCount(10),
    mFrameCount(0),
    bPaused(false)
{
    switch (gestureType)
    {
    case GestureType::SwipeRight:
        pGesture = new SwipeRightGesture();
        break;
    case GestureType::SwipeLeft:
        pGesture = new SwipeLeftGesture();
        break;
    default:
        break;
    }
}


GestureWrapper::~GestureWrapper(void)
{
    if (pGesture)
    {
        delete pGesture;
        pGesture = NULL;
    }
}

void GestureWrapper::UpdateGesture(Joint* pJoints)
{
    if (bPaused)
    {
        if (mFrameCount == mPuasedFrameCount)
        {
            bPaused = false;
        }
        mFrameCount ++;
    }

    GestureParseResult rs = pGesture->CheckGesture(pJoints, mStepIndex);

    if (rs == GestureParseResult::Succeed)
    {
        if (mStepIndex + 1 < pGesture->GetStepCount())
        {
            mStepIndex ++;
            mFrameCount = 0;
            mPuasedFrameCount = 10;
            bPaused = false;
        }
        else 
        {
            // 成功识别
            if (fGestureRecognized)
            {
                fGestureRecognized(mGestureType);
            }
        }
    }
    else if (rs == GestureParseResult::Fail || mFrameCount == 50)
    {
        mStepIndex = 0;
        mFrameCount = 0;
        mPuasedFrameCount = 5;
        bPaused = true;
    }
    else 
    {
        mFrameCount ++;
        mPuasedFrameCount = 5;
        bPaused = true;
    }
}

void GestureWrapper::Reset()
{
    mStepIndex = 0;
    mFrameCount = 0;
    mPuasedFrameCount = 5;
    bPaused = true;
}

