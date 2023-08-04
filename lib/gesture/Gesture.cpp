#include "stdafx.h"
#include "Gesture.h"


Gesture::Gesture(void)
{
    mStepCount = 0;
}

Gesture::~Gesture()
{

}

int Gesture::GetStepCount()
{
    return mStepCount;
}
