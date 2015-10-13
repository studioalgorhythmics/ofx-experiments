#include <stdio.h>
#include "sorthelp.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <algorithm>
#include "AlgorhythmicAudioIO.h"
#include "handyfuncs.h"

#include "SortingMan.h"

static int *arrayStatus = NULL;
static double doublePair[2];
void printdoublearray(double* array, int size, int focusstart, int focussize, int swapaindex, int swapbindex){
	int i;
    //audio has latency so we do this first then rest by the latency amt.
    float secondsToRest;
    if(swapaindex!=-1 && swapbindex !=-1)
    {
        //if we were given a pair, we just play the pair.
        doublePair[0]=array[swapaindex];
        doublePair[1]=array[swapbindex];
        //mark that we are just playing two possibly seperated values and their indexes
        //so we audio io thread can tell us which index is currently being played
        AlgorhythmicAudioIO::Instance()->SetIndividual(true);
        AlgorhythmicAudioIO::Instance()->SetAIndex(mymin(swapaindex, swapbindex));
        AlgorhythmicAudioIO::Instance()->SetBIndex(mymax(swapaindex, swapbindex));
        secondsToRest = AlgorhythmicAudioIO::Instance()->PlayDoubleArray((double*)doublePair, 2); //
    }else if(swapaindex!=-1)
    {
        //some cases have just one number in focus
        doublePair[0]=array[swapaindex];
        AlgorhythmicAudioIO::Instance()->SetIndividual(true);
        AlgorhythmicAudioIO::Instance()->SetAIndex(swapaindex);
        secondsToRest = AlgorhythmicAudioIO::Instance()->PlayDoubleArray((double*)doublePair, 1); //
    }else
    {
        // play the values (freq) of a block
        //in the array case we overload A index as starting point and b as size
        AlgorhythmicAudioIO::Instance()->SetIndividual(false);
        AlgorhythmicAudioIO::Instance()->SetAIndex(focusstart);
        AlgorhythmicAudioIO::Instance()->SetBIndex(focussize);
        secondsToRest = AlgorhythmicAudioIO::Instance()->PlayDoubleArray(array+focusstart, focussize); //
    }
    float secsLatency = AlgorhythmicAudioIO::Instance()->GetActualLatency()/1000.0f;
    
    //rest so the visuals don't popup before audio.
#ifdef WIN32
    timeBeginPeriod(1);
    Sleep(1000*(mymin(secsLatency, secondsToRest)));			//windows sleep in milliseconds
#else
    usleep(1000000*(mymin(secsLatency, secondsToRest)));	//mac sleep in microseconds - cooler :)
#endif
    
    static double* last = NULL;
    static int lastSize = -1;
    bool haveLast = false;
    
    if(-1==focussize)
    {
        focussize=size;
    }
    
    if(-1 == lastSize)
    {
        lastSize = size;
        last = new double[size];
        arrayStatus = new int[size];
    }
    else if(lastSize!=size)
    {
        delete [] last;
        delete [] arrayStatus;
        lastSize = size;
        last = new double[size];
        arrayStatus = new int[size];
    }
    else
    {
        //we have a previous array, so diff the values
        haveLast = true;
        
    }
    
    char myString[64];
    for(i=0; i<size; i++)
    {
        bool changed;
        changed = false;
        arrayStatus[i] = 0;
        //see if the value changed from last array
        if(haveLast)
        {
            if (last[i] != array[i]) {
                arrayStatus[i] |= kMarkerChanged;
                changed= true;
            }
            if (i >= focusstart && i < focusstart + focussize) {
                arrayStatus[i] |= kMarkerFocused;
            }
            if (i == swapaindex || i == swapbindex) {
                arrayStatus[i] |= kMarkerCompared;
            }
        }
        //save
        last[i] = array[i];
        //      sprintf(myString," %3.0f",array[i]);
        
        //      SortingMan::Instance()->AppendToOutput(myString);
    }
    SortingMan::Instance()->AppendToHistory(array, arrayStatus, size);
    SortingMan::Instance()->SetArrayStatus(arrayStatus);
    
    //   sprintf(myString,"\n");
    //   SortingMan::Instance()->AppendToOutput(myString);
    
    
#ifdef WIN32
    timeBeginPeriod(1);
    Sleep(1000 * mymax(0,secondsToRest - secsLatency));			//windows sleep in milliseconds
#else
    usleep(1000000 * mymax(0,secondsToRest - secsLatency));	//mac sleep in microseconds - cooler :)
#endif
    
    
}

void printseperator(int depth)
{
    AlgorhythmicAudioIO::Instance()->PlaySeperator(depth);
}

void printintarray(int* array, int size){
	int i;
	float delayseconds=1;
	for(i=0; i<size; i++){
        printf(" %d ",array[i]); // type (d integer) for float f variable type
#ifdef WIN32
        timeBeginPeriod(1);
		Sleep(1000*(delayseconds/size));			//windows sleep in milliseconds
#else
		usleep(1000000*(delayseconds/size));	//mac sleep in microseconds - cooler :)
#endif
	}
	printf("\n");
    
    AlgorhythmicAudioIO::Instance()->PlayIntArray(array, size, delayseconds); //
    //microseconds
}

void fillIntArray(int* array, int size, int minval, int maxval )
{
    for(int i=0;i<size;i++)
    {
        array[i] = randint(maxval-minval) + minval;
    }
}
void fillDoubleArray(double* array, int size, unsigned int fillStyle, double minval, double maxval)
{
    //for some reason the first value is always the same...
    float junk = randfloat(1.0);
    for(int i=0;i<size;i++)
    {
        array[i] = randfloat(maxval-minval) + minval;
    }
    
    if (fillStyle & eSortedFillStyle) {
        std::vector<double> vec;
        for (int i = 0; i < size; i++)
            vec.push_back(array[i]);
        
        std::sort(vec.begin(), vec.end());
        
        for (int i = 0; i < size; i++) {
            if ((fillStyle & eReverseSortedFillStyle) == eReverseSortedFillStyle)
                array[i] = vec[size - 1 - i];
            else
                array[i] = vec[i];
        }
    }
    
    if (fillStyle & eDiscreteFillStyle) {
        //take the second byte as the number of discrete values
        int numDiscrete = (fillStyle & 0xFF00) >> 8;
        float stepSize;
        if (numDiscrete == 0)
            numDiscrete = 2;
        stepSize = (maxval - minval) / numDiscrete;
        for (int i = 0; i < size; i++) {
            array[i] = ((int) ((array[i] - minval) / stepSize)) * stepSize + minval + stepSize*3.0/4.0;
        }
    }
}
