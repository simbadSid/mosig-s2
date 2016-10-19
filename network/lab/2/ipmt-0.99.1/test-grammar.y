/***************************************************************************
 *
 *  $Id: test-grammar.y 39 2010-11-29 10:21:25Z heusse $
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

%{
#include <math.h>
#include <stdio.h> 
#include <stdlib.h>
int yylex (void);
void yyerror (char const *);

#include "testfile.h"


struct tstdsc *atstdsc;
int curdsc=0, retseq=-1;

void printtest(struct tstdsc);

extern FILE* yyin;

void update_next(){
    // we just saw that the previous desc had a successor
    if(atstdsc[curdsc-1].last!=1) 
        atstdsc[curdsc-1].nextseq=curdsc;
}


%}

%union {
long int    a_val;
float       val_delay;
}

%type <val_delay>  delay


%token INTEGER
%token FLOAT

%%
line:   seq '\n' 
        | line seq '\n'

seq:    test                {
                            }
        | seq {update_next(1);}
          test              {    
                            }
        | rep               {
                            }
        | seq {update_next(1);}
          rep               {
                            }

rep:    '{' repdsc seq '}'  {   
                                //make the repdsc node point to the first node after the loop
                                if (retseq!=-1) atstdsc[retseq].nextseq=curdsc;
                                // retrieve the earlier return address
                                retseq=atstdsc[retseq].retseq;
                                 // mark the last desc of a repetition
                                 //we just got out of a seq: look at last dsc -- if it's a "test", and it's not marked "last", then mark last. If already marked, mark the enclosing repetition descriptor
                                 int parent_dsc = curdsc-1;
                                 while(atstdsc[parent_dsc].last==1){
                                    parent_dsc = atstdsc[parent_dsc].retseq;
                                 }
                                 atstdsc[parent_dsc].last=1;
                                 atstdsc[parent_dsc].nextseq=-1;
                            }
                                 
repdsc: delay INTEGER            {    // that's a repetition descriptor
                                atstdsc[curdsc].delay= $1 ;
                                atstdsc[curdsc].nbr = $<a_val>2 ; 
                                atstdsc[curdsc].sz= -1 ; // marks a repetition
                                // Memorize the current return address
                                atstdsc[curdsc].retseq= retseq ;
                                atstdsc[curdsc].subseq=curdsc+1; // we know this one will have a successor
                                retseq=curdsc; // copied on all offsprings
                                curdsc++; // we just added one node
                            }
                                

test:   '[' delay INTEGER INTEGER ']' {
                                atstdsc[curdsc].delay= $2 ;
                                atstdsc[curdsc].nbr = $<a_val>3 ; 
                                atstdsc[curdsc].sz= $<a_val>4;
                                //Duplication of retseq
                                atstdsc[curdsc].retseq = retseq;
                                curdsc++; // we just added one node
                            }


delay:  FLOAT               {
                            }
        | INTEGER           {
                                $$ = (float) $<a_val>1 ;
                            }

                            
%%

void printtest(struct tstdsc tstdsc){
    printf("test: nbr: %ld delay: %f sz: %ld nxt: %d sub:%d ret:%d lst:%d\t", tstdsc.nbr, tstdsc.delay, tstdsc.sz, tstdsc.nextseq, tstdsc.subseq, tstdsc.retseq, tstdsc.last);
}


void read_test_file(FILE * test_file ,struct  tstdsc *tstdsc)
{

    int i;

    yyin=test_file;
    atstdsc = tstdsc;
     
    yyparse();
    for (i=0;i<curdsc; i++){
        printf("%d\t", i);printtest(atstdsc[i]);
        if(atstdsc[i].nextseq==0 && atstdsc[i].subseq==0 && atstdsc[i].retseq==-1)
            printf("\n");
        printf("\n");
    }
}

void yyerror (char const *s){
    fprintf (stderr, "%s\n", s);
}
