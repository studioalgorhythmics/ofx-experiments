#include "SquaresView.h"
#include "SortingMan.h"
#include "AlgorhythmicAudioIO.h"
#include "ofMain.h"
#define kDrawMethodBorder 10
void SquaresView::Draw(std::vector<double*> &arrays, std::vector<int*> &markers)
{
    int len = SortingMan::Instance()->GetArraySize();
    int viewWidth  = m_width  - kDrawMethodBorder * 2;
    int viewHeight = m_height - kDrawMethodBorder * 2;
    int squareSize = viewWidth  / len;
    int numRows  = viewHeight / squareSize;
    int startRow = numRows - arrays.size();
    int currentIndex = AlgorhythmicAudioIO::Instance()->CurrentIndex();
    for(int i = startRow; i < numRows; i++) {
        for(int j = 0; j < len; j++) {
            int val;
            int color;
            val   = arrays[i - startRow][j];
            color = kHighNumberBrightness * 3 / 4 * ((float) (val+100)/200.0) + kHighNumberBrightness/4;//brightness
            color |= color << 8 | color << 16;
            
            if(markers[i - startRow][j] & kMarkerChanged)
                color = color & 0xFF22FF; //take out some green and blue components
            else if(markers[i - startRow][j] & kMarkerCompared)
                color = color & 0x22FFFF; //take out some green and red components
            else if(markers[i - startRow][j] & kMarkerFocused)
                color = color & 0xFFFF22; //take out some red and blue components
            
            SetFill(true);
            SetColor(color);
            DrawRect(m_x + kDrawMethodBorder + j * squareSize,
                     m_y + kDrawMethodBorder + i * squareSize,
                     squareSize, squareSize);
            if (i == numRows - 1 && currentIndex == j) {
                SetColor(0xFFFFFF);
                SetFill(false);
                DrawRect(m_x + kDrawMethodBorder + j * squareSize,
                         m_y + kDrawMethodBorder + i * squareSize,
                         squareSize - 1, squareSize - 1);
            }
        }
    }
}

int SquaresView::GetSquareHeight()
{
    int len = SortingMan::Instance()->GetArraySize();
    int viewWidth  = m_width  - kDrawMethodBorder * 2;
    return viewWidth  / len;
}

int SquaresView::CalcNumRowsNeeded()
{
    int viewHeight = m_height - kDrawMethodBorder * 2;
    return viewHeight / GetSquareHeight();
}

void SquaresView::SetColor(int color)
{
    ofSetHexColor(color);
}

void SquaresView::SetFill(bool fill)
{
    if (fill)
        ofFill();
    else
        ofNoFill();
}

void SquaresView::DrawRect(int x, int y, int w, int h)
{
    ofRect(x, y, w, h);
}

