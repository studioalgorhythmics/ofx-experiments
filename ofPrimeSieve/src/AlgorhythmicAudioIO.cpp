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

#include "ofApp.h"

void SortingInitAudio();
void SortingStartAudio();


#define SAMPLE_RATE   (44100)
#define NUM_CHANNELS 2
#define kFramesPerBuffer 256
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
    float* sieveBuf = pop_buffer();
    
    for (int i = 0; i < framesPerBuffer; i ++) {
        for( int z = 0; z < NUM_CHANNELS; z++ ) {
            ((float*)outputBuffer)[i*NUM_CHANNELS + z] = sieveBuf[i];
            //      memcpy((((float*)outputBuffer)+framesPerBuffer*z), sieveBuf, framesPerBuffer*sizeof(float));
        }
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
                        SAMPLE_RATE, /*paFramesPerBufferUnspecified, */kFramesPerBuffer,
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
        m_actualLatency = (kFramesPerBuffer/44100.0)/1000.0;
    }
    
    return true;
}


int AlgorhythmicAudioIO::GetFramesPerBuffer()
{
    return kFramesPerBuffer;
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
