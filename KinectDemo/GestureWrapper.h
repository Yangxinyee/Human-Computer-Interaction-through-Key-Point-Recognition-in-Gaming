#pragma once
#include "Gesture.h"
#include "SwipeRightGesture.h"
#include "SwipeLeftGesture.h"
#include "SwipeForwardGesture.h"
#include "SwipeBackwardGesture.h"

enum _GestureType
{
    None,

    // 向右摆
    SwipeRight,

    // 向左摆
    SwipeLeft,
    
    // 向前摆
    SwipeForward,

    // 向后摆
    SwipeBackward,
};

// 手势类型
typedef _GestureType GestureType;

// 手势识别结果回调。
typedef void(CALLBACK *GestureRecognized)(GestureType type);

// 封装手势识别
class GestureWrapper
{
public:
    GestureWrapper(GestureType gestureType);
    ~GestureWrapper(void);


    //************************************
    // Desc:      更新手势
    // Method:    UpdateGesture
    // FullName:  GestureWrapper::UpdateGesture
    // Access:    public 
    // Returns:   void
    // Qualifier:
    // Parameter: Joint * pJoints 骨骼点
    //************************************
    void UpdateGesture(Joint* pJoints);

    
    //************************************
    // Desc:      重置
    // Method:    Reset
    // FullName:  GestureWrapper::Reset
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void Reset();

    // 手势识别回调处理函数
    GestureRecognized fGestureRecognized;
private:
    // 手势类型
    GestureType mGestureType;

    // 手势识别
    Gesture* pGesture;

    // 步骤索引号
    int mStepIndex;

    // 检测结果为待定时，停留的帧数。
    int mPuasedFrameCount;

    // 帧数
    int mFrameCount;

    // 是否处于待定状态
    bool bPaused;
};

