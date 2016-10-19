/***************************************************************************
 *
 * $Id: udptarget.c 61 2011-10-24 10:02:17Z rousseau $
 *
 * Created by Gilles Berger Sabbatel
 *
 * This file is part of the ipmt package
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
 *
 ****************************************************************************/

#include <time.h>
#include "target.h"

#define MAX_IDLE	3600
#define DELTAMIN	10
#define DELTAMAX	500

int		interval = 1000;
int		deltamin = DELTAMIN;
int		deltamax = DELTAMAX;
int 		nb_histo = 0;
int		h_res = 1;
int		*histo;
int 		min_histo = DELTAMAX;
int 		max_histo = DELTAMIN;
int		display_histo = 0;
int       	application_ts = 0;
int		in_burst = 0;
int		in_batch = 0;
int		keep = 0;
int		quiet = 0;
uint32_t	seq=0,
		newseq=0;
unsigned int 	recpkt=0,
		lost=0,         //  number of lost packets
		nseqlost=0,     //  number of duplicate packet sequences
		maxseqlost=0,   //  length of max lost sequence of paquets
		ndup=0,         //  number of duplicated packets
		seq_err=0;      //  other sequence errors
struct timeval	rcv_time,
		prev_time,
		rcv_ts;

struct addrinfo hints,
		*rai = NULL;
struct sockaddr * sockin_addr;
struct sockaddr_storage peer_addr;

FILE		*out_file_histo;

// get a numeric option
// same as common, but did not want to link everything
int
opt_to_num(char *string, char *msg)
{
    int i;
    char *pc;

    i = strtol(string, &pc, 0);
    if (pc != optarg+strlen(optarg)) {
	fprintf(stderr, "%s should be a numeric value\n", msg);
	exit(2);
    }
    return i;
}

void
terminate()
{
    if (display_histo) {
	int i,j,k,l;
// 	if (out_flag) {
// 	    fprintf(out_file, "HISTO :\n");
// 	}
// 	else {
// 	    printf("HISTO :\n");
// 	}
	l = 0;
	for (i = (min_histo-deltamin)/h_res; i < (max_histo-deltamin)/h_res+1; i++) {
	    double f;
	    j = i*h_res+deltamin;
	    k = histo[i];
	    f = k*100;
	    f = f/nb_histo;
	    fprintf(out_file_histo, "%d\t%3.2f\n",j,f);
	    l = k;
	}
    }
    fprintf(stderr, "%u received packets\n", recpkt);
    fprintf(stderr, "%u duplicated packets\n", ndup);
    fprintf(stderr, "%u lost packets\n", lost);
    fprintf(stderr, "%u lost sequences\n", nseqlost);
    fprintf(stderr, "max length of lost sequence = %u\n", maxseqlost);
    fprintf(stderr, "%u sequence errors\n", seq_err);
    fprintf(stderr, "bye!\n");
    if (out_flag) {
	fclose(out_file);
    }
    close(sr);
    exit(0);
}

/*
 *  display usage message
 */

void
usage()
{
    fprintf(stderr,"Usage: %s [options]\n"
	           "Options:\n"
		   "    -a          : receive timestamp computed by application\n"
		   "    -d          : display delay for every packet\n"
		   "    -e          : display estimation of transmission speed\n"
		   "    -i ms:      : display interval (in ms)\n"
		   "    -j          : display delay variation (in ms)\n"
		   "    -k          : keep running after sender stops\n"
		   "    -b          : estimate bottleneck buffer size (use -n -P -s -r on sender side!)\n"
		   "    -H          : display histogram of intervals\n"
		   "    -m us       : min value of interval (microseconds)\n"
		   "    -M us       : max value of interval (microseconds)\n"
		   "    -r res      : histogram resolution\n"
		   "    -g          : pipe the result to gnuplot\n"
		   "    -o filename : write the result to filename (default: stdout)\n"
		   "    -p port     : port number (default: 13000)\n"
		   "    -q          : be quiet \n"
		   "    -t c        : timestamp display format:\n"
		   "		  c = e : relative to the epoch\n"
		   "		  c = d : relative to the current day\n"
		   "		  c = 0 : relative to the beginning of the program (default)\n"
    		   "    -4          : force IPv4 protocol\n"
    		   "    -6          : force IPv6 protocol\n"
		   "    -v          : display the version number and exit\n"
		   "    -h          : display this message\n",
		   execname);
    exit(1);
}

/*
 *  Main
 */

int
main(argc, argv)
     int	argc;
     char	*argv[];
{
    int         rlen;
    socklen_t 	fromlen;
    int 	ch, i;
    long int	ts;
    long int 	delay;
    char    	*filename=NULL;	//  output file name
    char	tsfmt;		//  format of timestamp
    struct msghdr msg;
    struct cmsghdr *cmsg;
    unsigned char ctlbuf[128];
    struct iovec iov;

    char portname[64] = "13000";
    int on = 1;
    
    int buf_estimate = 0; 
    struct timeval orig_rcv_time = { 0, 0 },
		   orig_rcv_ts = { 0, 0 },
		   prev_pkt_ts = { 0, 0 },
		   prev_pkt_time = { 0, 0 };

    long int timedifts, timedifrx;
    long int buf_bytes_rcvd = 0;

    previous_rcvd = 0;
    total_rcvd_bytes = 0;
    previous_total_rcvd_bytes = 0;
    previous_throughput = 0;
    out_flag = 0;
    gnuplot_flag = 0;
    time_idle=0;
    tsfmt = '0';
    tss0 = -1;
    tsus0 = 0;
    force_ipv4 = 0;
    force_ipv6 = 0;
    speed_estimate = 0;


    fromlen = sizeof(struct sockaddr);
    strncpy(execname, basename(argv[0]), sizeof(execname));

    display_dv = 0,
    display_times = 0,
    rel_min_delay = 100000000;	//  relative minimum delay
    abs_min_delay = 100000000;	//  absolute minimum delay
    abs_max_delay = -100000000;	//  absolute maximum delay
    rel_max_delay = -100000000;	//  relative maximum delay
    max_idle = MAX_IDLE;

    while ((ch = getopt(argc, argv, "abdegHhi:jM:m:r:o:p:t:vk46q")) != -1) {
        switch (ch) {
	    case 'a':		// timestamp computed by application
		application_ts = 1;
		break;
	    case 'b':		// buffer estimate
		buf_estimate = 1;
		printf("buf estimate, use -n -P options on sender\n");
		break;
	    case 'd':		// display delay 
		display_times = 1;
		break;
	    case 'e':		// speed estimate
		speed_estimate = 1;
		sum_speed = 0;
                sum_dt = 0;
		nb_speed = 0;
		break;
	    case 'g':		// pipe to gnuplot
		gnuplot_flag = 1;
		break;
	    case 'H':		// display histogram
		display_histo = 1;
		break;
	    case 'i':		// display interval
		interval=opt_to_num(optarg, "interval");
		fprintf(stderr,"Reporting every %d ms\n",interval);
		break;
	    case 'j':		// display delay variation
		display_dv = 1;
		break;
	    case 'k':		// continue after receiving magic packet
		keep = 1;
		break;
	    case 'M':
		deltamax = opt_to_num(optarg, "delta max");
		min_histo = deltamax;
		break;
	    case 'm':
		deltamin = opt_to_num(optarg, "delta min");
		max_histo = deltamin;
		break;
	    case 'o':		// output file name
		filename = optarg;
		out_flag = 1;
		break;
	    case 'p':		// port number
	        strncpy(portname, optarg, sizeof(portname));
		break;
	    case 'r':		// histo resolution
		h_res = opt_to_num(optarg, "resolution");
		break;
	    case 'v':		// version number
	    	fprintf(stderr,"%s version %s\n", execname, VERSION);
	    	exit(1);
	    case 't':		// time display format
		if (strlen(optarg) != 1) {
		    usage();
		}
		tsfmt = optarg[0];
		if (index("ed0",tsfmt) == NULL) {
		    usage();
		}
		break;
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
	    case 'q':		// force IPv4
		quiet = 1;
		break;
	    default:
		usage();
	}
    }
	
    // If incompatible options, resolve conflict
     if (force_ipv4 && force_ipv6) {
	fprintf(stderr,"Cannot specify both IPv6 and IPv4 protocol\n");
	exit(2);
    }
    if (display_times && display_dv){
	fprintf(stderr,"incompatible options -i and -d specified, using -d !\n");
	display_dv=0;
    }
    if (gnuplot_flag) {
	if (out_flag) {
	    fprintf(stderr,"Cannot specify both -g & -o options\n");
	    exit(2);
	}
	if (display_histo) {
	    fprintf(stderr,"Cannot specify both -g & -H options\n");
	    exit(2);
	}
    }
	
    if (optind != argc) {
	usage();
    }

#ifndef SO_TIMESTAMP
    if (speed_estimate || display_histo || display_times || display_dv) {
	fprintf(stderr,"No kernel timestamp on this system, timings wil be inaccurate\n");
    }
    application_ts = 1;
#endif

    if (display_histo) {	// initialisation de l'histogramme
	int i;
	i = (deltamax-deltamin+h_res-1)/h_res;
	if ((histo = malloc(i*sizeof(int))) == NULL) {
	fprintf(stderr, "Echec de l'allocation mémoire\n");
	exit(1);
    	}
	memset(histo, 0, i*sizeof(int));
    }

    //  open output file, if specified

    open_output_file(out_flag,filename);
    out_file_histo = out_file;
    if(display_histo && out_flag){
	out_file=stdout;
    }

    //  open pipe to gnuplot if specified
    
    open_gnuplot_pipe(gnuplot_flag);

    // check supported address family

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags          = AI_PASSIVE;

#ifdef IPv6
    if (force_ipv4) {
	hints.ai_family = AF_INET;
    } else if (force_ipv6) {
	hints.ai_family = AF_INET6;
    }
    else {
	hints.ai_family =  AF_UNSPEC;
    }
#else
    hints.ai_family = AF_INET;
#endif
    hints.ai_socktype       = SOCK_DGRAM;
    hints.ai_protocol       = IPPROTO_UDP;
    hints.ai_addrlen        = 0;
    hints.ai_addr           = NULL;
    hints.ai_canonname      = NULL;
    hints.ai_next           = NULL;

    if ((i = getaddrinfo(NULL, portname, &hints, &rai))) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(i));
	if (i == EAI_SYSTEM) {
	    perror("getaddrinfo");
	}
	exit(1);
    }
    
#ifdef IPv6
   
    //  look for IPv6
    while (rai->ai_family != AF_INET6 && rai->ai_next != NULL) {
	rai = rai->ai_next;
    }
    if (rai->ai_family == AF_INET6) {
	fprintf(stderr,"IPv6%s protocol\n", force_ipv6? "" : " (and IPv4)");
    }
    else if (rai->ai_family == AF_INET) {
	fprintf(stderr,"IPv4 protocol\n");
    }
    else {
	fprintf(stderr,"Unknown protocol family\n");
    }
#else
    fprintf(stderr,"IPv4 protocol\n");
#endif
    sockin_addr = rai->ai_addr;
    namelen = rai->ai_addrlen;
    
    // setup the socket for reception

    if ((sr = socket(rai->ai_family, rai->ai_socktype, rai->ai_protocol)) < 0) {
	perror("Socket");
	exit(1);
    }
#ifdef IPv6
#ifdef IPV6_BINDV6ONLY
    if (rai->ai_family == AF_INET6 && !force_ipv6) {
	int only = 0;
	if (setsockopt(sr, IPPROTO_IPV6, IPV6_BINDV6ONLY,
		&only, sizeof only) < 0) {
	    perror("setsockopt not IPv6 only");
	    exit(1);
	}
    }
#endif
#endif
    if ((bind(sr, sockin_addr, namelen)) < 0) {
	perror("Bind");
	exit(1);
    }
    if ((getsockname(sr, sockin_addr, &namelen)) < 0) {
	perror("Getsockname");
	exit(1);
    }

    if ((i = getnameinfo(sockin_addr, namelen, NULL, 0,
		    portname, sizeof(portname),  NI_NUMERICSERV)) < 0) {
	fprintf(stderr,"getnameinfo: %s\n", gai_strerror(i));
	exit(2);
    }

    fprintf(stderr, "Receiving from port %s\n", portname);
    fprintf(stderr, "    Time    Kbit/s");
    if (display_dv) {
	fprintf(stderr, "   abs dv  rel dv   rmind   rmaxd");
    }
    if (speed_estimate) {
	fprintf(stderr, "  Estimated   dt");
    }
    fprintf(stderr, "\n");

    // initialisation de sigact
    if (sigemptyset(&(sigact.sa_mask)) < 0) {
	perror("Sigemptyset");
	exit(1);
    }
    sigact.sa_flags = 0;
    sigact.sa_handler = terminate;

    if (sigaction(SIGPIPE, &sigact, NULL) < 0) {
	perror("Sigaction - SIGPIPE");
	exit(1);
    }
    if (sigaction(SIGINT, &sigact, NULL) < 0) {
	perror("Sigaction - SIGINT");
	exit(1);
    }
    if (sigaction(SIGTERM, &sigact, NULL) < 0) {
	perror("Sigaction - SIGTERM");
	exit(1);
    }

    // setup the timer

    itv.it_interval.tv_sec = interval / 1000;
    itv.it_interval.tv_usec = (interval % 1000) * 1000;
    itv.it_value.tv_sec = interval / 1000;
    itv.it_value.tv_usec = (interval % 1000) * 1000;
    setitimer(ITIMER_REAL, &itv, 0);

    sigact.sa_handler = do_report;
    if (sigaction(SIGALRM, &sigact, NULL) < 0) {
	perror("Sigaction");
	exit(1);
    }
    (void) gettimeofday(&previous_tvr, (struct timezone *) NULL);
    
    pkt_hdr = (struct pkt_hdr *)buf;
    
    seq = 0; // Sender starts at 1
    (void) gettimeofday(&rcv_time, (struct timezone *) NULL);
    
    memset(&msg, 0, sizeof(msg));	// just in case...
    msg.msg_name = &peer_addr;
    msg.msg_namelen = fromlen;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
#ifdef SO_TIMESTAMP
    msg.msg_control = &ctlbuf;
    msg.msg_controllen = sizeof(ctlbuf);
    msg.msg_flags = 0;
    if (setsockopt(sr, SOL_SOCKET, SO_TIMESTAMP, &on, sizeof(on)) != 0) {
	perror("setsockopt");
	exit(2);
    }
#endif

    memset(magic_buf, 0xFF, MAGIC_SIZE);
    while (1) {
        while ((rlen = recvmsg(sr, &msg, 0)) < 0) {
	    if (errno == EAGAIN || errno == EINTR) continue;
	}
	if (rlen == MAGIC_SIZE) {
	    if ( memcmp(buf, magic_buf, MAGIC_SIZE) == 0) {
		if (keep) {
		    seq = 0;
		    continue;
		}
		else {
		    terminate();
		}
	    }
	}
	prev_time = rcv_time;
#ifdef SO_TIMESTAMP
	if (!application_ts) {
	    cmsg = CMSG_FIRSTHDR(&msg);
	    while (cmsg!=NULL ? 
		    (cmsg->cmsg_level != SOL_SOCKET ||
		     cmsg->cmsg_type != SCM_TIMESTAMP) : 0) {
		cmsg = CMSG_NXTHDR(&msg,cmsg);
	    }
	    /* Copy to avoid alignment problems: */
	    if (cmsg!=NULL) {
		memcpy(&rcv_time, CMSG_DATA(cmsg), sizeof(rcv_time));
	    }
	    else {
		fprintf(stderr,"No kernel Timestamp, fallback to application Timestamp\n");
		fprintf(stderr,"Timings will be unaccurate\n");
		application_ts = 1;
	    }
	}
#endif
	if (application_ts) {
	    if (gettimeofday(&rcv_time, (struct timezone *) NULL) < 0) {
		perror("Gettimeofday");
		exit(2);
	    }
	}
	if (tss0 == -1) { // set initial time
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
        if (rlen < 0) {
            fprintf(stderr, "rlen = %d\n", rlen);
            perror("Recvfrom");
            exit(1);
        }
        total_rcvd_bytes += rlen;
        recpkt++;
        newseq = ntohl(pkt_hdr->seqnum); 

	if (rlen < sizeof(struct pkt_hdr)) {
	    if (display_dv) {
		fprintf(stderr,"Packet too small to compute jitter, disabling jitter display\n");
		display_dv = 0;
	    }
	    if (display_times) {
		fprintf(stderr,"Packet too small to compute clock delta, disabling delay display\n");
		display_times = 0;
	    }
	    if (speed_estimate) {
		fprintf(stderr,"Packet too small to estimate speed, disabling speed estimation\n");
		speed_estimate = 0;
	    }
	}
	if (display_dv){
	    //  compute delay variation in µs

	    delay = (rcv_time.tv_sec - ntohl(pkt_hdr->ts_pkt_sec))*1000000 +
	            (rcv_time.tv_usec - ntohl(pkt_hdr->ts_pkt_usec));
	    if (delay < abs_min_delay) {
		abs_min_delay = delay;
	    }
	    if (delay < rel_min_delay) {
		rel_min_delay = delay;
	    }
	    if (delay > abs_max_delay) {
		abs_max_delay = delay;
	    }
	    if (delay > rel_max_delay) {
		rel_max_delay = delay;
	    }
	}
	else if (display_times){
	    // Every time I get a packet, print the clock delta.
	    delay = (rcv_time.tv_sec - ntohl(pkt_hdr->ts_pkt_sec))*1000000 +
	            (rcv_time.tv_usec - ntohl(pkt_hdr->ts_pkt_usec));
	    ts = rcv_time.tv_sec*1000+rcv_time.tv_usec/1000;
			
	    fprintf(out_file, "\%ld\t%ld\n", ts, delay);
	}
		
	if (display_histo) {
	    int i, j, k;
	    i = (rcv_time.tv_sec-prev_time.tv_sec)*1000000;
	    j = (rcv_time.tv_usec-prev_time.tv_usec);
	    k = i+j;
	    if (k >= deltamin && k <= deltamax) {
		histo[(k-deltamin)/h_res]++;
		nb_histo++;
		if (k > max_histo) {
		    max_histo = k;
		}
		if (k < min_histo) {
		    min_histo = k;
		}
	    }
	}

	if (speed_estimate) {
	    if (in_burst && newseq == seq+1) {
		long int i, j, k;
		float s;
		i = (rcv_time.tv_sec-prev_time.tv_sec)*1000000;
		j = (rcv_time.tv_usec-prev_time.tv_usec);
		k = i+j;
		if (k > 0) {
		    s = rlen*8;
		    s = (s*1000000)/k;
		    sum_speed += s;
		    sum_dt += k;
		    nb_speed++;
		}
	    }
	    else 
		if (pkt_hdr->flags&FIRST_IN_BURST) in_burst = 1;
	    if (pkt_hdr->flags&LAST_IN_BURST) in_burst = 0;
        }
	if (buf_estimate) {
        // Thanks to Sears A Merritt for his contribution 
	    float buffer;
	    if (in_batch && newseq == seq+1) {
		printf(".");
		memcpy(&prev_pkt_time, &rcv_time,sizeof(prev_pkt_time));
		prev_pkt_ts.tv_sec =  ntohl(pkt_hdr->ts_pkt_sec);
		prev_pkt_ts.tv_usec =  ntohl(pkt_hdr->ts_pkt_usec);
		buf_bytes_rcvd += rlen;
	    }
	    else if (pkt_hdr->flags&FIRST_IN_BATCH) {
		in_batch = 1;
		//printf(" 1st in batch ");
		// record time of 1st packet (orig_time)
		memcpy(&orig_rcv_time, &rcv_time,sizeof(prev_pkt_time));
		orig_rcv_ts.tv_sec =  ntohl(pkt_hdr->ts_pkt_sec);
		orig_rcv_ts.tv_usec =  ntohl(pkt_hdr->ts_pkt_usec);
		buf_bytes_rcvd = 0;
	    }
	    else if(in_batch && newseq != seq+1){
	    	// a loss just occured, we can start the buffer size calculation now!
		float tx_rate, rx_rate;
//                 timeval_subtract(&timedifrx, &prev_pkt_ts, &orig_rcv_ts);
//                 timeval_subtract(&timedif, &prev_pkt_time, &orig_rcv_time);
		timedifts = (long int)(prev_pkt_ts.tv_sec-orig_rcv_ts.tv_sec ) * 1000000.00 + (long int)(prev_pkt_ts.tv_usec-orig_rcv_ts.tv_usec);
		timedifrx = (long int)(prev_pkt_time.tv_sec-orig_rcv_time.tv_sec ) * 1000000.00 + (long int)(prev_pkt_time.tv_usec-orig_rcv_time.tv_usec); 
		rx_rate = (8 * buf_bytes_rcvd)/(float)timedifrx* 1000000.00;
		tx_rate = (8 * buf_bytes_rcvd)/(float)timedifts* 1000000.00;
		printf("\n td tx: %ld td rx: %ld nb bytes:%ld\n",timedifts/1000, timedifrx/1000,buf_bytes_rcvd );
		buffer = (tx_rate - rx_rate)/8.0 * (timedifts/1000000.00);
		fprintf(out_file, "Buffer Size: %f\n", buffer);
                printf("Tx Speed Estimate: %f\tRx Speed Estimate: %f\n", tx_rate, rx_rate);

		in_batch=0;
	    }
	    if (pkt_hdr->flags&LAST_IN_BATCH) {
		if(in_batch) printf("It seems that you are not sending fast enough to overflow the buffer, dude!\n");
		in_batch = 0;
	    }
        }

    if (newseq != seq+1)
        {       			//  sequence error
            if (newseq > seq+1)         //  lost packet
            {
		nseqlost++;
		lost += newseq-seq-1;
		if (newseq-seq-1 > maxseqlost) maxseqlost = newseq-seq-1;
		seq = newseq;
		in_burst = 0;
            }
            else if (newseq == seq)      //  duplicated packet
		ndup++;
            else if (quiet==0){
		fprintf(stderr, "Sequence error : expected = %d, received = %d\n",
		        seq+1, newseq);
		seq_err++;
            }
        }
    seq = newseq;
    }
}
