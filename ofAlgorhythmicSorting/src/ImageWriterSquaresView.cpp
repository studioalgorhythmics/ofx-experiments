/*
 *  ImageWriterSquaresView.cpp
 *  AlgorhythmicSorting
 *
 *  Created by Administrator on 1/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ImageWriterSquaresView.h"

void ImageWriterSquaresView::SetColor(int color)
{
    mColor = color;
}

void ImageWriterSquaresView::SetFill(bool fill)
{
    mFill = fill;
}

void ImageWriterSquaresView::DrawRect(int x, int y, int w, int h)
{
    if (!mImage) return;
    
    if (mFill) {
        //get the buffer.
        unsigned char* pixels = mImage->getPixels();
        int pixelIndex;
        int imageHeight = mImage->getHeight();
        int imageWidth  = mImage->getWidth();
        int imageEnd    = 4 * imageWidth * imageHeight;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                pixelIndex = 4 * ((x + j) + (y + i) * imageWidth);
                if (pixelIndex >= 0 && pixelIndex < imageEnd) {
                    pixels[pixelIndex++] = (mColor & 0xFF0000) >> 16;
                    pixels[pixelIndex++] = (mColor & 0x00FF00) >> 8;
                    pixels[pixelIndex++] = mColor & 0xFF;
                    pixels[pixelIndex++] = 0xFF;
                }
            }
        }
    }
}

