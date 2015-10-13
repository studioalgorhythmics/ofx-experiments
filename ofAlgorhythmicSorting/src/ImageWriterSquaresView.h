/*
 *  ImageWriterSquaresView.h
 *  AlgorhythmicSorting
 *
 *  Created by Administrator on 1/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __IMAGE_WRITER_SQUARES_VIEW_H__
#define __IMAGE_WRITER_SQUARES_VIEW_H__

#include "ofMain.h"
#include "SquaresView.h"

class ImageWriterSquaresView : public SquaresView
{
public:
    ImageWriterSquaresView(int x, int y, int width, int height):SquaresView(x,y,width,height){};
    virtual ~ImageWriterSquaresView(){}
    
    void SetImage(ofImage *image) {mImage = image;}
protected:
    void SetColor(int color);
    void SetFill(bool fill);
    void DrawRect(int x, int y, int w, int h);
    
    ofImage *mImage;
    int      mColor;
    bool     mFill;
};

#endif