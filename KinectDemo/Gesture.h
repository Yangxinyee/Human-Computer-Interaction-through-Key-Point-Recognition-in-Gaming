#pragma once
#include <Kinect.h>

// 手势解析结果
enum _GestureParseResult
{
    // 失败
    Fail,

    // 成功
    Succeed,

    // 待定
    Pausing
};

// 手势解析结果
typedef _GestureParseResult GestureParseResult;

class Gesture
{
public:
    Gesture(void);
    ~Gesture(void);
    //************************************
    // Desc:      手势检测，不同的手势将分不同的步骤进行检测。
    // Method:    CheckGesture
    // FullName:  Gesture::CheckGesture
    // Access:    virtual public 
    // Returns:   GestureParseResult 检测结果
    // Qualifier:
    // Parameter: Joint * pJoints 骨骼点
    // Parameter: int nStepIndex 检测步骤
    //************************************
    virtual GestureParseResult CheckGesture(Joint* pJoints, int nStepIndex) = 0;
    int GetStepCount();
protected:
    // 检测步骤数
    int mStepCount;
};

