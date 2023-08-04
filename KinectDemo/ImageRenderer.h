#pragma once
#include <d2d1.h>

class ImageRenderer
{
public:
    ImageRenderer(void);
    ~ImageRenderer(void);

    // 初始化图片渲染器。将kinect获取的视频流显示到相应的控件上。
    HRESULT Initialize(HWND hwnd, ID2D1Factory* pD2DFactory, int sourceWidth, int sourceHeight, int sourceStride);

    // 绘制图片
    // pImage 是RGBX格式的图片数据，cbImage为图片数据的字节大小
    HRESULT Draw(BYTE* pImage, unsigned long cbImage);

    void DrawCircle(int x, int y, int radius = 10);
private:
    // 图片绘制的控件
    HWND m_hWnd;
    // 图片的高
    UINT m_sourceHeight;
    // 图片的宽
    UINT m_sourceWidth;
    // 图片高或宽的跨度。一维数组表示一个矩阵时，改值表示矩阵的列数。
    UINT m_sourceStride;

    // D2D工厂
    ID2D1Factory* m_pD2DFactory;
    // D2D渲染目标
    ID2D1HwndRenderTarget* m_pRenderTarget;
    ID2D1Bitmap* m_pBitmap;

    D2D1_ELLIPSE m_ellipse;
    ID2D1SolidColorBrush* m_pBrush;

    // 确保相关的D2D资源已创建
    HRESULT EnsureResources();

    D2D1_ELLIPSE m_Position;

    // 释放D2D的资源
    void DiscardResources();
};

