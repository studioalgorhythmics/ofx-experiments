#ifndef __BARS_VIEW_H__
#define __BARS_VIEW_H__

#include "ArrayView.h"
class BarsView : public ArrayView
{
public:
    BarsView(int x, int y, int width, int height):ArrayView(x,y,width,height){};
    virtual ~BarsView(){}
    
    virtual void Draw(std::vector<double*> &arrays, std::vector<int*> &markers);
};

#endif