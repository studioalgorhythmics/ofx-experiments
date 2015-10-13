#ifndef __SQUARES_VIEW_H__
#define __SQUARES_VIEW_H__

#include "ArrayView.h"
class SquaresView : public ArrayView
{
public:
    SquaresView(int x, int y, int width, int height):ArrayView(x,y,width,height){};
    virtual ~SquaresView(){}
    
    virtual void Draw(std::vector<double*> &arrays, std::vector<int*> &markers);
    //if the number of rows displayed is more than one override this.
    virtual int CalcNumRowsNeeded();
    
    virtual int GetSquareHeight();
    
    
protected:
    virtual void SetColor(int color);
    virtual void SetFill(bool fill);
    virtual void DrawRect(int x, int y, int w, int h);
};

#endif