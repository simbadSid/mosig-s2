/***************************************************************************
 *
 *  $Id: testfile.c 54 2011-06-28 16:04:58Z heusse $
 *  
 *  Created by Martin Heusse
 *
 *  This file is part of the ipmt package
 *
 * Copyright LIG Laboratory (2007)
 * 
 * This software is a computer program whose purpose is to provide a set
 * of tools to measure performance of networks at the transport level
 * (TCP/IP and UDP/IP)..
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 *
 ***************************************************************************/

#include "testfile.h"
#include "ipmt.h"

/* Reads a test descripor (loops (aka repetitions or rep) are handled by 
   move_to_next() ad prepare_next_burst()
   split_burst is one for UDP, 0 for TCP */

int read_desc(char split_burst){
    pkt_sz = tstdsc[curdsc].sz;
    
    if(split_burst == TEST_UDP){// This is for UDP: one event/packet
        tstdsc[curdsc].it--;
        b_burstsz = tstdsc[curdsc].sz;
    }
    else { // This is TCP: one event for the whole test
        b_burstsz = tstdsc[curdsc].it ;
        tstdsc[curdsc].it=0;
    }

    return tstdsc[curdsc].delay;

}

void move_to_next(){
    if(tstdsc[curdsc].last !=1){
        // Goes to next test descriptor
        curdsc = tstdsc[curdsc].nextseq;
        if (curdsc==0) {
            finish();
        }
        // initialize iteration counter
        tstdsc[curdsc].it=tstdsc[curdsc].nbr;
//         printf("\n step to # %d ",curdsc);
    }
    else { // reached "last"
//         printf("\nreached last! --");
        // Goes back to enclosing loop
        tstdsc[curdsc].retseq >= 0 ? curdsc = tstdsc[curdsc].retseq : finish();
        // Here we're back to the loop node
//         printf("go back up to # %d \n",curdsc);
        if (tstdsc[curdsc].it>0) tstdsc[curdsc].it--; // does not decrement if <0 (infinite)
        if(tstdsc[curdsc].it==0){// We're done
            // Recursively goes to enclosing loop descriptor
            move_to_next();
        }
        else {//re-enter the loop
           curdsc =  tstdsc[curdsc].subseq;
        }
    }
}

/* split_burst is one for UDP, 0 for TCP*/
int prepare_next_burst(char split_burst)
{
    int delay;
    
    if(tstdsc[curdsc].it >0 && tstdsc[curdsc].sz >0){
        // There is something left to do at this stage (which may only be a test, not loop)
        return read_desc(split_burst);
    }
    else{
        // move to next dsc
        if(tstdsc[curdsc].sz >0){
            move_to_next();
        }
        // If we just reached a loop descriptor (or pure delay node), keep looking
        delay=0;
        while(tstdsc[curdsc].sz <=0){
            if(tstdsc[curdsc].subseq>0){
                // This is a (valid) loop
                int subdsc=tstdsc[curdsc].subseq;
                // Just reached this loop: initialize iteration number
                tstdsc[curdsc].it=tstdsc[curdsc].nbr;
                // We should wait the initial wait (+ wait before the first sub test)
                delay += tstdsc[curdsc].delay;
//                 printf("delay : %d curdsc: %d\n",delay, curdsc);
                curdsc = subdsc;
            }
            else {//==> (tstdsc[curdsc].sz <=0 && tstdsc[curdsc].subseq<=0)
                // nowhere to go
                finish();
            }
        }
        // we reached a valid test descriptor (not a loop)
        tstdsc[curdsc].it=tstdsc[curdsc].nbr;
//         printf("before return delay : %d curdsc: %d\n",delay+tstdsc[curdsc].delay, curdsc);
        return  delay+read_desc(split_burst);
    }
}

/* split_burst is one for UDP, 0 for TCP*/
void exec_test_file(struct tstdsc *atstdsc, sigset_t * p_oset, char split_burst){

    int delay ;
    
    // First, initialize the test descriptor list
    tstdsc=atstdsc; // "Local" pointer to the test descriptor
    curdsc = 0;
    tstdsc[curdsc].it = tstdsc[curdsc].nbr;
    
    // Prepare for first burst
    gettimeofday(&current_t, NULL);
    delay=prepare_next_burst(split_burst);
    add_time(&next_event, &current_t, delay); 



    alarm_catcher(); // This may be a different function for UDP or TCP 
    while (TRUE) {
        // SIGALARM unmasked
        sigsuspend(p_oset);
        // back to critical section here, SIGALARM masked
    }

}
