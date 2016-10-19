/***************************************************************************
 *
 * $Id: tcptarget.c 61 2011-10-24 10:02:17Z rousseau $
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

#include "target.h"
//Needed for fork()
#include <sys/types.h>
#include <unistd.h>
#include "tcpcommon.h"

#define MAX_IDLE	3600

int     interval = 1000;
char    persist = 0;

struct timeval tv; // to record the test duration

void	
terminate_rcv()
{
    struct timeval  now;

    gettimeofday(&now, (struct timezone *) NULL);
    printf("%llu bytes received in %.4f seconds, bye!\n", total_rcvd_bytes, t_elapse(tv, now) );
//     printf("terminate\n");
    close(sr);
}

void
terminate_p()
{
	terminate_rcv();
	exit(0);
}

void
terminate_all()
{
    close(sa); // Note that exit() takes care of this...
    printf("terminate main process\n");
    if (out_flag) {
	fclose(out_file);
    }

    if (!persist) // Else each thread is reponsible for closing the socket
	terminate_rcv();
    exit(0);
}

/*
 *  display usage message
 */

void
usage()
{
    fprintf(stderr, "Usage: %s [options]\n"
	           "Options:\n"
		   "    -i ms       : display interval (in ms)\n"
	           "    -o filename : write the result to filename (default: stdout)\n"
	           "    -p port     : port number (default: 13000)\n"
	           "    -b bufsize  : set receive buffer size (default: system default)\n"
		   "    -g          : pipe the result to gnuplot\n"
	           "    -P          : make the server persistent (suppresses reporting)\n"
	           "    -A host     : connection to host initiated by target (see -P on tcpmt)\n"
   		   "    -t c        : timestamp display format:\n"
		   "                  c = e : relative to the epoch\n"
		   "                  c = d : relative to the current day\n"
		   "                  c = 0 : relative to the beginning of the program (default)\n"
		   "    -4          : force IPv4 protocol\n"
		   "    -6          : force IPv6 protocol\n"
	           "    -v          : display version number and exit\n"
		   "    -h          : display this message\n",
	    execname);
    exit(1);
}

int
main(argc, argv)
    int argc;
    char *argv[];
{
    struct timeval	rcv_time;
    struct sockaddr sockr;
    int ch;
    char    	*filename=NULL;	//  output file name
    char tsfmt = '0';
    int bufsz = 0; 
    unsigned int rlen;
    ssize_t bytes_received;
    pid_t frk; 
    char portname[64] = "13000";	// port number
    char servername[128]="";
	
    previous_rcvd = 0;
    total_rcvd_bytes = 0;
    previous_throughput = 0;
    previous_total_rcvd_bytes = 0;
    force_ipv4 = 0;
    force_ipv6 = 0;
    out_flag = 0;
    time_idle=0;
    tss0 = -1;
    tsus0 = 0;
    speed_estimate = 0;
    strncpy(execname, basename(argv[0]), sizeof(execname));

    display_dv = 0;
    max_idle = MAX_IDLE;

    while ((ch = getopt(argc, argv, "gi:o:p:b:t:A:vP46")) != -1) {
	switch (ch) {
	    case 'b':		// Buffer size
		bufsz = opt_to_num(optarg, "buffer size");
		break;
	    case 'g':		// pipe to gnuplot
		gnuplot_flag = 1;
		break;
	    case 'i':		// display interval
		interval=opt_to_num(optarg, "interval");
		printf("Reporting every %d ms\n",interval);
		break;
	    case 'o':		// output file name
		filename = optarg;
		out_flag = 1;
		break;
	    case 'p':		// port number
	    strncpy(portname, optarg, sizeof(portname));
		break;
	    case 'A':		// active mode
	    strncpy(servername, optarg, sizeof(servername));
		break;
	    case 't':		// time display format
		if (strlen(optarg) != 1) {
		    usage();
		}
		tsfmt = optarg[0];
		if (index("ed0",tsfmt) == NULL) {
		    usage();
		}
		break;
	    case 'P':		// makes the target persistent, will accept multiple connections
		persist = 1 ;
		break;
	    case 'v':		// version number
		fprintf(stderr,"%s version %s\n", execname, VERSION);
		exit(1);
	    case '4':		// force IPv4
		force_ipv4 = 1;
		break;
	    case '6':		// force IPv6
		force_ipv6 = 1;
#ifndef IPv6
		fprintf(stderr, "Invalid -6 option, this system does not support IPv6\n");
		exit(2);
#endif
	    break;
	    default:
		usage();
	}
    }
    if (optind != argc)
	usage();

    // If incompatible options, resolve conflict
     if (gnuplot_flag) {
	if (out_flag) {
	    fprintf(stderr,"Cannot specify both -g & -o options\n");
	    exit(2);
	}
    }


    if (force_ipv4 && force_ipv6) {
	fprintf(stderr,"Cannot specify both IPv6 and IPv4 protocol\n");
	exit(2);
    }
     //  open output file, if specified

    open_output_file(out_flag,filename);

    //  open pipe to gnuplot if specified
    
    open_gnuplot_pipe(gnuplot_flag);

    // setup the socket for reception
	
    if (strlen(servername)) {
	persist=0; // one never knows :-]
	sr = ipmt_connect(servername, portname, "", bufsz);
    }
    else {
	sa = ipmt_listen(portname, bufsz);
    	rlen = sizeof(sockr);
	sr = accept(sa, (struct sockaddr *) &sockr, &rlen);
    }

    // setup the timer
    gettimeofday(&tv, (struct timezone *) NULL);
	
    if (sr < 0) {
	perror("Accept");
	exit(1);
    }
	
    // initialisation de sigact
    if (sigemptyset(&(sigact.sa_mask)) < 0) {
	perror("Sigemptyset");
	exit(1);
    }
    sigact.sa_flags = 0;
    sigact.sa_handler = terminate_all;

    if (sigaction(SIGPIPE, &sigact, NULL) < 0) {
	perror("Sigaction - SIGPIPE");
	exit(1);
    }
    if (sigaction(SIGINT, &sigact, NULL) < 0) {
	perror("Sigaction -SIGINT");
	exit(1);
    }
    if (sigaction(SIGTERM, &sigact, NULL) < 0) {
	perror("Sigaction - SIGTERM");
	exit(1);
    }

    sigact.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &sigact, NULL) < 0) {
	perror("Sigaction - SIGTERM");
	exit(1);
    }

    frk=1;
    while (persist && frk) {
	frk = fork();
	if(frk>0) { //I'm the parent process
	    fprintf(stderr,"successful fork\n");
	    close(sr);
	    sr = accept(sa, &sockr, &rlen);
	    gettimeofday(&tv, (struct timezone *) NULL); // Record when connection was received!
	    if (sr < 0) {
		perror("Accept");
		exit(1);
	    }
	}
	else if(frk==0){
			
	    //I'm the child process
	    sigact.sa_handler = terminate_p;

	    if (sigaction(SIGPIPE, &sigact, NULL) < 0) {
		perror("Sigaction - SIGPIPE");
		exit(1);
	    }
	    if (sigaction(SIGINT, &sigact, NULL) < 0) {
		perror("Sigaction -SIGINT");
		exit(1);
	    }
	    if (sigaction(SIGTERM, &sigact, NULL) < 0) {
		perror("Sigaction - SIGTERM");
		exit(1);
	    }
	    continue;
	}
	else {
	    perror("Fork");
	    exit(1);
	}
    }

    //If I reach this point, then I'm a child, and the parent loops above

    itv.it_interval.tv_sec = interval / 1000;
    itv.it_interval.tv_usec = (interval % 1000) * 1000;
    itv.it_value.tv_sec = interval / 1000;
    itv.it_value.tv_usec = (interval % 1000) * 1000;
    if(!persist) {
	setitimer(ITIMER_REAL, &itv, 0);
	sigact.sa_handler = do_report;

	if (sigaction(SIGALRM, &sigact, NULL) < 0) {
	    perror("Sigaction - SIGALRM");
	    exit(1);
	}
    }
    (void) gettimeofday(&previous_tvr, (struct timezone *) NULL);

    // set intitial time

    switch (tsfmt) {
	case 'e':
	    tss0 = 0;
	    break;
	case 'd':
	    tss0 = previous_tvr.tv_sec - previous_tvr.tv_sec%(24*3600);
	    break;
	case '0':
	    tss0 = previous_tvr.tv_sec;
	    tsus0 = previous_tvr.tv_usec;
	    break;
    }

    while (1) {
	while ((bytes_received = recv(sr, &buf, sizeof(buf), 0)) < 0) {
	    if (errno == EAGAIN || errno == EINTR) continue;
	    perror("Recv");
	    exit(1);
	}
	if (tss0 == -1) { // set initial time
	    (void) gettimeofday(&rcv_time, (struct timezone *) NULL);
	    switch (tsfmt) {
	    case 'e':
		tss0 = 0;
		break;
	    case 'd':
		tss0 = rcv_time.tv_sec - rcv_time.tv_sec%(24*3600);
		break;
	    case '0':
		tss0 = rcv_time.tv_sec;
		tsus0 = rcv_time.tv_usec;
	    break;
    	    }
	}
	if (bytes_received == 0) {
	    printf("Connection reset by peer\n");
	    persist ? terminate_p() : terminate_all();
	}
        total_rcvd_bytes += bytes_received;
    }
}
