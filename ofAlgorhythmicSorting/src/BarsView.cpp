#include "BarsView.h"
#include "ofMain.h"
#include "SortingMan.h"
#include "AlgorhythmicAudioIO.h"
#include "handyfuncs.h"
void BarsView::Draw(std::vector<double*> &arrays, std::vector<int*> &markers)
{
    if(!arrays.size())
        return;
    int len = SortingMan::Instance()->GetArraySize();
    double* array = arrays[arrays.size() - 1];
    int* status = markers[markers.size() - 1];
    int currentIndex = AlgorhythmicAudioIO::Instance()->CurrentIndex();
    float y1,y2;
    for(int i=0;i<len;i++){
        
        int color;
        color = kHighNumberBrightness * 3 /4 * ((float) (array[i]+100)/200.0) + kHighNumberBrightness/4;//brightness
        color |= color << 8 | color << 16;
        if(status[i] & kMarkerChanged)
            color &= 0xFF22FF; //take out some green and blue components
        else if(status[i] & kMarkerCompared)
            color &= 0x22FFFF; //take out some green and red components
        else if(status[i] & kMarkerFocused)
            color &= 0xFFFF22; //take out some red and blue components
        
        y1 = m_height/2;
        y2 = (1.0 + (array[i]/-100.0)) * m_height/2;
        
        ofFill();
        ofSetHexColor(color);
        ofRect(m_x + m_width * ((float)i/len), m_y + mymin(y1,y2),
               (float)m_width/len, fabs(y1-y2));
        if (currentIndex == i) {
            ofSetHexColor(0xFFFFFF);
            ofNoFill();
            ofRect(m_x + m_width * ((float)i/len), m_y + mymin(y1,y2),
                   (float)m_width / len - 1, fabs(y1-y2) - 1);
        }   
        
    }
}