#pragma once
#include "KinectDevice.h"
class KinectBody
{
public:
    KinectBody(void);
    ~KinectBody(void);
    
    // 运行子窗口
    int Run(HINSTANCE hInst, HWND hWnd);

    // 窗口消息分发
    static LRESULT CALLBACK MessageRouter(HWND, UINT, WPARAM, LPARAM);

    // 窗口消息处理
    LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

private:
    // 窗口句柄
    HANDLE mHwnd;

    // 配置
    KinectConfig mKinectConfig;
    // Kinect 设备
    KinectDevice* pKinectDevice;
    // 更新数据帧
    void Update();
};
void track_mouse();

