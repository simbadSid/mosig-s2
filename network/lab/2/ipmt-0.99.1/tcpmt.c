/****************************************************************************
 *  
 * $Id: tcpmt.c 62 2011-10-28 21:59:40Z rousseau $
 *
 * created by Gilles Berger Sabbatel
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
 ***************************************************************************/

#include "ipmt.h"
#include "tcpcommon.h"
#include "testfile.h"
#include <netinet/tcp.h> // for TCP_NODELAY
#include <sys/select.h>

//variables for rate limited test option
FILE*   test_filehandle=0;
struct itimerval itv_tcp;
sigset_t oset_tcp;
long long ll_tcp,krate;
int ius_tcp=0;
long long rate_tcp=0;
int timer_interval_tcp;

// Buffer for data reception (-t option)
char sink_buf[64*1024];

struct sigaction sigact;
char host[NI_MAXHOST],
serv[NI_MAXSERV];

char    portname[64] = "13000";	// port number -- has to be long enough to fit most services in /etc/services :-)
char    srcportname[64] = "";	
int bufsz = 0;
int flag_alarm_set_tcpmt=0;

fd_set rset,wset;
int maxsock;
int child_pid;

int      tos = 0; // type of service, declared here because of send_one_file_tf callback

//Fonction qui actie l'option tcp no-delay
void enable_tcp_no_delay()
{
	int flag = 1;
	int result = setsockopt(s,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int));
	if (result< 0) 
	{
  		perror("Couldn't setsockopt(TCP_NODELAY)\n");
  		exit( EXIT_FAILURE );
	}

}
//Function to activate TCP NOPUSH
void enable_tcp_no_push()
{
	int flag = 1;
	int result = +1;
#ifdef TCP_CORK
	 result= setsockopt(s,IPPROTO_TCP,TCP_CORK,(char *)&flag,sizeof(int));
#elif defined TCP_NOPUSH
	 result= setsockopt(s,IPPROTO_TCP,TCP_NOPUSH,(char *)&flag,sizeof(int));
#endif
	if (result< 0) 
	{
  		perror("Couldn't setsockopt(TCP_NOPUSH | TCP_CORK)\n");
  		exit( EXIT_FAILURE );
	}
    else if (result >0){
        perror("Please note : I could not set either NOPUSH (BSD) or TCP_CORK (LINUX)\n\
        You may see small packets if the write size does not match the MSS\n");
    }
}


void send_data()
{
   	if (write(s, buf, pkt_sz) < 0) 
	{
        perror("write");
        terminate();
	 }
	smooth_win[c].nb_pkt++;
	nb_sent++;
	gettimeofday(&current_t, NULL);
	display_stats(); 
}

void send_one_file_tf(){
    int frk;
    
    frk = fork();
    if(frk>0) { //I'm the parent process -- do nothing
        nb_sent += b_burstsz;
    }
    else if(frk==0){// Child: send the data
        nb_sent = 0;

        s = ipmt_connect(hostname, portname, srcportname, bufsz);
        set_tos(s, tos);
        gettimeofday(&start_t, NULL);
        while (nb_sent < b_burstsz){ 
            if (write(s, buf, pkt_sz) < 0) {
                perror("write");
                terminate();
            }
            nb_sent++;
        }
        terminate();
//         close(s);
//         fflush(stdout);
//         _exit(0);
    }
    else {
	    perror("Fork");
	    exit(1);
	}
    
}

void (*send_one_file_ptr)()=NULL;


void
finish()
{
    printf(" **** Parent process stats: (block size may be wrong) ***\n");
    terminate();
}


/*
 * Compute time to the next burst, either for regular transmission, or 
 * if reading a test file
 */
int time_to_next_burst_tcp(){
    if (! test_filehandle) {
        return ius_tcp;
    }
    else {
        return 1000*prepare_next_burst(0);
    }
}

// Function called when the alarm rings: 
// Used for -r --> calls send_data()
//          -f --> calls send_one_file_tf()
void alarm_catcher()
{
    struct timeval time;
        
    gettimeofday(&time, NULL);
    //  envoi de tous les bursts dont l'échéance est atteinte
    while (posterieur(time, next_event))
    {
        (*send_one_file_ptr)();
        add_time(&next_event, &next_event, time_to_next_burst_tcp());
    }
    if (flag_alarm_set_tcpmt == 0) {
        flag_alarm_set_tcpmt = 1;
        sigact.sa_handler = alarm_catcher;
        if (sigaction(SIGALRM, &sigact, NULL) < 0) {
            perror("Sigaction");
            exit(1);
        }
    }
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
           "    -s pktsz : write chunk size in bytes (default: 1460 ; 1 std MSS; although generally ts option gets in the way)\n"
           "    -c       : Chop transmissions: PUSH the packets out at each write to the socket (default behavior of most applications) \n"
           "    -y       : tcp_nodelaY: de-activate Nagle algorithm \n"
           "    -S port  : source port number\n"
           "    -r rate  : rate in Kbit/s (K = 1000)\n"
           "    -n datasz: number of bytes to send (default transmit until stopped)\n"
           "    -d secs  : test duration in seconds (default transmit until stopped)\n"
           "    -N pknb  : total number of packets to send (default: transmit until stopped)\n"
           "    -b bufsz : TCP send buffer size\n"
           "    -P       : passive - Connection establishment will come from tcptarget -A; host not used\n"
           "    -T tos   : type of service (deprecated: RFC 1349, obsoleted by RFC 2474)\n"
           "    -D dscp  : diff serv code point (RFC 2474)\n"
           "    -v       : display version number and exit\n"
           "    -4       : force IPv4 protocol\n"
           "    -6       : force IPv6 protocol\n"
           "    -t       : target - Start a traffic sink for bidirectionnal testing\n"
           "    -f file  : read a test file\n",
           execname);
    exit(1);
}


void read_data(int fildes){
    ssize_t bytes_received;
    
    while ((bytes_received = recv(fildes, &sink_buf, sizeof(sink_buf), 0)) < 0) {
        if (errno == EAGAIN || errno == EINTR)  continue;
        perror("Recv");
        exit(1);
    }
    if(bytes_received==0){
        exit(0);   
    }
}


void
finish_t()
{   
    if (child_pid) {
        if(kill(child_pid,SIGTERM)<0){
            perror("kill child process failed?\n");
            exit(1);
        }
    } else 
        perror("What are we doing here if there is no child process?\n");
    terminate();
}

ssize_t write_and_rec(int fildes, const void *buf, size_t nbyte){
    
    FD_SET(fildes, &rset);
    FD_SET(fildes, &wset);

    while(1){
        select(maxsock,&rset, &wset,0,0);
        if (FD_ISSET(fildes, &rset)) {// first see if there's anything to receive
            read_data(fildes);
        }
        if(FD_ISSET(fildes, &wset)) {
            return write(fildes, buf, nbyte);
        }
        FD_SET(fildes, &wset); // If we reached this point, that means the fd was set in rset!
    }
}

/*
 * MAIN
 */

int
main(argc, argv)
char           *argv[];
{
	timer_interval_tcp= MIN_INTERVAL;
    ll_tcp=0;
    krate=0;
    ius_tcp=0;
    rate_tcp=0;

    int     ch=0,
    i;
    int      flags = 0;
    long int data_sz = -1;
    socklen_t rlen;
    char    chop=0, nonagle=0, passive = 0;
    struct sockaddr_storage sockr;

    pkt_sz = 1460; 
    nb_pkt = -1;
    force_ipv4 = 0;
    force_ipv6 = 0;
    int start_target = 0;
    sender_disp = 1 ;
    rate_tcp=0;
    char            testfilename[128] = "";
    struct tstdsc* atstdsc=0;
    
    send_one_file_ptr=&send_data; // by default, limited rate

    strncpy(execname, basename(argv[0]), sizeof(execname));
    while ((ch = getopt(argc, argv, "d:p:r:s:S:n:N:PT:D:cyv46b:tf:")) != -1) {
        switch (ch) {
            case 'd':		//  test duration
                duration = opt_to_num(optarg,"duration");
                if (duration < 0) {
                    fprintf(stderr, "Duration should be >= 0\n");
                    usage();
                }
                break;
            case 'p':		// port number
                strncpy(portname, optarg, sizeof(portname));
                break;
            case 'r':           // bitrate
                rate_tcp = opt_to_num(optarg,"rate");
                if (rate_tcp < 0) {
                    fprintf(stderr, "Rate should be > 0\n");
                    exit(2);
                }
                break;
            case 'S':		// port number
                strncpy(srcportname, optarg, sizeof(portname));
                break;
            case 's':		// write buffer size
                pkt_sz = opt_to_num(optarg, "packet size");
                if (pkt_sz < 0 || pkt_sz > MAX_OCTETS) {
                    printf("%d: limit is %d\n", pkt_sz, MAX_OCTETS);
                    usage();
                }
                break;
            case 'N':		// number of blocks
                nb_pkt = opt_to_num(optarg,"number of packets");
                if (nb_pkt <= 0)
                {printf("invalid number of packets\n"); usage();}
                break;
            case 'n':		// number of bytes
                data_sz = opt_to_num(optarg, "data size");
                if (data_sz <= 0)
                {printf("Invalid size\n"); usage();}
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
            case 'v':		// version number
                fprintf(stderr,"%s version %s\n", execname, VERSION);
                exit(0);
            case 'b':		// Buffer size
                bufsz = opt_to_num(optarg, "buffer size");
                break;
            case 'P':		// TCP connection initiated by tcptarget
                passive = 1;
                break;
            case 'c':		// TCP NOPUSH
                chop = 1;
                break;
            case 'y':		// TCP NOPUSH
                nonagle = 1;
                break;
            case '4':		// force IPv4
                force_ipv4 = 1;
                break;
            case '6':		// force IPv6
                force_ipv6 = 1;
            case 't':		// start target
                start_target = 1;
#ifndef IPv6
                fprintf(stderr, "Invalid -6 option, this system does not support IPv6\n");
                exit(2);
#endif
                break;
            case 'f':        // test file
                strncpy(testfilename, optarg, sizeof(testfilename));
                send_one_file_ptr=&send_one_file_tf;
                break; 

            default:
                usage();
        }
    }
    if (!passive && optind != argc - 1)
		usage();
    hostname = argv[optind];
    
	
    // Check that incompatible options are not used together
    if (nb_pkt >0 && data_sz > 0) {
        printf("You cannot specify the number of packets to send and the number of bytes to send at the same time !\n");
        usage();
    }
    else if (data_sz > 0) {
        nb_pkt = data_sz / pkt_sz ; 
        data_sz = data_sz % pkt_sz ;
        printf("will send %d full packets and then %ld bytes" , nb_pkt, data_sz);
    }
    
    if (force_ipv4 && force_ipv6) {
        fprintf(stderr,"Cannot specify both IPv6 and IPv4 protocol\n");
        exit(2);
    }
    // debit et intervalle de transmission
    if (rate_tcp) {
        b_burstsz = pkt_sz;
        krate = rate_tcp * K;
        ll_tcp = (long long) b_burstsz*8*1000000;
        if (ius_tcp && ll_tcp/ius_tcp != krate) {
            fprintf(stderr, "Inconsistent specification of Rate, interval and burst size\n");
            exit(2);
        }
        else ius_tcp = ll_tcp/krate;
    }
    else if (ius_tcp ||test_filehandle) {
        rate_tcp = b_burstsz*8;
        if (ius_tcp)
            rate_tcp = rate_tcp*1000000/(ius_tcp*K);
    }
    if (ius_tcp) {
        if(ius_tcp) fprintf(stderr, "Rate = %lld Kbit/s, inter transmission interval = %d microseconds\n",
                        rate_tcp, ius_tcp);
        init_timer(timer_interval_tcp,&itv_tcp,&oset_tcp);
    }   
    // initialisation de sigact
    if (sigemptyset(&(sigact.sa_mask)) < 0) {
        perror("Sigemptyset");
        exit(1);
    }
    sigact.sa_flags = 0;
    sigact.sa_handler = SIG_IGN;
    
    if (sigaction(SIGPIPE, &sigact, NULL) < 0) {
        perror("Sigaction - SIGPIPE");
        exit(1);
    }
    
    
    sigact.sa_handler = terminate;
    if (sigaction(SIGINT, &sigact, NULL) < 0) {
        perror("Sigaction - SIGINT");
        exit(1);
    }


    gettimeofday(&start_t, NULL);

    
    /* reads the test file and starts from there... */
    if(*testfilename){
        struct itimerval itv;
        
        if ((test_filehandle = fopen(testfilename, "r")) == NULL) {
    	    perror(testfilename);
    	    exit(1);
        }
        
        // We do not wait for the exit status of the child process (that would create zombies!)
        sigact.sa_flags = SA_NOCLDWAIT;
        sigact.sa_handler = SIG_IGN;
        if (sigaction(SIGCHLD, &sigact, NULL) < 0) {
            perror("Sigaction - SIGCHLD");
            exit(1);
        }


        atstdsc = calloc(MAXNUMDESC, sizeof(struct tstdsc));
        init_timer(MIN_INTERVAL,&itv,&oset_tcp);
        read_test_file(test_filehandle, atstdsc);
        exec_test_file(atstdsc, &oset_tcp, 0);
        printf("Never reached\n"); exit(1);
    }
    
    
    // Connect to hostname/portname, if not passive tcpmt
    if (!passive)
        s = ipmt_connect(hostname, portname, srcportname, bufsz);
    else {
        sa = ipmt_listen (portname,bufsz);
        rlen = sizeof(sockr);
        if ((s = accept (sa, (struct sockaddr *)&sockr, &rlen)) < 0) {
            perror("accept");
            exit(2);
        }
        flags = NI_NUMERICHOST | NI_NUMERICSERV ;
        if ((i = getnameinfo((struct sockaddr *)&sockr, rlen, host, sizeof(host),
                             portname, sizeof(portname),  flags)) < 0) {
            fprintf(stderr,"getnameinfo: %s\n", gai_strerror(i));
            exit(2);
        }
        hostname = host;
        printf("Accepted host %s\n", host); 
    }
    
    set_tos(s, tos);

    if(start_target&&ius_tcp){// forks a receiver if paced sending option
        
        printf("starting target\n");
        child_pid = fork();
        if(child_pid==0){// The child receives the data            
            while(1){
                read_data(s);
            }
            printf("Never reached\n");exit(1);
        }
        sigact.sa_handler = finish_t; 
        if (sigaction(SIGTERM, &sigact, NULL) < 0) {// so a simple TERM terminates the child...
            perror("Sigaction - SIGTERM");
            exit(1);
        }

    }

    printf
	("\nTime         Packets    Total	   |	Kbit/s	Avg 10	Avg\n");
    
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
    
    nb_sent = 0;
    
    // Adjusts sock opts depending on platform
    if (!chop) 
        enable_tcp_no_push();
    if (!nonagle)
        enable_tcp_no_delay();

    
    /*
     * main loop of the test
     */
        
   if (ius_tcp) {                 // test temporisé
        gettimeofday(&current_t, NULL);
        add_time(&next_event, &current_t, ius_tcp);
        // still protected by the sig mask set at the return from sigsuspend in calibrate_timer
        if (setitimer(ITIMER_REAL, &itv_tcp, 0) < 0) {
            perror("setitimer"); 
            exit(1);
        }
       alarm_catcher();
       while (TRUE) {
            // SIGALARM unmasked
            sigsuspend(&oset_tcp);
            // back to critical section here, SIGALARM masked
        }
    }
	else
    {
        ssize_t (*_write_and_rec)(); //function pointer to either send only, or receive as well
                
        // Adjust sending (+receiving) function
        if(!start_target)
            _write_and_rec = write; // We are juste sending data
        else{
            _write_and_rec = write_and_rec; // we want to receive, too!
            FD_ZERO(&rset);FD_ZERO(&wset); // A little bit of preparation
            maxsock=getdtablesize(); 
        }
		while (nb_pkt < 0 || nb_sent < nb_pkt) {
		    if ((*_write_and_rec)(s, buf, pkt_sz) < 0) {
		        perror("write");
		        terminate();
		        break;
		    }
		    
		    smooth_win[c].nb_pkt++;
		    nb_sent++;
		    gettimeofday(&current_t, NULL);
		    display_stats();
		}
	
        // If a size in bytes was entered, send the remainder 
        if (data_sz > 0) {
            if ((*_write_and_rec)(s, buf, data_sz) < 0) {
                perror("write");
                terminate();
            }
            
            smooth_win[c].nb_pkt++;
            nb_sent++;
            
            gettimeofday(&current_t, NULL);
            
            display_stats();
        }
    }
    
    terminate();
    exit(0);		//  not reached - just to avoid warning
}
