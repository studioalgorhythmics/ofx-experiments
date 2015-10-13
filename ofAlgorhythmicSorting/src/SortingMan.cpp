//SortingMANager that manages the threads and GUI settings
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include "SortingMan.h"
#include "AlgorhythmicAudioIO.h"
#include "handyfuncs.h"
#include "sorthelp.h"

#include "ofMain.h"

void* sortstuffhelper (void * arg);

class SortThread:public ofThread {
public:
    SortThread(){}
    virtual ~SortThread(){}
    
    void SetPriv(void* p){priv = p;}
protected:
    //-------------------------------------------------
    //you need to overide this with the function you want to thread
    virtual void threadedFunction(){
        sortstuffhelper(priv);
    }
    
    void* priv;
};

void SortingMan::AppendToOutput(const char* inputString)
{
    int len = strlen(inputString) + 1;
    char *entry = (char*)malloc(sizeof(char) * len);
    console_map m;
    
    memcpy(entry, inputString, sizeof(char) * len);
    m.message = entry;
    
    stringLock.lock();
    m.ownerArrayPtr = m_lastArrayPtr;
    m_outputStrings.push_back(m);
    stringLock.unlock();
}

console_map SortingMan::PopOutput()
{
    console_map ret;
    
    stringLock.lock();
    if(m_outputStrings.size()) {
        ret  = m_outputStrings[0];
        m_outputStrings.erase(m_outputStrings.begin() + 0);
    } else {
        ret.message = NULL;
    }
    stringLock.unlock();
    
    //users will need to delete ret.message if non null
    return ret;
}

void SortingMan::AppendToHistory(double *array, int *marker, int n)
{
    stringLock.lock();
    double *entry = (double*)malloc(sizeof(double) * n);
    memcpy(entry, array, sizeof(double) * n);
    m_history.push_back(entry);
    
    int *marks = (int*)malloc(sizeof(int) * n);
    memcpy(marks, marker, sizeof(int) * n);
    m_markers.push_back(marks);
    m_lastArrayPtr = entry;
    stringLock.unlock();
}

int SortingMan::PopHistory(double **arrayOut, int **markerOut)
{
    double *ret  = NULL;
    int    *ret2 = NULL;
    //we are recylcing mutexes.
    stringLock.lock();
    if(m_history.size()) {
        ret  = m_history[0];
        ret2 = m_markers[0];
        m_history.erase(m_history.begin() + 0);
        m_markers.erase(m_markers.begin() + 0);
    } else {
        ret = NULL;
    }
    stringLock.unlock();
    
    *arrayOut  = ret;
    *markerOut = ret2;
    return ret ? 1 : 0;
}



SortingMan* SortingMan::Instance()
{
    static SortingMan* man=NULL;
    //this isn't 100% threadsafe but I think Okay for this purpose.;
    if(!man)
    {
        man = new SortingMan();
        man->Init();
    }
    return man;
}

SortingMan::SortingMan()
{
}

SortingMan::~SortingMan()
{
    
}

bool SortingMan::Init()
{
    int stime;
    /* get the current calendar time */
    m_seed = time(NULL);
    stime = (unsigned) m_seed/2;
    srand(stime);
    
    m_arrayStatus = NULL;
    m_fillStyle = 0;
    //   stringLock = [[NSLock alloc] init];
    m_looping=false;
    m_size = 10;
    m_lastSort.array = new double [m_size];
    m_type  = eBubbleSortAlgorithm;
    GenerateNewArray(m_size);
    m_isSorting=false;
    m_shouldStop = false;
    m_lastArrayPtr = NULL;
    return m_inited;
}

void SortingMan::SetArrayFillStyle(bool sorted, bool reversed, int discrete)
{
    m_fillStyle = eNormalFillStyle;
    if (reversed)
        m_fillStyle |= eReverseSortedFillStyle;
    else if (sorted)
        m_fillStyle |= eSortedFillStyle;
    
    if (discrete)
        m_fillStyle |= eDiscreteFillStyle | (discrete << 8); //the second byte is the number of discrete elements
}



void SortingMan::SortStuff()
{
    //run the sorting algorithm on a different thread.
    //we can only run one at a time because of OF windows i think we need
    //to keep the thread ref around (unlike pthreads)
    static SortThread thread;
    
    if(thread.isThreadRunning())
        return;
    
    while(ShouldStop()) {
#ifdef WIN32
		Sleep(1);			//windows sleep in milliseconds
#else
		usleep(100);	//mac sleep in microseconds - cooler :)
#endif
    }
    
    isSortingLock.lock();
    m_isSorting=true;
    isSortingLock.unlock();
    m_lastSort.size = m_size;
    m_lastSort.algo = m_type;
    m_lastSort.shouldStop = false;
    m_lastSort.seed = m_seed;
    
    //we have to lock a mutex cause it gets used in audio thread.
    AlgorhythmicAudioIO::Instance()->LockSortData();
    delete [] m_lastSort.array;
    m_lastSort.array = new double[m_size];
    AlgorhythmicAudioIO::Instance()->unLockSortData();
    
    thread.SetPriv(&m_lastSort);
    //thread.startThread(true,false);
    thread.startThread();
}

void SortingMan::StopSorting()
{
    if(IsSorting())
    {
        m_lastSort.shouldStop = true;
        m_shouldStop = true;
    }
}


bool SortingMan::IsSorting()
{
    bool ret;
    isSortingLock.lock();
    ret = m_isSorting;
    isSortingLock.unlock();
    return ret;
}
void SortingMan::SetDoneSorting()
{
    isSortingLock.lock();
    m_isSorting=false;
    m_shouldStop = false;
    isSortingLock.unlock();
}


void SortingMan::SetAlgorithm(SortAlgorithms newType)
{
    m_type = newType;
}

void SortingMan::GenerateNewArray(int size)
{
    m_seed = time(NULL);
    m_size = size;
}



// insert sorting
//void PIKSRT(int N, double *ARR);
//cleaner one
void insertion_sort(double a[], int length);
void SHELL(int n, double *ARR) ;
void HPSORT(int n, double *RA);
int heapsort( double * data, int n );
void quicksort(double arr[], int n);

void Order(double* p,double* q);
void Bubble(double* something,int n); //bubble sort
void sort(double *a, int lo, int hi, int depth = 0);//merge sort

void dobubblesort (sort_params* params) {
	//printdoublearray(params->array, params->size);
	Bubble(params->array,params->size);
}

void *sortstuffhelper (void * arg)
{
	
    //   [[NSAutoreleasePool alloc] init];
    sort_params* params = (sort_params*) arg;
    
	AlgorhythmicAudioIO::Instance()->Start();
    
	int i,h, n=params->size;
	double* a = params->array;
    
    char mystring[256];
    const char *algoName;
    //sort a few arrays
    do
    {
        int freqMin = AlgorhythmicAudioIO::Instance()->GetFreqMin();
        int freqMax = AlgorhythmicAudioIO::Instance()->GetFreqMax();
        //get the name of the algo - this can change
        algoName = gAlgoStrings[params->algo];
        
        sprintf(mystring,"%s with array of length %d, speed %f, pitch range %d-%d\n",
                algoName,n,AlgorhythmicAudioIO::Instance()->GetSpeed(),freqMin,freqMax);
        
        SortingMan::Instance()->AppendToOutput(mystring);
        //create array based on seed
        int stime;
        stime = (unsigned) params->seed/2;
        srand(stime);
        
        AlgorhythmicAudioIO::Instance()->LockSortData();
        fillDoubleArray(a,n, SortingMan::Instance()->GetFillStyle());
        AlgorhythmicAudioIO::Instance()->unLockSortData();
        
        AlgorhythmicAudioIO::Instance()->ResetEvalCount();
        //NSDate *date = [NSDate date];
        float startTimeSecs = ofGetElapsedTimef();
        switch (params->algo) {
                //bubble sort
            case eBubbleSortAlgorithm:
                dobubblesort(params);
                break;
                
                //DO MSORT
            case eMergeSortAlgorithm:
                sort(a,0, n-1);
                break;
                //DO insertion
            case eInsertionSortAlgorithm:
                insertion_sort(a,n);
                break;
            case eShellSortAlgorithm:
                SHELL(n,a);
                break;
            case eHeapSortAlgorithm:
                //HPSORT(n,a);
                heapsort( a, n );
                break;
            case eQuicksortSortAlgorithm:
                quicksort( a, n );
                break;
            default:
                break;
        }
        
        float elapsedTimeSecs = ofGetElapsedTimef() - startTimeSecs;
        //print the array clean
        printdoublearray(a, n,0,0);
        static int debug = 0;
        sprintf(mystring, "%s %s. %i comparisons/moves. elapsed time:%.2f ms\n",
                algoName,
                SortingMan::Instance()->ShouldStop()?"cancelled":"finished",
                AlgorhythmicAudioIO::Instance()->GetEvalCount(),
                elapsedTimeSecs);
        
        SortingMan::Instance()->AppendToOutput(mystring);
        
    }while(SortingMan::Instance()->IsLooping() && !SortingMan::Instance()->ShouldStop())
        ;
    SortingMan::Instance()->SetDoneSorting();
    AlgorhythmicAudioIO::Instance()->RemoveCurrentIndex();
	AlgorhythmicAudioIO::Instance()->Stop();
	return 0;
	
}



