 /***************************************************************************
 *
 * $Id: target.c 39 2010-11-29 10:21:25Z heusse $
 *
 * This file is part of the ipmt package
 *
 * common functions for udptarget and tcptarget
 * Created by Gilles Berger Sabbatel
 *
 * Copyright LIG Laboratory (2002)
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

#include "target.h"

void terminate();

struct gp_buf {
    struct gp_buf *next;
    double throughput;
} *gp_buf;

struct gp_data {
    int nb;
    struct gp_buf *first;
} gp_data;

/*
 * Open pipe to gnuplot if specified
 */

void
open_gnuplot_pipe(gnuplot_flag)
	int gnuplot_flag;
{
    struct gp_buf *pgpb;

    if (gnuplot_flag) {
	if ((out_file = popen(GNUPLOT_CMD, "w")) == NULL) {
	    fprintf(stderr,"Cannot fork gnuplot\n");
	    exit(2);
	}
	fprintf(stderr,"Gnuplot forked\n");
	fprintf(out_file, "set xtics\n");
	fprintf(out_file, "set ytics\n");
	fprintf(out_file, "set style data linespoints\n");
	fprintf(out_file, "set grid\n");
	fprintf(out_file, "set terminal x11\n");

	//  create sample buffer and chain it circularly
	if ((gp_buf = malloc(sizeof(struct gp_buf) * GP_SAMPLES)) == NULL) {
	    fprintf(stderr, "Cannot allocate memory for gnuplot buffers\n");
	    exit(2);
	}
	gp_data.nb = 0;
	xcounter = 0;
	gp_data.first = gp_buf;
	for (pgpb = gp_buf; pgpb < gp_buf+GP_SAMPLES-1; pgpb++) {
	    pgpb->next = pgpb+1;
	}
	pgpb->next=gp_buf;
    }
    max_rate = 0;
}

/*
 *  display plot
 */

void
gplot(throughput)
    double throughput;
{
    struct gp_buf *pgpb;
    int i;

    if (gp_data.nb < GP_SAMPLES) {	// first GP_SAMPLES items
	pgpb = gp_buf+gp_data.nb;
	pgpb->throughput = throughput;
	gp_data.nb++;
    }
    else {				// next items
	pgpb = gp_data.first;
	pgpb->throughput = throughput;
	gp_data.first = pgpb->next;
    }
    xcounter++;
    fprintf(out_file, "set yrange [0:%f]\n", max_rate*1.07);
    fprintf(out_file, "set xrange [%d:%d]\n",xcounter-GP_SAMPLES, xcounter+1);
    fprintf(out_file, "plot \"-\" notitle\n");
    pgpb = gp_data.first;
    for (i = 1; i <= gp_data.nb; i++) {
	fprintf(out_file, "%d %g\n", xcounter-gp_data.nb+i, pgpb->throughput);
	pgpb = pgpb->next;
    }
    fprintf(out_file, "e\n");
    fflush(out_file);
}

/*
 *  Open output file, and check if it already exists
 */

void
open_output_file(out_flag,filename)
    char *filename;
{
    int ch;

    if(out_flag){
        // check if file exists
        if ((out_file = fopen(filename, "r")) != NULL) {
    	    fprintf(stderr, "File %s already exists, overwite?  (n/y) ", filename);
    	    ch = getchar();
    	    fclose(out_file);
    	    if (ch != 'y' && ch != 'Y') {
    	        exit(0);
    	    }
        }
    
        // open output file
        if ((out_file = fopen(filename, "w")) == NULL) {
    	    perror(filename);
    	    exit(1);
        }
    }
    else{
	out_file=stdout;
    }
}

/*
 *  periodicaly report statistics
 */

void
do_report()
{
    int         sec,
                usec,
                tss,
                tsms;
    int		rcvd_bytes = 0;
    double      delay,
                rdv,
		adv,
		rmaxd, rmind,
		throughput;
    char	msg[256];

    (void) gettimeofday(&tvr, (struct timezone *) NULL);
    sigact.sa_handler = SIG_IGN;
    if (sigaction(SIGALRM, &sigact, NULL) < 0) {
	perror("Sigaction");
	exit(1);
    }
    rcvd_bytes = total_rcvd_bytes-previous_total_rcvd_bytes;
    previous_total_rcvd_bytes = total_rcvd_bytes;
    if (rcvd_bytes != 0 || previous_rcvd != 0) {
        // compute actual delay in ms
	sec = tvr.tv_sec - previous_tvr.tv_sec;
	usec = tvr.tv_usec - previous_tvr.tv_usec;
	delay = sec * 1000 + usec / 1000;

        // compute timestamp

	tss = tvr.tv_sec-tss0;
	tsms = (tvr.tv_usec-tsus0) / 1000;
	if (tsms < 0) {
	    tsms += 1000;
	    tss -= 1;
	}

        // compute throughput in bit/ms == kb/s
	throughput = (double) rcvd_bytes *8 / delay;

	if (gnuplot_flag) {
	    if (throughput > max_rate) {
		max_rate = throughput;
	    }
	    gplot(throughput);
	}
	else if (throughput != 0 || previous_throughput != 0) {
	    sprintf(msg, "%5d.%03d %9.1f", tss, tsms, throughput);
	    if (display_dv) {
		adv = abs_max_delay-abs_min_delay;
                adv = adv/1000;
		if (adv < 0) adv = 0;
		rdv = rel_max_delay-rel_min_delay;
                rdv = rdv/1000;
		if (rdv < 0) rdv = 0;
		rmaxd = rel_max_delay;
                rmaxd = rmaxd/1000;
                rmind = rel_min_delay;
                rmind = rmind/1000;
		sprintf(msg+strlen(msg), "  %6.3f  %6.3f  %6.3f  %6.3f",
			adv, rdv, rmind, rmaxd);
		rel_max_delay = -100000000;
		rel_min_delay = 100000000;
	    }
	    if (speed_estimate && nb_speed > 0) {
		float s;
		s = sum_speed/(nb_speed*1000);
		sprintf(msg+strlen(msg), " %9.1f",s);
		s = sum_dt/(nb_speed*1000);
		sprintf(msg+strlen(msg), " %6.3f",s);
		sum_speed = 0;
		sum_dt = 0;
		nb_speed = 0;
	    }
	    if (out_flag) {
		if (! display_times)
		    fprintf(out_file, "%s\n", msg);
		fprintf(stderr, "%s\r", msg);
	    }
	    else {
		printf("%s\n", msg);
		fflush(stdout);
	    }
	    time_idle = 0;
	}
	previous_throughput = throughput;
    }
    else if (gnuplot_flag && max_rate != 0) {
	gplot(0);
    }

    else time_idle++;
    previous_rcvd = rcvd_bytes;
    previous_tvr = tvr;
    if (time_idle >= max_idle)
    {
	fprintf(stderr, "Maximum idle time elapsed\n");
	terminate();
    }

    sigact.sa_handler = do_report;
    if (sigaction(SIGALRM, &sigact, NULL) < 0) {
	perror("Sigaction");
	exit(1);
    }
}
