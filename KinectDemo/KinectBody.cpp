#include "stdafx.h"
#include "resource.h"
#include "KinectBody.h"
#include "BodyRenderer.h"
#include "GestureDetection.h"
#include <Strsafe.h>
#include <windows.h>
#include <time.h>
#include<winuser.h>
#include<iostream>
#include <cstdio>
#include <math.h>


using namespace std;
double OK_MOUSE = 0.0001; //鼠标开始移动的阈值
extern double rec_mouse_x=0, rec_mouse_y=0;


//extern DepthSpacePoint  front;				//用来记录上一次鼠标的位置
extern DepthSpacePoint  depthUpLeft;			//操作窗口的左上角和右下角，要注意这两个X和X、Y和Y的差会作为除数，所以不能都为0
extern DepthSpacePoint  depthDownRight;
extern DepthSpacePoint  depthhandRight;
extern CameraSpacePoint	cameraUpLeft;
extern CameraSpacePoint	cameraDownRight;


extern int filter;
extern int countfilter;
extern double sumx;
extern double sumy;
extern double x;
extern double y;

/**********************************************************/
//点击鼠标
/**********************************************************/
void click()
{

    POINT P;
    GetCursorPos(&P);
    int x1 = P.x;
    int y1 = P.y;
    mouse_event(MOUSEEVENTF_LEFTDOWN, x1, y1, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, x1, y1, 0, 0);

}

/***********************************************************/
HINSTANCE hGlobalInst;
HWND hGlobalHwnd;
GestureDetection* pGestureDetection = NULL;
void WINAPI OnGestureRecognized(GestureType type);
// 骨骼帧数据回调
void HandleBodyFrame(IBody** ppBodies, int bodyCount);

// 自定义的一个延时函数delay()
void delay(int seconds) //  参数必须为整型，表示延时多少秒
{
    clock_t start = clock();
    clock_t lay = (clock_t)seconds * CLOCKS_PER_SEC;
    while ((clock() - start) < lay);
}

void delay(double seconds) //  参数为双精度浮点型。这个函数是我修改的上面那个函数，重载一下。
{
    double start = clock();
    double lay = (double)seconds * CLOCKS_PER_SEC;
    while ((clock() - start) < lay);
}

// 键盘控制
void down(int vk)
{
    keybd_event(vk, 0, 0, 0);
    delay(0.1);
}
void up(int vk)
{
    keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
}
void press(int vk)
{
    down(vk);
    up(vk);
}
KinectBody::KinectBody(void)
{
}


KinectBody::~KinectBody(void)
{
	if (pKinectDevice != NULL) 
	{
		pKinectDevice->Close();
		delete pKinectDevice;
	}
}

int KinectBody::Run(HINSTANCE hInst, HWND parent) 
{
    EnableWindow(parent, false);
    MSG msg = {0};
    hGlobalInst = hInst;// 保存对应用的全局引用

    // 初始化窗体类
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(hInst, MAKEINTRESOURCE(IDI_KINECTDEMO));
    wc.lpfnWndProc   = DefDlgProc;
    wc.lpszClassName = _TEXT("KinectBodyCls");

    if (!RegisterClass(&wc)) 
    {
        EnableWindow(parent, TRUE);
        SetForegroundWindow(parent);
        return 0;
    }

    // 创建窗体
    HWND hWnd = CreateDialogParam(
        NULL,
        MAKEINTRESOURCE(IDD_BODY),
        NULL,
        (DLGPROC) KinectBody::MessageRouter,
        reinterpret_cast<LPARAM> (this)
        );

    // 屏幕大小
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    RECT region;
    GetWindowRect(hWnd, &region);
    int w = region.right - region.left;
    int h = region.bottom - region.top;
    int x = (screenWidth - w) / 2;
    int y = (screenHeight - h) / 2;

    MoveWindow(hWnd, x, y, w, h, FALSE);
    ShowWindow(hWnd,SW_SHOW);

    while (WM_QUIT != msg.message)
    {
        // 更新数据
        Update();
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (hWnd && IsDialogMessage(hWnd, &msg)) 
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnregisterClass(_TEXT("KinectBodyCls"), hInst);
    EnableWindow(parent, TRUE);
    SetForegroundWindow(parent);

    return 1;
}

LRESULT CALLBACK KinectBody::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    KinectBody* pThis = NULL;
    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<KinectBody*> (lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (pThis));
    }
    else 
    {
        pThis = reinterpret_cast<KinectBody*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK KinectBody::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    int wmId, wmEvent;
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        {
            mHwnd = hWnd;
            hGlobalHwnd = hWnd;
            // 配置
            mKinectConfig.bAcquireColor = false;
            mKinectConfig.bAcquireDepth = false;
            mKinectConfig.bAcquireBody = true;
            mKinectConfig.hWndForColor = NULL;
            mKinectConfig.hWndForDepth = NULL;
            mKinectConfig.hWndForBody = GetDlgItem(hWnd, IDC_BODY);
            mKinectConfig.BodyFrameCallback = HandleBodyFrame;

            // 初始化手势识别
            pGestureDetection = new GestureDetection();
            pGestureDetection->fGestureRecongnized = OnGestureRecognized;
            pGestureDetection->AddGesture(GestureType::SwipeRight);//添加识别手势类型
            pGestureDetection->AddGesture(GestureType::SwipeLeft);
            pGestureDetection->AddGesture(GestureType::SwipeForward);//添加识别手势类型
            pGestureDetection->AddGesture(GestureType::SwipeBackward);

            pKinectDevice = new KinectDevice(&mKinectConfig);
            HRESULT hr = pKinectDevice->Open();
            if (FAILED(hr))
            {
                MessageBox(hWnd, L"初始化失败。", L"提示", MB_OK);
            }
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }

    return FALSE;
}

void KinectBody::Update()
{
    if (!pKinectDevice)
        return;
    pKinectDevice->Update();
}

void HandleBodyFrame(IBody** ppBodies, int bodyCount)
{
    // TODO:处理骨骼帧数据

    IBody* pBody = NULL;
    float distance = -1;
    // 寻找离镜头最近的人体
    for (int i = 0; i < bodyCount; i ++)
    {
        IBody* tmpBody = ppBodies[i];
        if (tmpBody)
        {
            BOOLEAN bTracked = FALSE;
            tmpBody->get_IsTracked(&bTracked);
            if (bTracked)
            {
                Joint joints[JointType_Count];
                tmpBody->GetJoints(_countof(joints), joints);
                Joint head = joints[JointType_Head];
                float tmpDis = head.Position.Z;
                if (distance >= 0)
                {
                    if (tmpDis < distance)
                    {
                        distance = tmpDis;
                        pBody = tmpBody;
                    }
                }
                else
                {
                    distance = tmpDis;
                    pBody = tmpBody;
                }
            }
        }
    }

    if (pBody == NULL)
        return;
    WCHAR result[60];
    HandState righHandState = HandState_Unknown;
    HRESULT hr = pBody->get_HandRightState(&righHandState);// 获取右手状态
    if (SUCCEEDED(hr)) 
    {
        StringCchPrintf(result, _countof(result), L"状态：%s\t", (righHandState == HandState_Open? L"张开" : L"握拳")); // 只判断2种

        /**********************************************************/
        //                      鼠标跟踪
        /**********************************************************/
        Joint joints[JointType_Count];
        pBody->GetJoints(_countof(joints), joints);
        Joint handRight = joints[JointType_HandRight];

        // 离散梯度下降滤波算法--原创
        sumx += handRight.Position.X - x;
        sumy += handRight.Position.Y - y;
        x = x + sumx / filter;
        y = y + sumy / filter;
        sumx = 0;
        sumy = 0;
        

        Joint neck = joints[JointType_Neck];
        Joint head = joints[JointType_Head];
        double	unite = sqrt(pow(neck.Position.X - head.Position.X, 2) + pow(neck.Position.Y - head.Position.Y, 2) + pow(neck.Position.Z - head.Position.Z, 2));
        CameraSpacePoint	cameraUpLeft = { neck.Position.X + unite * 3, neck.Position.Y + unite * 1.2, neck.Position.Z };
        CameraSpacePoint	cameraDownRight = { neck.Position.X + unite * 6.5, neck.Position.Y - unite * 0.8 , neck.Position.Z };
        /*CameraSpacePoint cameraUpLeft = { 0.1, 0.08, 1.3 };
        CameraSpacePoint cameraDownRight = { 1, -1, 1.3 };
        double width = 0.4;
        double height = 0.38;*/
        double width = fabs(cameraUpLeft.X - cameraDownRight.X);
        double height = fabs(cameraUpLeft.Y - cameraDownRight.Y);
        double mouseX = fabs(x - cameraUpLeft.X);
        double mouseY = fabs(y - cameraUpLeft.Y);
        bool flg = true;
        if (fabs(x - rec_mouse_x) >= OK_MOUSE || fabs(y - rec_mouse_y) >= OK_MOUSE) // 如果鼠标移动大于阈值OK_MOUSE,认为鼠标移动了
        {
            // 碰到边缘就停止，不反弹
            if (x <= cameraUpLeft.X) x = cameraUpLeft.X;
            if (x >= cameraDownRight.X) x = cameraDownRight.X;
            if (y >= cameraUpLeft.Y) y = cameraUpLeft.Y;
            if (y <= cameraDownRight.Y) y = cameraDownRight.Y;
        }
        else
            flg = false;

        double mouseX = fabs(x - cameraUpLeft.X);
        double mouseY = fabs(y - cameraUpLeft.Y);
        if (flg)
        {   
            //计算公式：小窗口的点/小窗口尺寸 = 大窗口的点/大窗口尺寸
            mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 65535 * mouseX / width, 65535 * mouseY / height, 0, 0);
            rec_mouse_x = x, rec_mouse_y = y;
        }

        SetDlgItemText(hGlobalHwnd, IDC_RIGHT_HAND, result);
    }
    HandState leftHandState = HandState_Unknown;
    hr = pBody->get_HandLeftState(&leftHandState); // 获取左手状态
    if (SUCCEEDED(hr)) 
    {
        StringCchPrintf(result, _countof(result), L"状态：%s\t", (leftHandState == HandState_Open? L"cease fire！" : L"fire！"));
        /**********************************************************/
        //                     鼠标左键点击
        /**********************************************************/
        /*if (leftHandState != HandState_Open)
            click();*/

        SetDlgItemText(hGlobalHwnd, IDC_LEFT_HAND, result);
    }

    // 识别手势
    if (pGestureDetection)
    {
        Joint joints[JointType_Count];
        pBody->GetJoints(_countof(joints), joints);
        pGestureDetection->UpdateAllGesture(joints);
    }
}

/**********************************************************/
//                      按键执行
/**********************************************************/
void WINAPI OnGestureRecognized(GestureType type)
{
    switch (type)
    {
    /*case GestureType::SwipeRight:
        SetDlgItemText(hGlobalHwnd, IDC_GESTURE, L"向右摆头");
       
        press(68);
        break;
    case GestureType::SwipeLeft:
        SetDlgItemText(hGlobalHwnd, IDC_GESTURE, L"向左摆头");
        press(65);
        break;
    case GestureType::SwipeForward:
        SetDlgItemText(hGlobalHwnd, IDC_GESTURE, L"向前摆头");
        press(87);
        break;
    case GestureType::SwipeBackward:
        SetDlgItemText(hGlobalHwnd, IDC_GESTURE, L"向后摆头");
        press(83);
        break;*/
    default:
        break;
    }
}