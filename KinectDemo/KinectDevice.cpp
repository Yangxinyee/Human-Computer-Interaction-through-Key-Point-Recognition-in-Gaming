#include "stdafx.h"
#include "KinectDevice.h"
#include <limits>

using namespace std;

KinectDevice::KinectDevice(KinectConfig* pConfig) :
    pKinectSensor(NULL),
    pCoordinateMapper(NULL),
    pMultiSourceFrameReader(NULL),
    pDepthSpacePoint(NULL),
    pD2DFactory(NULL),
    pDrawColorBasics(NULL),
    pDrawDepthBasics(NULL),
    pDrawBody(NULL),
    pColorRGBX(NULL),
    pDepthRGBX(NULL),
    pDepthValue(NULL),
    pKinectConfig(pConfig)
{
    
}


KinectDevice::~KinectDevice(void)
{
    // 避免忘记调用Close的情况
    Close();
}

HRESULT KinectDevice::Open()
{
    if (!pKinectConfig->bAcquireColor && !pKinectConfig->bAcquireDepth && !pKinectConfig->bAcquireBody)
        return E_FAIL;

    bShowColorFrame = pKinectConfig->bAcquireColor && pKinectConfig->bAcquireColor;
    bShowDepthFrame = pKinectConfig->bAcquireDepth && pKinectConfig->hWndForDepth;
    bShowBodyFrame = pKinectConfig->bAcquireBody && pKinectConfig->hWndForBody;

    if (pKinectConfig->bAcquireColor) 
    {
        pColorRGBX = new RGBQUAD[COLOR_FRAME_WIDTH * COLOR_FRAME_HEIGHT];
    }

    if (pKinectConfig->bAcquireDepth)
    {
        pDepthRGBX = new RGBQUAD[DEPTH_FRAME_WIDTH * DEPTH_FRAME_HEIGHT];
        pDepthValue = new UINT16[DEPTH_FRAME_WIDTH * DEPTH_FRAME_HEIGHT];
    }

    if (bShowColorFrame || bShowDepthFrame || bShowBodyFrame) 
    {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
        if (bShowColorFrame)
        {
            pDrawColorBasics = new ImageRenderer();
            pDrawColorBasics->Initialize(pKinectConfig->hWndForColor, pD2DFactory, COLOR_FRAME_WIDTH, COLOR_FRAME_HEIGHT, COLOR_FRAME_WIDTH * sizeof(RGBQUAD));
        }

        if (bShowDepthFrame)
        {
            pDrawDepthBasics = new ImageRenderer();
            pDrawDepthBasics->Initialize(pKinectConfig->hWndForDepth, pD2DFactory, DEPTH_FRAME_WIDTH, DEPTH_FRAME_HEIGHT, DEPTH_FRAME_WIDTH * sizeof(RGBQUAD));
        }
    }



   HRESULT hr = GetDefaultKinectSensor(&pKinectSensor);
   if (FAILED(hr))
   {
       return hr;
   }

   if (pKinectSensor)
   {
       hr = pKinectSensor->get_CoordinateMapper(&pCoordinateMapper);

       if (SUCCEEDED(hr))
       {
           pDepthSpacePoint = new DepthSpacePoint[COLOR_FRAME_WIDTH * COLOR_FRAME_HEIGHT];
           if (bShowBodyFrame)
           {
               pDrawBody = new BodyRenderer();
               pDrawBody->Initialize(pKinectConfig->hWndForBody, pD2DFactory, pCoordinateMapper);
           }
       }

       hr = pKinectSensor->Open();
       if (SUCCEEDED(hr))
       {
           DWORD type = 0;
           if (pKinectConfig->bAcquireColor)
               type |= FrameSourceTypes_Color;
           if (pKinectConfig->bAcquireDepth)
               type |= FrameSourceTypes_Depth;
           if (pKinectConfig->bAcquireBody)
               type |= FrameSourceTypes_Body;
               

           hr = pKinectSensor->OpenMultiSourceFrameReader(type, &pMultiSourceFrameReader);
       }
   }

   if (!pKinectSensor || FAILED(hr))
   {
       return E_FAIL;
   }

   return hr;
}

void KinectDevice::Close()
{
    if (pDrawColorBasics)
    {
        delete pDrawColorBasics;
        pDrawColorBasics = NULL;
    }

    if (pDrawDepthBasics)
    {
        delete pDrawDepthBasics;
        pDrawDepthBasics = NULL;
    }

    if (pDrawBody)
    {
        delete pDrawBody;
        pDrawBody = NULL;
    }

    if (pColorRGBX)
    {
        delete[] pColorRGBX;
        pColorRGBX = NULL;
    }

    if (pDepthRGBX)
    {
        delete[] pDepthRGBX;
        pDepthRGBX = NULL;
    }

    if (pDepthSpacePoint)
    {
        delete pDepthSpacePoint;
        pDepthSpacePoint = NULL;
    }

    if (pDepthValue)
    {
        delete[] pDepthValue;
        pDepthValue = NULL;
    }

    SafeRelease(pD2DFactory);
    SafeRelease(pMultiSourceFrameReader);
    SafeRelease(pCoordinateMapper);

    if (pKinectSensor)
    {
        pKinectSensor->Close();
    }

    SafeRelease(pKinectSensor);
}

void KinectDevice::Update()
{
    if (!pMultiSourceFrameReader)
        return;

    IMultiSourceFrame* pMultiSourceFrame = NULL;
    
    IDepthFrame* pDepthFrame = NULL;
    IColorFrame* pColorFrame = NULL;
    IBodyFrame* pBodyFrame = NULL;
    DepthData depthData = {0};
    ColorData colorData = {0};
    BodyData bodyData = {0};
    
    HRESULT hr = pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
    if (SUCCEEDED(hr))
    {
        if (pKinectConfig->bAcquireDepth)
        {
            IDepthFrameReference* pDepthFrameRef = NULL;
            hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameRef);
            if (SUCCEEDED(hr))
            {
                hr = pDepthFrameRef->AcquireFrame(&pDepthFrame);
            }

            if (pDepthFrame && SUCCEEDED(hr))
            {
                hr = UpdateDepthFrame(pDepthFrame,&depthData);
            }
            SafeRelease(pDepthFrameRef);
        }

        if (pKinectConfig->bAcquireColor)
        {
            IColorFrameReference* pColorFrameRef = NULL;
            hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameRef);
            if (SUCCEEDED(hr))
            {
                hr = pColorFrameRef->AcquireFrame(&pColorFrame);
            }
            if (pColorFrame && SUCCEEDED(hr))
            {
                hr = UpdateColorFrame(pColorFrame, &colorData);
            }
            SafeRelease(pColorFrameRef);
        }

        if (pKinectConfig->bAcquireBody)
        {
            IBodyFrameReference* pBodyFrameRef = NULL;
            hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameRef);
            if (SUCCEEDED(hr))
            {
                hr = pBodyFrameRef->AcquireFrame(&pBodyFrame);
            }
            if (pBodyFrame && SUCCEEDED(hr))
            {
                IBody* ppBodies[BODY_COUNT] = {0};

                pBodyFrame->get_RelativeTime(&bodyData.nBodyTime);

                hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);

                if (SUCCEEDED(hr))
                {
                    bodyData.nBodyCount = BODY_COUNT;
                    bodyData.ppBodies = ppBodies;
                    ProcessBody(&bodyData);
                    if (pKinectConfig->BodyFrameCallback)
                    {
                        pKinectConfig->BodyFrameCallback(ppBodies, BODY_COUNT);
                    }
                }

                for (int i = 0; i < _countof(ppBodies); i ++)
                {
                    SafeRelease(ppBodies[i]);
                }
            }
            SafeRelease(pBodyFrameRef);
        }
    }

    if (SUCCEEDED(hr))
    {
        ProcessFrame(&depthData, &colorData);
    }

    SafeRelease(pDepthFrame);
    SafeRelease(pColorFrame);
    SafeRelease(pBodyFrame);
    SafeRelease(pMultiSourceFrame);
}

bool KinectDevice::MapDepthPointToCameraSpace(DepthSpacePoint point, int depth, CameraSpacePoint* pCameraSpacePoint)
{
    if (!pCoordinateMapper || !pDepthSpacePoint || !(pKinectConfig->bAcquireColor && pKinectConfig->bAcquireDepth))
        return false;

    HRESULT hr = pCoordinateMapper->MapDepthPointToCameraSpace(point, depth, pCameraSpacePoint);
    if (SUCCEEDED(hr))
    {
        return true;
    }
   
    return false;
}

bool KinectDevice::MapColorPointToDepthPoint(int xInColorImg, int yInColorImg, DepthSpacePoint &depthPoint)
{
    if (!pKinectConfig->bAcquireDepth || !pCoordinateMapper || !pDepthSpacePoint)
        return false;

    int index = xInColorImg + yInColorImg * COLOR_FRAME_WIDTH;
    DepthSpacePoint dsPoint = pDepthSpacePoint[index];
    if (dsPoint.X == -std::numeric_limits<float>::infinity() || dsPoint.Y == -std::numeric_limits<float>::infinity())
    {
        // 无效映射
        return false;
    }

    depthPoint.X = dsPoint.X;
    depthPoint.Y = dsPoint.Y;
    return true;
}

bool KinectDevice::MapDepthPointToColorPoint(int xInDepthImg, int yInDepthImg, ColorSpacePoint* colorSpacePoint) 
{
    if (!pKinectConfig->bAcquireDepth || !pCoordinateMapper || !pDepthSpacePoint)
        return false;

    DepthSpacePoint ds = {0};
    ds.X = xInDepthImg;
    ds.Y = yInDepthImg;
    int depth = *(pDepthValue + xInDepthImg + yInDepthImg * DEPTH_FRAME_WIDTH);
    HRESULT hr = pCoordinateMapper->MapDepthPointToColorSpace(ds, depth, colorSpacePoint);
    if (FAILED(hr))
    {
        return false;
    }
    return true;
}

bool KinectDevice::GetDepth(int xInDepthImg, int yInDepthImg, int* depth)
{
    if (!pKinectConfig->bAcquireDepth)
        return false;

    (*depth) = pDepthValue[xInDepthImg + yInDepthImg * DEPTH_FRAME_WIDTH];

    return true;
}

void KinectDevice::ProcessFrame(DepthData* pDepthData, ColorData* pColorData)
{
    if (pCoordinateMapper && pDepthSpacePoint && pDepthData && pColorData &&
        pDepthData->pDepthBuffer && (pDepthData->nDepthWidth == DEPTH_FRAME_WIDTH && pDepthData->nDepthHeight == DEPTH_FRAME_HEIGHT) &&
        pColorData->pColorBuffer && (pColorData->nColorWidth == COLOR_FRAME_WIDTH && pColorData->nColorHeight == COLOR_FRAME_HEIGHT))
    {
        // 将彩色图像点对应到深度图像上的点。由于彩色图像与深度图像的分辨率不一样，因此不能一一映射。
        HRESULT hr = pCoordinateMapper->MapColorFrameToDepthSpace(pDepthData->nDepthWidth * pDepthData->nDepthHeight, (UINT16*)pDepthData->pDepthBuffer,
            pColorData->nColorWidth * pColorData->nColorHeight, pDepthSpacePoint);
        //  计算深度图像上的z值
        memcpy_s(pDepthValue, pDepthData->nDepthBufferSize, pDepthData->pDepthBuffer, pDepthData->nDepthBufferSize);
    }
}

HRESULT KinectDevice::UpdateDepthFrame(IDepthFrame* pDepthFrame, DepthData* pDepthData)
{
    IFrameDescription* pFrameDesc = NULL;
    HRESULT hr = pDepthFrame->get_RelativeTime(&pDepthData->nDepthTime);
    if (SUCCEEDED(hr))
    {
        hr = pDepthFrame->get_FrameDescription(&pFrameDesc);
    }
    if (SUCCEEDED(hr))
    {
        hr = pFrameDesc->get_Width(&pDepthData->nDepthWidth);
    }
    if (SUCCEEDED(hr))
    {
        hr = pFrameDesc->get_Height(&pDepthData->nDepthHeight);
    }
    if (SUCCEEDED(hr))
    {
        hr = pDepthFrame->get_DepthMinReliableDistance(&pDepthData->nDepthMinReliableDistance);
    }
    if (SUCCEEDED(hr))
    {
        pDepthData->nDepthMaxDistance = USHRT_MAX;
    }
    if (SUCCEEDED(hr))
    {
        hr = pDepthFrame->AccessUnderlyingBuffer(&pDepthData->nDepthBufferSize, &pDepthData->pDepthBuffer);
    }
    if (SUCCEEDED(hr))
    {
        ProcessDepth(pDepthData->pDepthBuffer, pDepthData->nDepthWidth, pDepthData->nDepthHeight, pDepthData->nDepthMinReliableDistance, pDepthData->nDepthMaxDistance);
    }
    SafeRelease(pFrameDesc);
    return hr;
}

HRESULT KinectDevice::UpdateColorFrame(IColorFrame* pColorFrame, ColorData* pColorData)
{
    IFrameDescription* pFrameDescription = NULL;
    ColorImageFormat imageFormat = ColorImageFormat_None;

    HRESULT hr = pColorFrame->get_FrameDescription(&pFrameDescription);
    if (SUCCEEDED(hr))
    {
        hr = pFrameDescription->get_Width(&pColorData->nColorWidth);
    }
    if (SUCCEEDED(hr))
    {
        hr = pFrameDescription->get_Height(&pColorData->nColorHeight);
    }
    if (SUCCEEDED(hr))
    {
        hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
    }
    if (SUCCEEDED(hr))
    {
        if (imageFormat == ColorImageFormat_Bgra)
        {
            hr = pColorFrame->AccessRawUnderlyingBuffer(&pColorData->nColorBufferSize, reinterpret_cast<BYTE**> (&pColorData->pColorBuffer));
        }
        else if (pColorRGBX)
        {
            pColorData->pColorBuffer = pColorRGBX;
            pColorData->nColorBufferSize = COLOR_FRAME_WIDTH * COLOR_FRAME_HEIGHT * sizeof(RGBQUAD);
            hr = pColorFrame->CopyConvertedFrameDataToArray(pColorData->nColorBufferSize, reinterpret_cast<BYTE*> (pColorData->pColorBuffer), ColorImageFormat_Bgra);
        }
        else 
        {
            hr = E_FAIL;
        }
    }

    if (SUCCEEDED(hr))
    {
        ProcessColor(pColorData->pColorBuffer, pColorData->nColorWidth, pColorData->nColorHeight);
    }

    SafeRelease(pFrameDescription);

    return hr;
}

void KinectDevice::ProcessBody(BodyData* pBodyData)
{
    if (pDrawBody)
    {
        pDrawBody->Draw(pBodyData->ppBodies, pBodyData->nBodyCount);
    }
}

void KinectDevice::ProcessColor(RGBQUAD* pBuffer, int nWidth, int nHeight)
{
    if (bShowColorFrame && pBuffer && 
        (nWidth == COLOR_FRAME_WIDTH) && (nHeight == COLOR_FRAME_HEIGHT))
    {
        pDrawColorBasics->Draw(reinterpret_cast<BYTE*> (pBuffer), nWidth * nHeight * sizeof(RGBQUAD));
    }
}

void KinectDevice::ProcessDepth(UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth)
{
    if (bShowDepthFrame && pDepthRGBX && pBuffer && (nWidth == DEPTH_FRAME_WIDTH) && (nHeight == DEPTH_FRAME_HEIGHT))
    {
        RGBQUAD* pRGBX = pDepthRGBX;
        const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

        while (pBuffer < pBufferEnd)
        {
            USHORT depth = *pBuffer;

            BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);

            pRGBX->rgbRed   = intensity;
            pRGBX->rgbGreen = intensity;
            pRGBX->rgbBlue  = intensity;

            ++pRGBX;
            ++pBuffer;
        }

        pDrawDepthBasics->Draw(reinterpret_cast<BYTE*> (pDepthRGBX), DEPTH_FRAME_WIDTH * DEPTH_FRAME_HEIGHT * sizeof(RGBQUAD));
    }
}

ImageRenderer* KinectDevice::GetColorImageRenderer() 
{
    return pDrawColorBasics;
}

ImageRenderer* KinectDevice::GetDepthIamgeRenderer()
{
    return pDrawDepthBasics;
}

