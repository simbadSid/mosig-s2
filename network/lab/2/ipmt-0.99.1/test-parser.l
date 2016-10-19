%{
/***************************************************************************
 *
 *  $Id: test-parser.l 39 2010-11-29 10:21:25Z heusse $
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


/* need this for the calls to strtol/f() below */
#include <math.h>
#include <stdio.h> 
int var=0;
#include "test-grammar.h"

%}



DIGIT       [0-9]

FLOAT       {DIGIT}+("."{DIGIT}*)?
INTEGER     -?{DIGIT}+

%%
"\["      return '[';
"\]"      return ']';
"\{"      return '{';
"\}"      return '}';
"\n"      return '\n';
{INTEGER}   { 
                yylval.a_val = (long) strtol( yytext , NULL, 10 ); 
                return INTEGER;
            }
{FLOAT}     { 
                yylval.val_delay = (float) strtof( yytext , NULL); 
                return FLOAT;
            }
.          //this dot catches all other characters: anything can be used as separator! 
%%


int yywrap(){return 1;}
