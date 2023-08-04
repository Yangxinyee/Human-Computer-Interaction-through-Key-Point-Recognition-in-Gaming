#pragma once
#include "ImageRenderer.h"
#include "BodyRenderer.h"
#include <Kinect.h>
#include <d2d1.h>
#pragma comment (lib, "d2d1.lib")

#define COLOR_FRAME_WIDTH       1920
#define COLOR_FRAME_HEIGHT      1080
#define DEPTH_FRAME_WIDTH       512
#define DEPTH_FRAME_HEIGHT      424
#define BODY_COUNT              6

typedef struct _DepthData
{
    INT64 nDepthTime;
    int nDepthWidth;
    int nDepthHeight;
    USHORT nDepthMinReliableDistance;
    USHORT nDepthMaxDistance;
    UINT nDepthBufferSize;
    UINT16 *pDepthBuffer;
}DepthData;

typedef struct _ColorData
{
    int nColorWidth;
    int nColorHeight;
    UINT nColorBufferSize;
    RGBQUAD *pColorBuffer;
}ColorData;

typedef struct _BodyData
{
    INT64 nBodyTime;
    int nBodyCount;
    IBody** ppBodies;
}BodyData;

typedef struct _KinectConfig
{
    bool bAcquireColor;
    bool bAcquireDepth;
    bool bAcquireBody;
    HWND hWndForColor;
    HWND hWndForDepth;
    HWND hWndForBody;

    void (*BodyFrameCallback)(IBody** ppBodies, int nBodyCount);
}KinectConfig;

class KinectDevice
{
public:
    KinectDevice(KinectConfig*);
    ~KinectDevice(void);

    HRESULT Open();
    void Close();

    void Update();

    bool MapDepthPointToCameraSpace(DepthSpacePoint point, int depth, CameraSpacePoint* pCameraSpacePoint);
    bool MapColorPointToDepthPoint(int xInColorImg, int yInColorImg, DepthSpacePoint &depthPoint);
    bool MapDepthPointToColorPoint(int xInDepthImg, int yInDepthImg, ColorSpacePoint* colorSpacePoint);
    bool GetDepth(int xInDepthImg, int yInDepthImg, int* depth);

    ImageRenderer* GetColorImageRenderer();
    ImageRenderer* GetDepthIamgeRenderer();
private:
    bool bShowColorFrame;
    bool bShowDepthFrame;
    bool bShowBodyFrame;

    KinectConfig* pKinectConfig;

    IKinectSensor* pKinectSensor;
    ICoordinateMapper* pCoordinateMapper;
    IMultiSourceFrameReader* pMultiSourceFrameReader;
    DepthSpacePoint* pDepthSpacePoint;

    ImageRenderer* pDrawColorBasics;
    ImageRenderer* pDrawDepthBasics;
    BodyRenderer* pDrawBody;
    ID2D1Factory* pD2DFactory;
    RGBQUAD* pColorRGBX;
    RGBQUAD* pDepthRGBX;

    // 深度图像上每个点对应的z值(即距离红外摄像头的距离)
    UINT16* pDepthValue;

    void ProcessFrame(DepthData* pDepthData, ColorData* pColorData);

    HRESULT UpdateColorFrame(IColorFrame* pColorFrame, ColorData* pColorData);
    HRESULT UpdateDepthFrame(IDepthFrame* pDepthFrame, DepthData* pDepthData);
    

    void ProcessColor(RGBQUAD* pBuffer, int nWidth, int nHeight);
    void ProcessDepth(UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth);
    void ProcessBody(BodyData* pBodyData);
};

