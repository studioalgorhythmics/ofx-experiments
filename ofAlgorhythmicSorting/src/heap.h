/**************************************************************
 *          Sorting an array with the Heapsort method          *
 * ----------------------------------------------------------- *
 * REFERENCE:                                                  *
 *                                                             *
 *   "NUMERICAL RECIPES By W.H. Press, B.P. Flannery,          *
 *    S.A. Teukolsky and W.T. Vetterling, Cambridge            *
 *    University Press, 1986" [BIBLI 08].                      *
 *                                                             *
 *                                 C++ version by J-P Moreau   *
 * -----------------------------------------------------------
 
 *                                                             *
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sorthelp.h"
#include "SortingMan.h"


/****************************************************
 *  Sorts an array RA of length n in ascending order *
 *                by the Heapsort method             *
 * ------------------------------------------------- *
 * INPUTS:                                           *
 *	    n	  size of table RA                  *
 *	    RA	  table to be sorted                *
 * OUTPUT:                                           *
 *	    RA	  table sorted in ascending order   *
 *                                                   *
 * NOTE: The Heapsort method is a N Log2 N routine,  *
 *       and can be used for very large arrays.      *
 ****************************************************/

// Heap sort is like selection sort, but uses a heap (max-heap)

int heapsort( double * data, int n )    // lenght of array to sort
{
    const int WIDTH= 8;                // Number of childs per knot
    double val;
    int parent, child, w, max, i;
    int m= (n + (WIDTH - 2)) / WIDTH;  // first leaf in the heap
    int count= 0;
    int origchild, orign,origparent;
    int depth ;
    orign= n;
    
    while ( 1 )
    {
        
        if ( m ) {                       // First Part of the Algorithm: Make a max-Heap
            parent= --m;
            
            val= data[parent];             // Value, which goes down the heap
            
        }
        else
            if ( --n ) {                     // Teil 2: eigentliche Sortierung
                val= data[n];                  // zu versickernder Wert vom Heap-Ende
                data[n]= data[0];              // Spitze des Heaps hinter den Heap in
                parent= 0;                     //   den sortierten Bereich verschieben
                
                printdoublearray(data, orign,0,0,0,n);//origparent, origchild-origparent);
            }
            else                             // Heap ist leer; Sortierung beendet
                break;
        
        origparent=parent;
        origchild = parent * WIDTH + 1;
        depth =1;
        while ( (child= parent * WIDTH + 1) < n )  // erstes Kind;
        {                                          // Abbruch am Ende des Heaps
            
            printseperator(depth);
            
            w= n - child < WIDTH ?
            n - child : WIDTH;          // Anzahl der vorhandenen Geschwister
            
            count += w;
            
            for ( max= 0, i= 1; i < w; ++i )   {       // größtes Kind suchen
                printdoublearray(data,orign,child, w, child+max, child+i);
                if ( data[child+i] > data[child+max] )
                {
                    max= i;
                }
            }
            child += max;
            
            
            printdoublearray(data, orign,0,0,child, parent);//origparent, origchild-origparent);
            if ( data[child] <= val )      // kein Kind mehr größer als der
                break;                       //   zu versickernde Wert
            
            data[parent]= data[child];     // größtes Kind nach oben rücken
            
            printdoublearray(data, orign,0,0,parent, child);//origparent, origchild-origparent);
            
            
            if(SortingMan::Instance()->ShouldStop())
                return 1;
            
            parent= child;                 // in der Ebene darunter weitersuchen
            depth++;//for our print/sonification
        }
        
        data[parent]= val;               // versickerten Wert eintragen
        printdoublearray(data, orign,0,0,parent,-1);
        if(parent-origchild>0) {
            printseperator(0);
        }
    }
    
    return count;
}


void HPSORT(int n, double *RA)  {
    // Labels: e10, e20
    int i,ir,j,l;
    double rra;
    
    l=(n/2)+1;
    ir=n;
    //The index L will be decremented from its initial value during the
    //"hiring" (heap creation) phase. Once it reaches 1, the index IR
    //will be decremented from its initial value down to 1 during the
    //"retirement-and-promotion" (heap selection) phase.
e10:
    if (l > 1) {
        l--;
        rra=RA[l];
    }
    else {
        rra=RA[ir];
        RA[ir]=RA[1];
        printdoublearray(RA, n,1,1);
        
        if(SortingMan::Instance()->ShouldStop())
            return;
        
        ir--;
        if (ir==1) {
            RA[1]=rra;
            printdoublearray(RA, n,1,1);
            
            return;
        }
    }
    i=l;
    j=l+l;
e20: 
    if (j <= ir) {
        if (j < ir)  
            if (RA[j] < RA[j+1]) j++;
        if (rra < RA[j]) {
            RA[i]=RA[j];
            printdoublearray(RA, n,i,1);	
            
            if(SortingMan::Instance()->ShouldStop())
                return;         
            
            
            i=j; j=j+j;
        }
        else
            j=ir+1;
        goto e20;
    }
    RA[i]=rra;
    goto e10;
    
}