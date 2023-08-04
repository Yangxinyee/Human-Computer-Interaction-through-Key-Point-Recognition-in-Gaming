#pragma once
#include "GestureWrapper.h"
#include <vector>

using namespace std;

typedef vector<GestureWrapper*> GLIST;

class GestureDetection
{
public:
    GestureDetection(void);
    ~GestureDetection(void);

    //************************************
    // Desc:      添加需要检测的手势类型
    // Method:    AddGesture
    // FullName:  GestureDetection::AddGesture
    // Access:    public 
    // Returns:   void
    // Qualifier:
    // Parameter: GestureType type 手势类型
    //************************************
    void AddGesture(GestureType type);

    //************************************
    // Desc:      更新所有的手势
    // Method:    UpdateAllGesture
    // FullName:  GestureDetection::UpdateAllGesture
    // Access:    public 
    // Returns:   void
    // Qualifier:
    // Parameter: Joint * pJoints 骨骼点
    //************************************
    void UpdateAllGesture(Joint* pJoints);


    //************************************
    // Desc:      重置
    // Method:    RestAll
    // FullName:  GestureDetection::RestAll
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void RestAll();

    // 识别结果回调函数
    GestureRecognized fGestureRecongnized;

private:
    // 手势列表
    GLIST* pGestureList; 
};

