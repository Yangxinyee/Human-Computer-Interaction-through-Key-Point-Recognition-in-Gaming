// KinectDemo.cpp : 定义应用程序的入口点。


#include "stdafx.h"
#include "KinectDemo.h"
#include "KinectBody.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

int filter = 10;            //均值滤波邻域帧数
int countfilter = 1;        //均值滤波相邻帧计数
double sumx = 0;			// 过去帧X信息累加和
double sumy = 0;			// 过去帧Y信息累加和
double x = 0;
double y = 0;


HWND btnHwnd;                                    // 保存按钮句柄
DepthSpacePoint		front = { 0,0 };			//用来记录上一次鼠标的位置
DepthSpacePoint		depthUpLeft = { 1,1 };		//操作窗口的左上角和右下角，要注意这两个X和X、Y和Y的差会作为除数，所以不能都为0
DepthSpacePoint		depthDownRight = { 0,0 };
DepthSpacePoint		depthhandRight = { 0,0 };   // 右手的深度图信息
CameraSpacePoint	cameraUpLeft;
CameraSpacePoint	cameraDownRight;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//是一个 Windows 平台上用于定义应用程序入口点（entry point）的函数。

//这个函数是用于创建 Windows 程序的主函数，它会在应用程序启动时被调用。参数的含义如下：
//
//hInstance：表示当前应用程序的实例句柄（handle），是一个唯一标识当前正在运行的应用程序实例的值。
//hPrevInstance：在早期的 Windows 版本中用于表示前一个实例的句柄，但在现代 Windows 中已不再使用，通常会被忽略（设置为 NULL）。
//lpCmdLine：包含命令行参数的字符串，是以空格分隔的参数列表。
//nCmdShow：指定应用程序窗口的显示方式，默认情况下控制应用程序窗口的显示方式。
//APIENTRY 是一个宏定义，用于指定函数的调用约定。在 Windows 平台上，APIENTRY 通常会被定义为 __stdcall，这意味着函数遵循 stdcall 调用约定。
//
//这个函数的返回类型是 int，表示应用程序的退出码（exit code），它会在应用程序结束时被返回给操作系统。返回值通常用于指示应用程序的执行结果或状态。


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_KINECTDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KINECTDEMO));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KINECTDEMO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_KINECTDEMO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

//    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, 250, 250, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
        case 10001: // 按钮ID
            {
                // TODO: 跳转到子窗口
                KinectBody* pBody = new KinectBody();
                pBody->Run(hInst, hWnd);
                delete pBody;
            }
            break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
    case WM_CREATE:
        {
            // 获取主窗体的位置和大小
            RECT region;
            GetClientRect(hWnd, &region);
            // 窗体宽高
            int with = region.right - region.left;
            int height = region.bottom - region.top;
            // 按钮宽高
            int btnWidth = with - 20;
            int btnHeight = 30;
            // 按钮在窗体中的位置
            int x = (with - btnWidth) / 2;
            int y = 40;

            HWND button = CreateWindow(
                _TEXT("Button"),// 类型
                _TEXT("GAME 体感控制器"),// 名称
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // 风格：可视，子窗口，按钮
                x, y, btnWidth, btnHeight, // 位置和大小
                hWnd, // 父窗口
                (HMENU)10001, // 按钮的ID
                hInst,
                NULL
                );
            SendMessage(button, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0); // 设置字体
        }
        break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
