#ifndef __SORTINGMAN__
#define __SORTINGMAN__

#include "ofMain.h"
#include <string>
#include <vector>
//#include "msort.h"
//#include "bubble.h"
//#include "shell.h"
//#include "quicksort.h"
//#include "heap.h"

using std::string;

enum SortAlgorithms {
    eBubbleSortAlgorithm = 0,
    eMergeSortAlgorithm = 1,
    eInsertionSortAlgorithm,
    eShellSortAlgorithm,
    eHeapSortAlgorithm,
    eQuicksortSortAlgorithm,
    eNumSortAlgorithms
};

static const char* gAlgoStrings[]=
{
    "Bubble",
    "Merge",
    "Insert",
    "Shell",
    "Heap",
    "Quick"
};

struct sort_params {
    int size;
    SortAlgorithms algo;
    bool shouldStop;
    double* array;
    long seed;
};

typedef struct console_map {
    double *ownerArrayPtr;
    char   *message;
} console_map;

//for array markers returned by PopHistory()
#define kMarkerChanged  1
#define kMarkerFocused  2
#define kMarkerCompared 4



class SortingMan
{
public:
    ///Gets the singleton instance
    static SortingMan* Instance();
    
    bool Init();
    
    void AppendToOutput(const char* inputString);
    console_map PopOutput();
    
    void AppendToHistory(double *array, int *marker, int n);
    
    //returns 1 when valid pop returned, 0 otherwise
    int PopHistory(double **arrayOut, int **markerOut);
    
    //runs on a different thread;
    void SortStuff();
    void StopSorting();
    
    bool IsSorting();
    void SetDoneSorting();
    
    void SetAlgorithm(SortAlgorithms newType);
    void SetArrayFillStyle(bool sorted = false, bool reversed = false, int discrete = 0);
    unsigned int GetFillStyle() {return m_fillStyle;}
    void GenerateNewArray(int size);
    
    bool ShouldStop() {return m_shouldStop;}
    
    void ToggleLooping() {m_looping = !m_looping;}
    bool IsLooping()     {return m_looping;}
    
    int  GetArraySize()  {return m_size;}
    double*  GetArray()      {return m_lastSort.array; }
    int* GetArrayStatus(){return m_arrayStatus; }
    void SetArrayStatus(int* status){m_arrayStatus = status;}
protected:
    //private constructor - Singleton.
    SortingMan();
    
    virtual ~SortingMan();
    std::vector<console_map>     m_outputStrings;
    std::vector<double*>   m_history;
    std::vector<int*>      m_markers;
    //   pthread_mutex_t m_outViewMutex;
    double *m_lastArrayPtr;
    bool           m_inited;
    bool           m_looping;
    
    int* m_arrayStatus;
    
    //random seed - used to generate duplicates etc
    long           m_seed;
    
    // the type of array we generate - random, sorted, reversed etc
    unsigned int m_fillStyle;
    
    int            m_size;
    SortAlgorithms m_type;
    
    volatile bool           m_isSorting;
    sort_params    m_lastSort;
    bool           m_shouldStop;
    
    ofMutex       stringLock ;
    ofMutex       isSortingLock ;
};

#endif

