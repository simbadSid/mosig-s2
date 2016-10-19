/***************************************************************************
 *
 *  $Id: udpmt.c 61 2011-10-24 10:02:17Z rousseau $
 *  
 *  Created byGilles Berger Sabbatel
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

#include "ipmt.h"
#include "testfile.h"
#include <time.h>


int     p_burstsz = 0,      // bursts size (Nb of paquets)
        burst_remain = 0,   // reliquat burst 
        timer_interval,
        force_ipv4 = 0,
        force_ipv6 = 0,
        nobufs = 0,
        wireless = 0,       // affichage des parametres wireless
        ius = 0,            // intervalle entre émissions
        gpause = 0,            // intervalles entre "batch"
        prev_nb_pkt = 1;

FILE*   test_file=0;


int        flag_alarm_set=0;

long long rate = 0;           // bitrate

struct addrinfo hints,
        *rai = NULL;


//  Send a magical packet, then terminate...

void
finish()
{
    memset(buf, 0xFF, MAGIC_SIZE);
    while (sendto(s, buf, MAGIC_SIZE, 0, rai->ai_addr, rai->ai_addrlen) < 0) {
    if (errno == ENOBUFS) {
        nobufs++;
        continue;
    }
    perror("Sendto");
    }
    fprintf(stderr,"Nobufs = %d\n", nobufs);
    nb_sent--;
    terminate();
}

/*
 * Display usage message
 */

void
usage()
{
    printf("Usage: %s [options] host\n"
            "Options :\n"
            "    -p port  : destination port number (default: 13000)\n"
            "    -s pktsz : packet size in bytes (default: 1472)\n"
            "    -S port  : source port number\n"
            "    -d secs  : test duration in seconds (default transmit until stopped)\n"
            "    -n pknb  : total number of packets to send (default: transmit until stopped)\n"
            "    -i us    : interval between transmissions, in microseconds (default: 0)\n"
            "    -B burst : burst size, in packets (default 1)\n"
            "    -b burst : burst size, in bytes (default 1 packet)\n"
            "    -t int   : timer interval in microseconds (default 500)\n"
            "    -r rate  : rate in Kbit/s (K = 1000)\n"
            "    -T tos   : type of service (deprecated: RFC 1349, obsoleted by RFC 2474)\n"
            "    -D dscp  : diff serv code point (RFC 2474)\n"
#ifdef HAVE_LINUX_WIRELESS_H
            "    -w int   : display wireless conditions every int ms\n"
#endif
            "    -v       : display version number and exits\n"
            "    -4       : force IPv4 protocol\n"
            "    -6       : force IPv6 protocol\n"
            "    -c       : allow sending broadCasts\n"
            "    -f file  : reads traffic description from a file\n"
            "    -V       : be Verbose\n"
            "    -P msecs : instead of stopping after sending -n pknb, wait for this duration \n",
        execname);
    exit(1);
}

void
maketimestamp()
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    pkt_hdr->ts_pkt_sec = htonl(tv.tv_sec);
    pkt_hdr->ts_pkt_usec = htonl(tv.tv_usec);
}

/*
 *  Send one burst
 */

void
send_one_burst()
{
    int i;

    if (nb_pkt > 0 && nb_sent >= nb_pkt + prev_nb_pkt) 
        {
            if(gpause > 0){
                add_time(&next_event, &next_event, gpause*1000);
                prev_nb_pkt = nb_sent;
            }
            else
                finish();
        }
    for (i = burst_remain; i < b_burstsz; i += pkt_sz) {
    pkt_hdr->flags = 0;
        pkt_hdr->seqnum = htonl(nb_sent);
    
    if (i == burst_remain) {    // 1st packet
        pkt_hdr->flags |= FIRST_IN_BURST;
    }
    if (i+pkt_sz >= b_burstsz) {    // last packet
        pkt_hdr->flags |= LAST_IN_BURST;
    }
        if(nb_sent == prev_nb_pkt){
            pkt_hdr->flags |=  LAST_IN_BATCH;
        }
        if(nb_sent == prev_nb_pkt + 1 ){
            pkt_hdr->flags |= FIRST_IN_BATCH;
        }


    maketimestamp();
        while (sendto(s, buf, pkt_sz, 0, rai->ai_addr, rai->ai_addrlen) < 0) {
        if (errno == ENOBUFS) {
        nobufs++;
        maketimestamp();
        continue;
        }
            perror("Sendto");
            terminate();
        }
        nb_sent++;
        smooth_win[c].nb_pkt++;
    }
    burst_remain = i - b_burstsz;
    gettimeofday(&current_t, NULL);
    
#ifdef HAVE_LINUX_WIRELESS_H
    if (wireless) wireless_stats();
#endif
    display_stats();
}



/*
 * Compute time to the next burst, either for regular transmission, or 
 * if reading a test file
 */
int time_to_next_burst(){
    if (! test_file) {
        return ius;
    }
    else {
        return 1000*prepare_next_burst(1);
    }

}

/*
 *  traitement des signaux SIGALRM
 */

void
alarm_catcher()
{
    struct timeval time;
        
    gettimeofday(&time, NULL);
    //  envoi de tous les bursts dont l'échéance est atteinte
    while (posterieur(time, next_event))
    {
        send_one_burst();
        add_time(&next_event, &next_event, time_to_next_burst());
    }
    if (flag_alarm_set == 0) {
        flag_alarm_set = 1;
        sigact.sa_handler = alarm_catcher;
        if (sigaction(SIGALRM, &sigact, NULL) < 0) {
            perror("Sigaction");
            exit(1);
        }
    }
}


/*
 * MAIN
 */

int
main(int argc,char **argv)
{
    char   portname[64] = "13000";// port number -- has to be long enough to fit most services in /etc/services :-)
    char   srcportname[64] = "" ;
    char   testfilename[128] = "";
    int   tos = 0;        // type of service

    int i,
        default_pkt_sz,
        ch,
        bcast = 0;
        
    long long ll, krate;

    struct tstdsc* atstdsc=0;
    struct itimerval itv;
    sigset_t oset;

    timer_interval = MIN_INTERVAL;
    duration = 0;
    pkt_sz = 0;
    nb_pkt = -1;
    sender_disp = 0 ;

    strncpy(execname, basename(argv[0]), sizeof(execname));
    while ((ch = getopt(argc, argv, "p:s:n:i:B:b:d:t:r:w:T:D:v46hcVP:S:f:")) != -1) {
        switch (ch) {
        case 'B':           // bursts size in packets
            p_burstsz = opt_to_num(optarg,"burst size");
            if (p_burstsz <= 0) {
                fprintf(stderr, "Size of bursts should be > 0\n");
                exit(2);
            }
            break;
        case 'b':           // bursts size in bytes
            b_burstsz = opt_to_num(optarg,"burst size");
            if (b_burstsz <= 0) {
                fprintf(stderr, "Size of bursts should be > 0\n");
                exit(2);
            }
            break;
    case 'd':        //  test duration
        duration = opt_to_num(optarg,"duration");
        if (duration < 0) {
        fprintf(stderr, "Duration should be >= 0\n");
        exit(2);
        }
        break;
    case 'P':        //  gpauses duration
        gpause = opt_to_num(optarg,"Pause duration");
        if (duration < 0) {
        fprintf(stderr, "Pause duration should be >= 0\n");
        exit(2);
        }
        break;
        case 'i':           // interval
            ius = opt_to_num(optarg,"interval");
            if (ius < 0) {
                fprintf(stderr, "Interval should be >= 0\n");
                exit(2);
            }
            break;
        case 'n':        // number of blocks
            nb_pkt = opt_to_num(optarg,"Number of blocks");
            if (nb_pkt == 0) {
                printf("Number of blocks to send should be > 0\n");
               exit(2);
        }
            break;
        case 'p':        // port number
        strncpy(portname, optarg, sizeof(portname));
            break;
        case 'r':           // bitrate
            rate = opt_to_num(optarg,"rate");
            if (rate < 0) {
                fprintf(stderr, "Rate should be > 0\n");
                exit(2);
            }
            break;
        case 's':        // blocks size
            pkt_sz = opt_to_num(optarg,"block size");
            if (pkt_sz < 0 || pkt_sz > MAX_OCTETS) {
                printf("%d: limit is %d\n", pkt_sz, MAX_OCTETS);
                exit(2);
        }
            break; 
        case 't':           // timer interval
            timer_interval = opt_to_num(optarg,"timer interval");
            if (timer_interval < 0 || timer_interval > 1000000) {
                fprintf(stderr, "Timer interval should be between 0 and 1000000\n");
                exit(2);
            }
            break;
        case 'T':         // type of service
            if (tos) {
                fprintf(stderr, "Please use TOS or DSCP, not both\n");
                exit(2);
            }
            i = opt_to_num(optarg,"TOS");
            if (i < 0 || i >= 256 ) {
                fprintf(stderr, "TOS should be between 0 and 255\n");
                exit(2);
            }
#if 0
#ifndef IPTOS_TOS
#define IPTOS_TOS(tos) ((tos) & 0x1E)
#endif
#ifndef IPTOS_MINCOST
#define IPTOS_MINCOST 0x02
#endif
            switch (IPTOS_TOS(i)) {
                case IPTOS_LOWDELAY:
                case IPTOS_THROUGHPUT:
                case IPTOS_RELIABILITY:
                case IPTOS_MINCOST:
                    break;
                default:
                    fprintf(stderr, "Invalid tos value\n");
                    exit(2);
            }
#endif
            tos = i;
            break;
        case 'D':         // diff serv code point
            if (tos) {
                fprintf(stderr, "Please use TOS or DSCP, not both\n");
                exit(2);
            }
            i = opt_to_num(optarg,"DSCP");
            if (i < 0 || i >= 64 ) {
                fprintf(stderr, "DSCP should be between 0 and 63 (EF = 46)\n");
                exit(2);
            }
            tos = i << 2;
            break;
        case 'S':        // port number
            strncpy(srcportname, optarg, sizeof(portname));
            break;
    case 'v':        // version number
        fprintf(stderr,"%s version %s\n", execname, VERSION);
        exit(1);
        case 'w':           // wireless parameters
#ifdef HAVE_LINUX_WIRELESS_H
            wireless = 1;
            rate_interval = opt_to_num(optarg, "rate interval");
            break;
#else
        fprintf(stderr, "w option unavalaible under this system\n");
        exit(2);
#endif
    case '4':        // force IPv4
        force_ipv4 = 1;
        break;
    case '6':        // force IPv6
        force_ipv6 = 1;
#ifndef IPv6
        fprintf(stderr, "Invalid -6 option, this system does not support IPv6\n");
        exit(2);
#endif
        break;
    case 'c':
        bcast = 1;
        break;
    case 'V':
        sender_disp = 1;
        break;
    case 'f':        // test file
        strncpy(testfilename, optarg, sizeof(testfilename));
        break; 

    default:
            usage();
        }
    }
    if (optind != argc - 1)
    usage();
    hostname = argv[optind];
    
    if (force_ipv4 && force_ipv6) {
    fprintf(stderr,"Cannot specify both IPv6 and IPv4 protocol\n");
    exit(2);
    }

    hints.ai_flags          = 0;
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

    if ((i = getaddrinfo(hostname, portname, &hints, &rai))) {
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
	default_pkt_sz = DEFAULT_PKT_SZ_6;
	fprintf(stderr,"IPv6, packet size defaults to %d\n", DEFAULT_PKT_SZ_6);
    }
    else if (rai->ai_family == AF_INET) {
	default_pkt_sz = DEFAULT_PKT_SZ_4;
	fprintf(stderr,"IPv4, packet size defaults to %d\n", DEFAULT_PKT_SZ_4);
    }
    else {
	fprintf(stderr,"Unknown protocol family %d, packet size defaults to %d\n",
		rai->ai_family, DEFAULT_PKT_SZ_4);
	default_pkt_sz = DEFAULT_PKT_SZ_4;
    }
#else
    default_pkt_sz = DEFAULT_PKT_SZ_4;
    fprintf(stderr,"IPv4, packet size defaults to %d\n", DEFAULT_PKT_SZ_4);
#endif

    //  verifier la coherence des arguments
    
    //  tailles de paquets et de blocs
    if (b_burstsz) {
        if (p_burstsz) {
            if (pkt_sz && (b_burstsz != p_burstsz * pkt_sz)) {
                fprintf(stderr,"Inconsistent specifications of burst and packet zize\n");
                exit(2);
            }
            pkt_sz = b_burstsz/p_burstsz;
        }
	if (pkt_sz == 0)
	    pkt_sz = default_pkt_sz;
	p_burstsz = b_burstsz/pkt_sz;           //  just in case
    }
    else {
	if (pkt_sz == 0)
	    pkt_sz = default_pkt_sz;
	if (p_burstsz)
	    b_burstsz = p_burstsz*pkt_sz;
        else {
            p_burstsz = 1;
            b_burstsz = pkt_sz;
        }
    }
    
    if(*testfilename){
        if ((test_file = fopen(testfilename, "r")) == NULL) {
    	    perror(testfilename);
    	    exit(1);
        }
        atstdsc = calloc(MAXNUMDESC, sizeof(struct tstdsc));
    }
    
    fprintf(stderr, "Packet size = %d bytes, burst size = %d bytes\n", pkt_sz, b_burstsz);
    
    // debit et intervalle de transmission
    if (rate) {
	krate = rate * K;
	ll = (long long) b_burstsz*8*1000000;
	if (ius && ll/ius != krate) {
	    fprintf(stderr, "Inconsistent specification of Rate, interval and burst size\n");
	    exit(2);
	}
	else ius = ll/krate;
    }
    else if (ius) {
	rate = b_burstsz*8;
	rate = rate*1000000/(ius*K);
    }
    if (ius || test_file) {
	if(ius) fprintf(stderr, "Rate = %lld Kbit/s, inter transmission interval = %d microseconds\n",
		rate, ius);
	init_timer(timer_interval,&itv,&oset);
    }

    if ((s = socket(rai->ai_family, rai->ai_socktype, rai->ai_protocol)) < 0)
    {
        perror("Socket");
        exit(1);
    }
    
    if(*srcportname != 0)
    {
        bind_src_port(s,srcportname,rai);
    }

    set_tos(s, tos);

#ifdef HAVE_LINUX_WIRELESS_H
    if (wireless) {
        if (select_wireless_interface(s) <= 0)
            fprintf(stderr, "No wireless interface\n");
        else
            fprintf(stderr,"Wireless interface : %s\n", interface_name);
        bitrate = 0;
    }
#endif

    if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &(bcast), sizeof(int))<0){
        perror("Setsockopt Broadcast");
        exit(1);
    }

    // initialisation de sigact
    if (sigemptyset(&(sigact.sa_mask)) < 0) {
	perror("Sigemptyset");
	exit(1);
    }
    sigact.sa_flags = 0;

    sigact.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sigact, NULL) < 0) {
	perror("Sigaction");
	exit(1);
    }
    
    pkt_hdr = (struct pkt_hdr *)buf;
    pkt_hdr->seqnum = 0;

    if(sender_disp)
        printf ("\nTime         Packets    Total       |    Kbit/s    Avg 10    Avg\n");
   
    sigact.sa_handler = finish;
    if (sigaction(SIGINT, &sigact, NULL) < 0) {
	perror("Sigaction");
	exit(1);
    }
    if (sigaction(SIGTERM, &sigact, NULL) < 0) {
	perror("Sigaction");
	exit(1);
    }

    gettimeofday(&start_t, NULL);
    display_t = start_t;
    display_t.tv_sec++;
    stop_t = start_t;
    stop_t.tv_sec += duration;

    c = 0;
    total10 = 0;
    for (i = 0; i < SMOOTH_NB; i++) {
	smooth_win[i].nb_pkt = 0;
	smooth_win[i].time = start_t;
    }

    nb_sent = 1;

    rate_control_t = start_t;

    //Fixme


    /*
     * main loop of the test
     */
     
    if (ius) {                  // test temporisé
        gettimeofday(&current_t, NULL);
        add_time(&next_event, &current_t, ius);
    // still protected by the sig mask set at the return from sigsuspend in calibrate_timer
        if (setitimer(ITIMER_REAL, &itv, 0) < 0) {
            perror("setitimer"); exit(1);
        }
        alarm_catcher();
        while (TRUE) {
            // SIGALARM unmasked
            sigsuspend(&oset);
            // back to critical section here, SIGALARM masked
        }
    }
    
    else if(test_file){
        read_test_file(test_file, atstdsc);
        exec_test_file(atstdsc, &oset , TEST_UDP);
    }
    
    else while (TRUE) {         // continuous test
        send_one_burst();
    }
    finish();
    return 0; //never reached
}
