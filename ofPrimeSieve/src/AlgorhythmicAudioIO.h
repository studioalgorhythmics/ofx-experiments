/*
 *  AlgorhythmicAudioIO.h
 *
 *  Created by michael takezo chinen on 12/19/09.
 *
 *   A singleton that manages SORTING audio
 *//*******************************************************************/





#ifndef __AlgorhythmicAudioIO__
#define __AlgorhythmicAudioIO__

//audio latency in millis
#define kAudioLatency 80.0

#include "portaudio.h"

class AlgorhythmicAudioIO
{
public:
    ///Gets the singleton instance
    static AlgorhythmicAudioIO* Instance();
    ///init audio, but don't start the stream
    bool Init();
    
    ///Start the stream
    bool Start();
    bool Stop();
    void Terminate();
    
    float GetSpeed(){return m_speed;}
    void ChangeSpeed(float newSpeed){m_speed=newSpeed;}
    void ToggleSeperator(){m_seperatorOn = !m_seperatorOn;}
    
    void ToggleNotes(){m_notesOn = !m_notesOn;}
    void SetSeperator(bool yn) {m_seperatorOn = yn;}
    void SetNotes(bool yn) {m_notesOn = yn;}
    
    void ChangeVolume(float vol){m_volume=vol;}
    
    void ChangeFreqMin(float min);
    
    void ChangeFreqMax(float max);
    
    float GetVolume(){return m_volume;}
    bool  IsNotesOn(){return m_notesOn;}
    bool  IsSeperatorOn(){return m_seperatorOn;}
    float GetFreqMin(){return m_freqMin;}
    float GetFreqMax(){return m_freqMax;}
    
    float GetActualLatency(){return m_actualLatency;}
    
    void SetIndividual(bool i) { m_playingIndividual = i; }
    void SetAIndex(int a)      { m_playIndexA        = a; }
    void SetBIndex(int b)      { m_playIndexB        = b; }
    int  CurrentIndex()        { return m_currentIndex; }
    void  RemoveCurrentIndex()  { m_currentIndex = -1; }
    void ComputeCurrentIndex(int localIndex);
    
    int GetFramesPerBuffer();
protected:
    //private constructor - Singleton.
    AlgorhythmicAudioIO();
    
    virtual ~AlgorhythmicAudioIO();
    
    PaStream*      m_stream;
    float          m_actualLatency;
    bool           m_inited;
    float          m_speed;
    float          m_volume;
    float          m_freqMin;
    float          m_freqMax;
    
    bool           m_seperatorOn;
    bool           m_notesOn;
    
    //these variables let the audio thread compute which index is currently being played back
    bool           m_playingIndividual;
    int            m_playIndexA;
    int            m_playIndexB;
    int            m_currentIndex;
    
};

#endif

