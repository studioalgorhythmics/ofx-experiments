/*********************************************
 *   Demonstration program of Bubble sorting  *
 *        (about n*n comparisons used).       *
 * ------------------------------------------ *
 * Reference: "A book on C By Al Kelley and   *
 * Ira Pohl, The Benjamin/Cummings Publishing *
 * Company, Inc, 1984" [BIBLI 09].            *
 *                                            *
 *                C++ version by J-P Moreau.  *
 * ------------------------------------------ *
 * SAMPLE RUN:                                *
 *                                            *
 * Initial table A:                           *
 * 7  3  66  3  -5  22  -77  2  36  -12       *
 *                                            *
 * Sorted table A:                            *
 * -77  -12  -5  2  3  3  7  22  36  66       *
 *                                            *
 *********************************************/

#include "sorthelp.h"

#include "SortingMan.h"


// >>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>

// return 0 if no swap, 1 if swap
//p,q in ascending order (swap, when they are not in order)
int Order(double* p,double* q) { // double datentyp is more precise than float
	double temp; // saves the value
	if(*p>*q) { // ie. value at p is 500, value at q is 100
		temp=*p; // temp gets 500
		*p=*q; // value 100(q) gets transferred to the address where 500 was? q doesnt change
		*q=temp; // change q to temps value 500 right now.
        return 1;
	}
    return 0;
}

//Bubble sorting of integer array A[]
void Bubble(double* something,int n) { // n is size of array
	int i,j;
	
	for (i=0; i<n; i++) {
        printseperator(0);
		for (j=n-1; i<j; j--){ // nested (verschachtelt) forloop | starts at the end of the array
			printdoublearray(something, n, i, n - i, j-1, j);	// sonification what happens here?
			
			if (Order(&something[j-1], &something[j]))
                printdoublearray(something, n, i, n - i, j-1, j);
            
            if(SortingMan::Instance()->ShouldStop())
                return;
		} 
	}
	
}
