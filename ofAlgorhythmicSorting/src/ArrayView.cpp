#include "ArrayView.h"


ArrayView::ArrayView(int x, int y, int width, int height)
{
    SetRect(x, y, width, height);
}

void ArrayView::SetRect(int x, int y, int width, int height)
{
    m_width = width;
    m_height = height;
    m_x = x;
    m_y = y;
}