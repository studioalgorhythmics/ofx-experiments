/*
 *  ConsoleView.cpp
 *  AlgorhythmicSorting
 *
 *  Created by mchinen on 12/13/10.
 *  Copyright 2010 Institute for Algorhythmics. All rights reserved.
 *
 */

#include "ConsoleView.h"
#include "SortingMan.h"
#include "ofMain.h"
#include "AlgorhythmicAudioIO.h"
#define kDrawMethodBorder 10

ConsoleView::ConsoleView(int x, int y, int width, int height)
:ArrayView(x,y,width,height)
{
    m_arrayFont     = new ofTrueTypeFont;
    m_currFontSize  = -1;
    m_lastArraySize = -1;
    m_rowsNeeded    = 0;
}

#define kNormalTextColor 0xFFFFFF
void ConsoleView::Draw(std::vector<double*> &arrays, std::vector<int*> &markers)
{
    //TODO: there is a bug where old messages get redrawn out of place, becuase
    //we are doing the timestamps by pointer values, and an old deleted array
    //in the history gets reallocated to the same spot in memory.
    //fix this by adding timestamps to the array/marker system.
    //I've hacked it for now by adding kMinHistory to fetchHistory, and it fixes
    //it in the most cases.
    
    //start drawing from the bottom up.
    int consoleCursor = m_consoleOutput.size() - 1;
    int startRow = arrays.size() - 1;
    int len = SortingMan::Instance()->GetArraySize();
    int currentIndex = AlgorhythmicAudioIO::Instance()->CurrentIndex();
    int xCursor;
    char fill[5];
    fill[4] = 0;
    fill[0] = fill[1] = fill[2] = 'p';
    fill[3] = '1';
    int rows = m_rowsNeeded;
    for (int i = startRow; rows > 0 && i >= 0; i--) {
        //before we draw the array, see if a console message comes after it.
        while (consoleCursor >= 0 &&
               m_consoleOutput[consoleCursor].ownerArrayPtr == arrays[i]) {
            //draw the message
            ofSetHexColor(kNormalTextColor);
            m_arrayFont->drawString(m_consoleOutput[consoleCursor].message,
                                    m_x + kDrawMethodBorder,
                                    m_y + rows * m_arrayFont->getLineHeight() + kDrawMethodBorder);
            rows--;
            consoleCursor--;
        }
        
        //draw the array using the same cmy(k) scheme in the other views.
        xCursor = 0;
        for(int j = 0; j < len; j++) {
            double val;
            int color, bgcolor;
            char numStr[8];
            int dx, dy;
            
            val   = arrays[i][j];
            sprintf(numStr, "%3.0f ", val);
            //sprintf might not null terminate.
            numStr[4] = 0;
            color = kNormalTextColor;
            bgcolor = 0xffffff;
            if(markers[i][j] & kMarkerChanged)
                color = 0xff22ff; //take out some green and blue components
            else if(markers[i][j] & kMarkerCompared)
                color = 0x22ffff; //take out some green and red components
            else if(markers[i][j] & kMarkerFocused)
                color = 0xffff22; //take out some red and blue components
            
            ofRectangle bounds = m_arrayFont->getStringBoundingBox(fill,
                                                                   m_x + kDrawMethodBorder + xCursor,
                                                                   m_y + rows * m_arrayFont->getLineHeight() + kDrawMethodBorder);
            dx = bounds.width;
            dy = bounds.height;
            //the bounds rect is funny, so omit drawing it for now (by not setting bgcolor)
            if (bgcolor != 0xffffff) {
                ofSetColor(0);
                ofRect(bounds.x,
                       bounds.y,
                       dx, dy);
            }
            
            //if it is playing make the index brighter but not white to keep the color
            if (currentIndex == j && i == startRow)
                color |= 0xaaaaaa;
            ofSetColor(color);
            m_arrayFont->drawString(numStr,
                                    m_x + kDrawMethodBorder + xCursor,
                                    m_y + rows * m_arrayFont->getLineHeight() + kDrawMethodBorder);
            //stringWidth doesn't handle spaces, so substitute a char filled buffer
            
            xCursor += dx;
        }
        rows--;
    }
    
    //draw whatever console messages can fit above
    while (consoleCursor >= 0 && rows > 0) {
        ofSetColor(kNormalTextColor);
        m_arrayFont->drawString(m_consoleOutput[consoleCursor].message,
                                m_x + kDrawMethodBorder,
                                m_y + rows * m_arrayFont->getLineHeight() + kDrawMethodBorder);
        rows--;
        consoleCursor--;
    }
}


int ConsoleView::CalcNumRowsNeeded()
{
    int len = SortingMan::Instance()->GetArraySize();
    int viewWidth  = m_width  - kDrawMethodBorder * 2;
    int viewHeight = m_height - kDrawMethodBorder * 2; //we take off xtra for the title bar
    
    //compute font size
    if (m_lastArraySize != len) {
        int desiredFontSize = 10;
        //Make sure the font size fits in the view size
        static char test[800];
        memset(test, 'p', 800);
        test[len*4] = 0;
        
        do {
            delete m_arrayFont;
            m_arrayFont = new ofTrueTypeFont;
            m_arrayFont->loadFont("Inconsolata.otf",desiredFontSize,true, true);
            m_currFontSize = desiredFontSize--;
        } while (desiredFontSize > 3 &&
                 m_arrayFont->stringWidth(test) > viewWidth - 2 * kDrawMethodBorder)
            ;
        
        m_lastArraySize = len;
    }
    m_rowsNeeded = viewHeight / m_arrayFont->getLineHeight();
    
    //console is a special case cause we mix console messages with the array history.
    //maintain a vector of <history array ptr key, console string> that keeps track
    //of when in the history the string came out.  The console string should be printed
    //after the history array.
    //a key of NULL means it is on the top.
    //we only need to keep track of CalcNumRowsNeeded() of these because the display will be full then.
    console_map m;
    do {
        m = SortingMan::Instance()->PopOutput();
        if (m.message)
            m_consoleOutput.push_back(m);
    } while(m.message)
        ;
    
    //detect if we have too many messages
    while (m_consoleOutput.size() > m_rowsNeeded) {
        free(m_consoleOutput[0].message);
        m_consoleOutput.erase(m_consoleOutput.begin() + 0);
    }
    
    return m_rowsNeeded;
}

