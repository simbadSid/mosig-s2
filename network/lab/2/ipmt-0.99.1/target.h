/***************************************************************************
 *
 * $Id: target.h 39 2010-11-29 10:21:25Z heusse $
 *
 * Created by Gilles Berger Sabbatel
 *
 * This file is part of the ipmt package
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
 ****************************************************************************/

#include "config.h"
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#ifndef HAVE_GETADDRINFO
#include "getaddrinfo.h"
#endif
#include "ipmt.h"

#define MAGIC_SIZE	16

#define GNUPLOT_CMD	"gnuplot -geometry 600x340 -xrm \"gnuplot*line1Width:3\""
#define GP_SAMPLES	90

socklen_t       namelen;
unsigned long long	total_rcvd_bytes,
		previous_total_rcvd_bytes;

int             sr,		// socket de reception
		tss0, tsus0,
		time_idle,
		max_idle,
		out_flag,
		gnuplot_flag,
		ymax,
		display_dv,
		display_times,
		previous_rcvd,
		speed_estimate,
		nb_speed,
		xcounter,
		rel_min_delay,	//  relative minimum delay
		abs_min_delay,	//  absolute minimum delay
		abs_max_delay,	//  absolute maximum delay
		rel_max_delay;	//  relative maximum delay


double 		previous_throughput,
		max_rate,
		sum_speed, sum_dt;

char            buf[64*1024];
char		magic_buf[MAGIC_SIZE];
char		execname[256];

struct sigaction sigact;
struct itimerval itv;
struct timeval  tvr,
                previous_tvr;
FILE		*out_file;

void open_output_file(int out_flag,char *filename);
void do_report();
void terminate();
void open_gnuplot_pipe(int gnuplot_flag);
