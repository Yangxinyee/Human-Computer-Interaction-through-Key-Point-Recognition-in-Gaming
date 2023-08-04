#include "stdafx.h"
#include "ImageRenderer.h"


ImageRenderer::ImageRenderer(void) :
    m_hWnd(0),
    m_sourceWidth(0),
    m_sourceHeight(0),
    m_sourceStride(0),
    m_pD2DFactory(NULL), 
    m_pRenderTarget(NULL),
    m_pBitmap(0),
    m_pBrush(NULL)
{
}


ImageRenderer::~ImageRenderer(void)
{
    DiscardResources();
    SafeRelease(m_pD2DFactory);
}

// 确保相关的D2D资源已创建
HRESULT ImageRenderer::EnsureResources()
{
    HRESULT hr = S_OK;

    if (NULL == m_pRenderTarget)
    {
        D2D1_SIZE_U size = D2D1::SizeU(m_sourceWidth, m_sourceHeight);

        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

        // Create a hWnd render target, in order to render to the window set in initialize
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            rtProps,
            D2D1::HwndRenderTargetProperties(m_hWnd, size),
            &m_pRenderTarget
            );

        if (FAILED(hr))
        {
            return hr;
        }

        // Create a bitmap that we can copy image data into and then render to the target
        hr = m_pRenderTarget->CreateBitmap(
            size, 
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),
            &m_pBitmap 
            );

        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0, 0), &m_pBrush);
            
            m_ellipse.point.x = -1;
            m_ellipse.point.y = -1;
            m_ellipse.radiusX = 20;
            m_ellipse.radiusY = 20;
        }

        if (FAILED(hr))
        {
            SafeRelease(m_pRenderTarget);
            return hr;
        }
    }

    return hr;
}

// 释放D2D的资源
void ImageRenderer::DiscardResources()
{
    SafeRelease(m_pRenderTarget);
    SafeRelease(m_pBitmap);
    SafeRelease(m_pBrush);
}

// 初始化图片渲染器。将kinect获取的视频流显示到相应的控件上。
HRESULT ImageRenderer::Initialize(HWND hWnd, ID2D1Factory* pD2DFactory, int sourceWidth, int sourceHeight, int sourceStride)
{
    if (NULL == pD2DFactory)
    {
        return E_INVALIDARG;
    }

    m_hWnd = hWnd;

    // One factory for the entire application so save a pointer here
    m_pD2DFactory = pD2DFactory;

    m_pD2DFactory->AddRef();

    // Get the frame size
    m_sourceWidth  = sourceWidth;
    m_sourceHeight = sourceHeight;
    m_sourceStride = sourceStride;

    return S_OK;
}

void ImageRenderer::DrawCircle(int x, int y, int radius /* = 5 */)
{
    m_Position.point.x = x;
    m_Position.point.y = y;
    m_Position.radiusX = radius;
    m_Position.radiusY = radius;
}

// 绘制图片
    // pImage 是RGBX格式的图片数据，cbImage为图片数据的字节大小
HRESULT ImageRenderer::Draw(BYTE* pImage, unsigned long cbImage)
{
    // 检验数据大小是否正确
    if (cbImage < ((m_sourceHeight - 1) * m_sourceStride) + (m_sourceWidth * 4))
    {
        return E_INVALIDARG;
    }
    HRESULT hr = EnsureResources();

    if (FAILED(hr))
    {
        return hr;
    }

    // 复制图片数据
    hr = m_pBitmap->CopyFromMemory(NULL, pImage, m_sourceStride);

    if (FAILED(hr))
    {
        return hr;
    }

    m_pRenderTarget->BeginDraw();

    // 绘制图片
    m_pRenderTarget->DrawBitmap(m_pBitmap);

    // 运动位置
    if (m_pBrush && m_ellipse.point.x > 0 && m_ellipse.point.y > 0)
    {
        m_pRenderTarget->DrawEllipse(m_ellipse, m_pBrush, 3.5f);
    }

    if (m_pBrush && m_Position.point.x >=0 && m_Position.point.y >= 0)
    {
        //m_pRenderTarget->DrawEllipse(m_Position, m_pBrush, 3.0f);
        m_pRenderTarget->FillEllipse(m_Position, m_pBrush);
    }

    hr = m_pRenderTarget->EndDraw();

    // 目标丢失时，重新建创建
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardResources();
    }

    return hr;
}
