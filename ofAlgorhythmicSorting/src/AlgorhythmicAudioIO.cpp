/*
 *  AlgorhythmicAudioIO.c
 *
 *  Created by michael takezo chinen on 12/19/09.
 */

#include "AlgorhythmicAudioIO.h"
#include "portaudio.h"
#include "stdio.h"
#include <cstdlib>
#include "handyfuncs.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <limits.h>

void SortingInitAudio();
void SortingStartAudio();


#define SAMPLE_RATE   (44100)
#define NUM_CHANNELS 2

typedef struct
{
    float left_phase;
    float right_phase;
}
paTestData;

static paTestData data;
static bool gManagerCreated = false;

//actual desired params
float nb_freq =1000;
float nb_bw = 0.99;
float nb_phase = 0.0;
float nb_amp = 0.5;

//for real time state keeping
bool laston;
float lastfreq;
float lastamp;
float lastbw;
float instfreq;
float phase;
float targetfreq;
int samplesTillNextTarget;
bool initedForRealTime = false;


//generate a block the size of the buffer.
void NoiseBandFillFrame(float* buf, int length, float vol)
{
    if(!initedForRealTime)
    {
        
        laston = false;
        lastfreq = nb_freq;
        lastamp  = 0.0;
        lastbw  = nb_bw;
        instfreq = nb_freq;
        targetfreq = nb_freq;
        
        phase = nb_phase;
        samplesTillNextTarget=0;
        initedForRealTime = true;
    }
    
    
    double phaseAdd;
    bool on = true;
    float amp = lastamp;
    
    float freq = lastfreq;
    float bw    =lastbw;
    //for quick ref
    
    float      curamp;
    float      curfreq;
    float      curbw;
    
    curamp = nb_amp;
    curfreq = nb_freq;
    curbw = nb_bw;
    
    
    laston = on;
    
    if(on||laston)
    {
        for(int j = 0 ; j<length ; j++ )
        {
            
            bw = curbw;//curpg->Interpolate(3,lastbw,curbw,length==0?1.0:(float)j/(float)length,bwFormat);
            freq = curfreq;//curpg->Interpolate(1,lastfreq,curfreq,length==0?1.0:(float)j/(float)length,freqFormat);
            amp = curamp;//curpg->Interpolate(0,lastamp,on?curamp:0.0,length==0?1.0:(float)j/(float)length,ampFormat);
            
            
            //see if we need a new target
            if(samplesTillNextTarget <0)
            {
                targetfreq = randfloat(freq*bw*2.0)+freq*(1.0-bw);  //assumes bw <1.0
                samplesTillNextTarget = randint(44100/mymax(2.0,freq))+1.0;
            }else
            {
                samplesTillNextTarget--;
            }
            
            instfreq = instfreq + (targetfreq-instfreq)/mymax(1.0,samplesTillNextTarget);
            
            phaseAdd = instfreq*2.0*3.141592/44100.0;
            //phaseAdd += phaseAdd* (randfloat(bw*2)-bw);
            
            phase += phaseAdd;
            while(phase>2.0*3.141592)
                phase-=2.0*3.141592;
            
            buf[j*2]+=amp*sin(phase) *vol;
            buf[j*2+1]+=amp*sin(phase) * vol;
            
            
        }
    }else
    {
        //advance phase, but try not to loose precision with a large to small addition.
        phaseAdd = length*freq*2.0*3.141592/44100.0;
        while(phaseAdd>2.0*3.141592)
            phaseAdd-=2.0*3.141592;
        
        phase += phaseAdd;
        while(phase>2.0*3.141592)
            phase-=2.0*3.141592;
    }
    
    lastamp = amp;
    lastfreq = freq;
    lastbw = bw;
    
}

static void ComputeCurrentIndex(int localIndex)
{
    AlgorhythmicAudioIO *io = AlgorhythmicAudioIO::Instance();
    
    io->ComputeCurrentIndex(localIndex);
}
void AlgorhythmicAudioIO::ComputeCurrentIndex(int localIndex)
{
    if (m_playingIndividual)
        m_currentIndex = localIndex ? m_playIndexB : m_playIndexA;
    else
        m_currentIndex = m_playIndexA + localIndex;
    
}

int nb_samples_left = 0;
int nb_samples_total = 0;
/* This routine will be called by the PortAudio engine when audio is needed.
 ** It may called at interrupt level on some machines so don't do anything
 ** that could mess up the system like calling malloc() or free().
 */
static int SortingPortAudioCallback( const void *inputBuffer, void *outputBuffer,
                                    unsigned long framesPerBuffer,
                                    const PaStreamCallbackTimeInfo* timeInfo,
                                    PaStreamCallbackFlags statusFlags,
                                    void *userData )
{
    int noisefactor;
    noisefactor = 1;
    
    float vol = AlgorhythmicAudioIO::Instance()->GetVolume();
    float freqMin = AlgorhythmicAudioIO::Instance()->GetFreqMin();
    float freqRange = AlgorhythmicAudioIO::Instance()->GetFreqMax() - freqMin;
    bool notesOn = AlgorhythmicAudioIO::Instance()->IsNotesOn();
    bool sepOn   = AlgorhythmicAudioIO::Instance()->IsSeperatorOn();
    bool click = false;
    
    for( int z = 0; z < NUM_CHANNELS; z++ )
        memset(((float*)outputBuffer)+framesPerBuffer*z, 0, framesPerBuffer*sizeof(float));
    
    //sonify stuff
	struct ssortdata* dataa = AlgorhythmicAudioIO::Instance()->GetSortData();
	int index;
	
	if(dataa->usedouble)
	{
        AlgorhythmicAudioIO::Instance()->LockSortData();
        
        //if this is a fresh array
        if(!dataa->seperatorPlayed)
        {
            click = true;
            nb_samples_left = nb_samples_total = dataa->duration * SAMPLE_RATE/8;
            dataa->seperatorPlayed=true;
        }
        
		//double array.
        //duration is for the entire array.
        //get the index of the number we are interested in by looking at how much we've played back so far.
        if(dataa->durations && dataa->lastNoteIndex< dataa->durationsSize && dataa->sampleCursor > dataa->durations[dataa->lastNoteIndex] * SAMPLE_RATE)
        {
            dataa->sampleCursor=0;
            dataa->lastNoteIndex++;
        }
		index = dataa->lastNoteIndex;
        ComputeCurrentIndex(index);
        
		if(dataa->doubleArray && index <= dataa->doubleArraySize && dataa->durations && dataa->durations[dataa->lastNoteIndex]* SAMPLE_RATE> dataa->sampleCursor)
		{
            float pan = 0.5f;
			float freq = 0.0f;
            if(index < dataa->doubleArraySize)
            {
                freq =  (dataa->doubleArray[index] + 100) / 200.0; //hack - values range from -100 to 100
                freq = freqRange*exp2cursor(freq, (logf(freqRange+freqMin)/logf(2) - logf(freqMin)/logf(2)))+freqMin;
                pan = ((float)index)/dataa->doubleArraySize;
			}
            
            if(sepOn && nb_samples_left) {
                nb_freq = 10000/(dataa->seperatorDepth + 1);
                
                NoiseBandFillFrame((float*) outputBuffer, framesPerBuffer, vol);
                for(int i=0;i<framesPerBuffer;i++) {
                    ((float*)outputBuffer)[i*2]   *= (float)nb_samples_left/nb_samples_total;
                    ((float*)outputBuffer)[i*2+1] *= (float)nb_samples_left/nb_samples_total;
                    if(nb_samples_left>0)
                        nb_samples_left--;
                }
            }
            
			for(unsigned int i = 0; i < framesPerBuffer*NUM_CHANNELS; i+=2)
			{
                float sinval;
                sinval = sin(dataa->phase);
                if(notesOn)//old sep || (index == dataa->doubleArraySize && sepOn ))
                {
                    ((float*)outputBuffer)[i]   += sinval * (1.0-pan) * vol * 0.5;
                    ((float*)outputBuffer)[i+1] += sinval * pan       * vol * 0.5;
                }
                if(1 || click) {
                    float bassmult = 16*dataa->seperatorDepth+1;
                    //               ((float*)outputBuffer)[i]   +=  vol*2*((float)mymax(0,(framesPerBuffer*NUM_CHANNELS-(i*bassmult)))/framesPerBuffer*NUM_CHANNELS);
                    //               ((float*)outputBuffer)[i+1] +=  vol*2*((float)mymax(0,(framesPerBuffer*NUM_CHANNELS-(i*bassmult)))/framesPerBuffer*NUM_CHANNELS);
                }
				//inc phase if we are under the size, otherwise use noise.
                if(index < dataa->doubleArraySize)
				{
                    dataa->phase+=2.0*3.14592 *freq / SAMPLE_RATE;
                    while(dataa->phase>2.0*3.14592)
                        dataa->phase-=2.0*3.14592;
                }
                else
                {
                    //noise
                    if(dataa->lastNoteIndex<= dataa->doubleArraySize && dataa->durations[dataa->lastNoteIndex] !=0)
                    {
                        dataa->phase = randfloat(2.0*3.14592);
                        pan = 0.5;
                    }
                }
                dataa->sampleCursor++;
                
                //see if we've stepped over a boundry
                if(dataa->sampleCursor > dataa->durations[dataa->lastNoteIndex] * SAMPLE_RATE)
                {
                    dataa->sampleCursor=0;
                    dataa->lastNoteIndex++;
                    index = dataa->lastNoteIndex;
                    ComputeCurrentIndex(index);
                    if(!(dataa->lastNoteIndex<= dataa->doubleArraySize))
                        break;
                    else if (dataa->lastNoteIndex < dataa->doubleArraySize)
                    {
                        freq = (dataa->doubleArray[index] + 100) / 200.0; //hack - values range from -100 to 100
                        freq = freqRange*exp2cursor(freq, (logf(freqRange+freqMin)/logf(2) - logf(freqMin)/logf(2)))+freqMin;
                        pan = ((float)index)/dataa->doubleArraySize;
                    }
                }
			}
		}
        AlgorhythmicAudioIO::Instance()->unLockSortData();
	}
    
    
    // Clip output to [-1.0,+1.0] range
    for(unsigned int i = 0; i < framesPerBuffer*NUM_CHANNELS; i++)
    {
        float f = ((float*)outputBuffer)[i];
        if (f > 1.0f)
            ((float*)outputBuffer)[i] = 1.0f;
        else if (f < -1.0f)
            ((float*)outputBuffer)[i] = -1.0f;
    }
    return 0;
}

AlgorhythmicAudioIO* AlgorhythmicAudioIO::Instance()
{
    static AlgorhythmicAudioIO* man=NULL;
    //this isn't 100% threadsafe but I think Okay for this purpose.;
    if(!man)
    {
        man = new AlgorhythmicAudioIO();
        man->Init();
        gManagerCreated = true;
    }
    return man;
}

AlgorhythmicAudioIO::AlgorhythmicAudioIO()
{
    m_stream = NULL;
}

AlgorhythmicAudioIO::~AlgorhythmicAudioIO()
{
    //[[NSAutoreleasePool alloc] init];
    //[audioDataMutex release];
}

//Change Preset values
bool AlgorhythmicAudioIO::Init()
{
    PaError err;
    m_speed = 0.025;
    m_seperatorOn=m_notesOn=true;
    m_freqMin = 100;
    m_freqMax = 4000;
    m_volume = 0.8;
    
    /* Initialize our data for use by callback. */
    data.left_phase = data.right_phase = 0.0;
	mzsortdata.intArray = NULL;
	mzsortdata.doubleArray = NULL;
	mzsortdata.durations = NULL;
    mzsortdata.lastNoteIndex=0;
    mzsortdata.durationsSize=0;
    mzsortdata.seperatorDepth = 0;
    mzsortdata.seperatorPlayed = true;
    /* Initialize library before making any other calls. */
    err = Pa_Initialize();
    m_actualLatency = 0.0001f;
    m_inited= err == paNoError ;
    
    //    audioDataMutex = [[NSLock alloc] init];
    
    return m_inited;
}

void AlgorhythmicAudioIO::ChangeFreqMin(float min)
{
    m_freqMin = mymax(mymin(20000,min),20);
}

void AlgorhythmicAudioIO::ChangeFreqMax(float max)
{
    m_freqMax = mymax(mymin(20000,max),m_freqMin);
}

void AlgorhythmicAudioIO::LockSortData()
{
    audioDataMutex.lock();
}

void AlgorhythmicAudioIO::unLockSortData()
{
    
    audioDataMutex.unlock();//   [audioDataMutex unlock];
}
void AlgorhythmicAudioIO::PlaySeperator(int depth){
    mzsortdata.seperatorDepth = depth;
    mzsortdata.seperatorPlayed = false;
}

float AlgorhythmicAudioIO::PlayDoubleArray(double * array, int size)//, int swapaindex, int swapbindex)
{
    mzsortdata.approxEvals+=size;
    
    if(array!=mzsortdata.doubleArray || size != mzsortdata.doubleArraySize)
    {
        LockSortData();
        mzsortdata.doubleArray=array;
        mzsortdata.doubleArraySize=size;
        unLockSortData();
    }
    
    if(mzsortdata.durationsSize<size)
    {
        LockSortData();
        if(mzsortdata.durationsSize)
            delete [] mzsortdata.durations;
        mzsortdata.durationsSize=size;
        //durations needs one greater for the noise seperator duration.
        mzsortdata.durations = new double[size+1];
        
        unLockSortData();
    }
	mzsortdata.sampleCursor=0;
	mzsortdata.phase=0;
	mzsortdata.usedouble=true;
    
    mzsortdata.lastNoteIndex=0;
    float secDuration = 0.0;
    
    int min = INT_MAX;
    int max = INT_MIN;
    for(int i=0;i<size;i++)
    {
        min = mymin(array[i],min);
        max = mymax(array[i],max);
    }
    
    
    //make up some durations based on the number.
    for(int i=0;i< size;i++)
    {
        //calculate the delay per note,
        //      mzsortdata.durations[i] = (((int)array[i])%20)/20.0 + 0.05 ;
        
        //short notes
        //      {
        //         int chop;
        //         chop = (int)array[i];
        //         mzsortdata.durations[i] = 0.05;
        //         while(fabs(chop) > 0)
        //         {
        //            mzsortdata.durations[i] *= 1.75;
        //            chop = chop/3;
        //         }
        //      }
        {
            float cursor;
            cursor = (array[i] - min)/(max-min);
            cursor = 1.0-cursor;
            cursor = exp2cursor(cursor,7);
            mzsortdata.durations[i] =m_speed;//(0.05 + cursor * (0.45) )* m_speed;
        }
        
        //we sum up the durations
        secDuration += mzsortdata.durations[i];
    }
    //duration of break
    mzsortdata.durations[size] = 0.0;//(m_seperatorOn?1.0:0.0) *m_speed;
    secDuration += mzsortdata.durations[size];
    mzsortdata.duration=secDuration;
	return secDuration;
}

void AlgorhythmicAudioIO::PlayIntArray(int * array, int size, float duration)
{
    
	mzsortdata.intArray=array;
	mzsortdata.intArraySize=size;
	mzsortdata.duration=duration;
	mzsortdata.sampleCursor=0;
	mzsortdata.phase=0;
	mzsortdata.usedouble=false;
	
}

bool AlgorhythmicAudioIO::Start()
{
    if(!m_inited && !Init())
        return false;
    
    PaError err;
    /* Open an audio I/O stream. */
    PaDeviceIndex outDev = Pa_GetDefaultOutputDevice();
    PaStreamParameters playbackParameters;
    playbackParameters.device = outDev;
    playbackParameters.sampleFormat = paFloat32;
    playbackParameters.hostApiSpecificStreamInfo = NULL;
    playbackParameters.channelCount = 2;
    playbackParameters.suggestedLatency = kAudioLatency/1000.0;
    
    err = Pa_OpenStream( &m_stream,
                        NULL, &playbackParameters,
                        SAMPLE_RATE, /*paFramesPerBufferUnspecified, */256,
                        paNoFlag,
                        SortingPortAudioCallback, &data  );
    
    if( err != paNoError ) return false;
    
    
    err = Pa_StartStream( m_stream );
    if( err != paNoError ) return false;
    
    const PaStreamInfo *info;
    info = Pa_GetStreamInfo(&m_stream);
    
    if(info && info->outputLatency > 0)
        m_actualLatency = info->outputLatency;
    else {
        m_actualLatency = (256.0/44100.0)/1000.0;
    }
    
    return true;
}

bool AlgorhythmicAudioIO::Stop()
{
    PaError err;
    
    err = Pa_StopStream( m_stream );
    if( err != paNoError ) return false;
    err = Pa_CloseStream( m_stream );
    if( err != paNoError ) return false;
    
    return true;
}

void AlgorhythmicAudioIO::Terminate()
{
    Pa_Terminate();
}
