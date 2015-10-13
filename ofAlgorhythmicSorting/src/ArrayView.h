#ifndef __ARRAY_VIEW_H__
#define __ARRAY_VIEW_H__

#include <vector>

#define kHighNumberBrightness 0xFF
class ArrayView
{
public:
    ArrayView(int x, int y, int width, int height);
    virtual ~ArrayView(){}
    virtual void Draw(std::vector<double*> &arrays, std::vector<int*> &markers) = 0;
    //if the number of rows displayed is more than one override this.
    virtual int CalcNumRowsNeeded() {return 1;}
    void SetRect(int x, int y, int width, int height);
protected:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
};
#endif