#include "sorthelp.h"
#include "SortingMan.h"

void quicksort_helper(double arr[], int low, int high, int depth =0) ;

/* sort everything inbetween `low' <-> `high' */
int orign;

void quicksort(double arr[], int n)
{
    orign=n;
    quicksort_helper(arr,0,n-1);
}

void quicksort_helper(double arr[], int low, int high, int depth) {
    int i = low;
    int j = high;
    double y = 0;
    /* compare value */
    double z = arr[(low + high) / 2];
    
    /* partition */
    do {
   		printseperator(depth);
        /* find member above ... */
        while(arr[i] < z) {
            printdoublearray(arr,orign, low, high - low, i, (low+high)/2);
            i++;
        }
        
        /* find element below ... */
        while(arr[j] > z) {
            printdoublearray(arr,orign, low, high - low, j, (low+high)/2);
            j--;
        }
        
        
        printdoublearray(arr,orign, low, high - low, i, j);
        if(i <= j) {
            /* swap two elements */
            y = arr[i];
            arr[i] = arr[j];
            arr[j] = y;
            printdoublearray(arr,orign, low, high - low, i, j);
            
            i++;
            j--;
        }
        
        if(SortingMan::Instance()->ShouldStop())
            return;
        
    } while(i <= j);
    
    /* recurse */
    if(low < j)
        quicksort_helper(arr, low, j,depth+1);
    
    if(i < high) 
        quicksort_helper(arr, i, high,depth+1); 
}
