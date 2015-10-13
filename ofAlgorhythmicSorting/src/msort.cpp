/*********************************************************
 *   Demonstration Program of In-place Merge Sorting      *
 *  (about n*n comparisons used with no extra storage).   *
 * ------------------------------------------------------ *
 * Reference: After a java algorithm By Jason Harrison,   *
 *            1995 University of British Columbia.        *
 *                                                        *
 *                    C++ Version By J-P Moreau, Paris.   *
 * ------------------------------------------------------ *
 *                                                        *
 *********************************************************/

#include <stdio.h>
#include <math.h>
#include "sorthelp.h"
#include "SortingMan.h"

/*****************************************************
 * In-place sorting of a table a[] in ascending order *
 * -------------------------------------------------- *
 * Inputs:                                            *
 *         a :  Table of n real numbers               *
 *         lo:  Starting index (>=0)                  *
 *         hi:  Ending index (<=n-1), with lo<hi.     *
 * Output:                                            *
 *         a :  Table sorted in ascending order.      *
 *****************************************************/

// >>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>

// merge sorting:
// assume that also 1 value is a sorted list, so next step you compare 2 values each of a array with x values,
// and then 4 and then 8 etc. There is natural merge sorts and inplace mergesorts. Natural mergesort uses more arrays.


void sort(double *a, int lo, int hi, int depth ) { // lo=0, hi=n-1 (only for the first call)
    // lo and hi are index
    
    static double* original;
    static int originalsize;
    
    if(SortingMan::Instance()->ShouldStop())
        return;
    
    if(depth==0){
        
        original = a;
        originalsize = hi+1;
    }
    
    // >>>>>>>>>>>>>>>>
    
    if (lo>=hi) return; // no action if the array is already sorted
    
    int mid = (lo + hi) / 2; // for example 9 / 2 = 4.5 >> 4
    
    // Recursion breaks up the array in to smaller pieces before they are going to be sorted!
    
    // Partition the list into two lists and sort them recursively
    sort(a, lo, mid, depth+1); // compare with void sort(double *a, int lo, int hi, bool toplevel)
    sort(a, mid + 1, hi, depth+1);
    
    
    // Merge the two sorted lists
    int start_hi = mid + 1;
    int end_lo = mid;
    
    // [2,3] [5,4] [0,5] [4,6] >> 2 at index 3, 5 at index 4 ...
    // code which is run on each piece of the whole array
    while ((lo <= end_lo) && (start_hi <= hi)) {
        printseperator(depth);
        printdoublearray(original, originalsize, lo, (start_hi-lo)+1, lo, start_hi);	// Sonification
        if (a[lo] < a[start_hi]) // if value of array a at index lo is smaller value at start-hi
        {
            
            //blue means a[lo] < a[start_hi]. lo = first value of a chunk
            
            lo++; // lo gets incremented
            
            
        }
        
        else {
            
            // a[lo] >= a[start_hi]
            
            // The next element comes from the second list,
            // move the a[start_hi] element into the next
            // position and shuffle all the other elements up.
            
            double T = a[start_hi]; // [2,3] [5,4] [0,5] [4,6]
            
            // 0 (the value at index 5) goes to a storage called T
            
            
            for (int k = start_hi-1; k >= lo; k--) { // count down from start-1 to lo
                a[k+1] = a[k]; // [2,3] [5,4] [<0>,5] [4,6] >> [2,3] [5,4] [<5>,5] [4,6]
                // = is like arrow pointing to left
                
                // makes a line of red values, because all values change
                
            }
            
            a[lo] = T;
            
            //print and play as an array segment instaid of a pair since the above for loop moves as a chunk (like memcpy)
            printdoublearray(original, originalsize, lo, (start_hi-lo)+1); // Sonification
            
            // 
            
            lo++;
            end_lo++;
            start_hi++;
            
        }
        
        if(SortingMan::Instance()->ShouldStop())
            return;
    }
}