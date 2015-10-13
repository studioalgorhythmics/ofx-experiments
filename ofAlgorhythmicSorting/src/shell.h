/**************************************************************
 *           Sorting an array with the Shell method            *
 * ----------------------------------------------------------- *
 * REFERENCE:                                                  *
 *                                                             *
 *   "NUMERICAL RECIPES By W.H. Press, B.P. Flannery,          *
 *    S.A. Teukolsky and W.T. Vetterling, Cambridge            *
 *    University Press, 1986" [BIBLI 08].                      *
 *                                                             *
 *                                 C++ version by J-P Moreau   *
 * ----------------------------------------------------------- *
 
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sorthelp.h"
#include "SortingMan.h"





/****************************************************
 * Sorts an array ARR of length N in ascending order *
 *            by the Shell-Mezgar method             *
 * ------------------------------------------------- *
 * INPUTS:                                           *
 *	   N	  size of table ARR                 *
 *          ARR	  table to be sorted                *
 * OUTPUT:                                           *
 *	   ARR   table sorted in ascending order    *
 *                                                   *
 * NOTE: The Shell method is a N^3/2 routine and can *
 *       be used for relatively large arrays.        *
 ****************************************************/
void SHELL(int n, double *ARR)  {
    // Label: e10
    double* realArray=ARR;
    //SORTING stupid but this algorithm starts counding at 1, not zero, so this is a hack.
    ARR--;
    
#define   ALN2I  1.0/0.69314718
#define   TINY   1e-5
    
    double LOGNB2, t;
    int   i,j,k,l,m,nn;
    
    LOGNB2=(double) floor(log((double)n)*ALN2I+TINY);
    m=n;
    for (nn=1; nn<=floor(LOGNB2); nn++) {
        printseperator(0);
        m=m / 2;
        k=n-m;
        for (j=1; j<k+1; j++) {
            i=j;
        e10:  l=i+m;
            //if(j!=1)
            //      printseperator(1);
            printdoublearray(realArray, n,i-1,1,i-1,l-1);
            if (ARR[l] < ARR[i]) {
                t=ARR[i];
                ARR[i]=ARR[l];
                ARR[l]=t;
                if(SortingMan::Instance()->ShouldStop())
                    return;
                
                printdoublearray(realArray, n,i-1,1,i-1,l-1);
                i=i-m;
                if (i >= 1) goto e10;
            }
        }
    }
}