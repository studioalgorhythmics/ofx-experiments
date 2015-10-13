/*
 *  ConsoleView.h
 *  AlgorhythmicSorting
 *
 *  Created by mchinen on 12/13/10.
 *  Copyright 2010 Institute for Algorhythmics. All rights reserved.
 *
 */

#ifndef __CONSOLE_VIEW_H__
#define __CONSOLE_VIEW_H__

#include "ArrayView.h"
#include "SortingMan.h"

class ofTrueTypeFont;
class ConsoleView : public ArrayView
{
public:
    ConsoleView(int x, int y, int width, int height);
    virtual ~ConsoleView(){}
    
    virtual void Draw(std::vector<double*> &arrays, std::vector<int*> &markers);
    //if the number of rows displayed is more than one override this.
    virtual int CalcNumRowsNeeded();
protected:
    ofTrueTypeFont           *m_arrayFont;
    std::vector<console_map>  m_consoleOutput;
    int                       m_currFontSize;
    int                       m_lastArraySize;
    int                       m_rowsNeeded;
};

#endif