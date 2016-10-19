/***************************************************************************
 *  
 * $Id: common.c 61 2011-10-24 10:02:17Z rousseau $
 *
 * Common functions for tcpmt and udpmt
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
 ************************************************************************/

#include "ipmt.h"

/*
 *  Convert number of packets into Kilo bits (Kilo = 1000)
 */

double
kbits(nbpkt)
    unsigned int nbpkt;
{
    double r;

    r = nbpkt;
    r = r*pkt_sz*8/K;
    return(r); 
}

/*
 * display a timestamp
 */

void
tsprint()
{
    struct timeval  tv;
    struct timezone tz;
    int             s,
                    ms;

    gettimeofday(&tv, &tz);
    s = tv.tv_sec % (24 * 3600);
    ms = tv.tv_usec / 1000;
    printf("%d.%03d     ", s, ms);
}

#ifdef HAVE_LINUX_WIRELESS_H

// code for wireless statistics : used to work on former versions of
// Linux, but unmaintained for a long time...
// Kept as a template, just in case...

/*
 *  find a wireless interface, for wireless statistics
 */

void
wireless_stats()
{
    int i, tss1, tsms1, tss2, tsms2;    // unused variables according to -Wall
    struct iwreq wrq;
    int r1, r2;		
    i = (current_t.tv_sec-rate_control_t.tv_sec)*1000+
            (current_t.tv_usec-rate_control_t.tv_usec)/1000;
    if (i >= rate_interval) {
	rate_control_t = current_t;
	strcpy(wrq.ifr_name, interface_name);
	if (ioctl(s, SIOCGIWRATE, &wrq) >= 0) { 
	    tss2 = current_t.tv_sec % (24 * 3600);
	    tsms2 = current_t.tv_usec / 1000;
	    tss1 = tss2;
	    tsms1 = tsms2-10;
	    if (tsms1 < 0) {
		tss1--;
		tsms1 += 1000;
	    }
	    new_bitrate = wrq.u.bitrate.value;
	    if (bitrate != new_bitrate) {
		r1 = bitrate/1000;
		r2 = new_bitrate/1000;
		bitrate = new_bitrate;
		printf("%d.%03d, Seq = %d, bitrate = %d\n",
			tss1, tsms1, nb_sent-1, r1);
		printf("%d.%03d, Seq = %d, bitrate = %d\n", 
			tss2, tsms2, nb_sent, r2);
	    }
	}
	else
	    fprintf(stderr, "SIOCGIWRATE failed\n");
    }

}

int select_wireless_interface(int wl_sock)
{
struct iwreq wrq;
char buff[1024];
struct ifconf ifc;
struct ifreq *ifr;
int i;

	ifc.ifc_len = sizeof(buff);
	ifc.ifc_buf = buff;
	if (ioctl(wl_sock, SIOCGIFCONF, &ifc) < 0)
	{
		fprintf(stderr, "SIOCGIFCONF: %s\n", strerror(errno));
		return -1;
	}

	/*
	 * We select the first wireless interface from the list of network
	 * interfaces.
	 *
         * We should check that the interface is actually the interface
	 * used for the transmission, but it is rather uncommon to have 
	 * multiple wireless interfaces, and it hard to implement.
         */
    
	ifr = ifc.ifc_req;
	for (i = ifc.ifc_len/sizeof(struct ifreq); --i >= 0; ifr++)
	{
		strcpy(wrq.ifr_name, ifr->ifr_name);
		if (ioctl(wl_sock, SIOCGIWNAME, &wrq) >= 0)
		{
			strcpy(interface_name, ifr->ifr_name);
			return 1;
		}
	}
	return -1;
}
#endif

/*
 * verifie que la valeur temporelle t1 soit posterieure à 
 * valeur temporelle t2
 */

int 
posterieur(t1, t2)
    struct timeval t1, t2;
{
    if (t1.tv_sec > t2.tv_sec) return(TRUE);
    if (t1.tv_sec == t2.tv_sec) return(t1.tv_usec>t2.tv_usec);
    return(FALSE);
}

/*
 * Fin du traitement (normal ou sur ^C)
 *
 */

void
terminate()
{
    double kbps;

    gettimeofday(&end_t, NULL);

    printf("\n------- %s statistics -------\n", execname);
    printf("%d blocks of %d bytes sent to %s in %.3f seconds.\n",
	   nb_sent, pkt_sz, hostname, t_elapse(start_t, end_t));
    if (nb_sent > 0) {
	kbps = kbits(nb_sent) / t_elapse(start_t, end_t);
	printf("Avg throughput = %.0f kbit/s\n", kbps);
    }

    close(s);
    fflush(stdout);
    exit(0);
}

/*
 *  display statistics
 */

void
display_stats()
{
    int		next_c;	        // cycle suivant (lissage)
    double      kbps1,
		kbps10,
		kbps;
    if(sender_disp==1){
        if (nb_sent == nb_pkt || !posterieur(display_t, current_t) ) {
            kbps1 = kbits(smooth_win[c].nb_pkt) /
                t_elapse(smooth_win[c].time, current_t);
            kbps = kbits(nb_sent) / t_elapse(start_t, current_t);
    
            total10 += smooth_win[c].nb_pkt;
            next_c = (c + 1) % SMOOTH_NB;
            kbps10 = kbits(total10) / t_elapse(smooth_win[next_c].time, current_t);
    
            tsprint();
            printf("%d	%d	   |	%.0f	%.0f	%.0f\n",
            smooth_win[c].nb_pkt, nb_sent, kbps1, kbps10, kbps);
    
            while ( ! posterieur(display_t, current_t) )
                display_t.tv_sec++;
            c = next_c;
            total10 -= smooth_win[c].nb_pkt;
            smooth_win[c].nb_pkt = 0;
            smooth_win[c].time = current_t;
        }
    }
    else {// no display
        if (!posterieur(display_t, current_t) ){
            c = (c+1) % 3;
            switch(c){
                case 0: printf("\b\b\b- -");break; // \b
                case 1: printf("\b\b\b\\ \\"); break;
                case 2: printf("\b\b\b/ /");break;
            }
            fflush(stdout);
            while ( ! posterieur(display_t, current_t) )
                display_t.tv_sec++;
        }
    }
    if (duration && ((current_t.tv_sec > stop_t.tv_sec) ||
	      ((current_t.tv_sec == stop_t.tv_sec) &&
	       (current_t.tv_usec >= stop_t.tv_usec))
	     )) {
	    terminate();
    }
}

// get a numeric option

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

/*
 * Compute elapsed time in seconds from two dates
 */

double
t_elapse(t1, t2)
     struct timeval  t1;
     struct timeval  t2;
{
    t2.tv_sec -= t1.tv_sec;
    t2.tv_usec -= t1.tv_usec;

    if (t2.tv_usec < 0) {
	t2.tv_sec--;
	t2.tv_usec += 1000000;
    }

    return ((double) t2.tv_sec + (double) t2.tv_usec / 1000000.);
}


void bind_src_port(int s,char* srcportname, struct addrinfo *pai)
{
    int i;
    struct addrinfo hints,
		*rai = NULL;
    struct sockaddr * sockin_addr;
    socklen_t       namelen;


    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags          = AI_PASSIVE;
	hints.ai_family         = pai->ai_family;
    hints.ai_socktype       = pai->ai_socktype;
    hints.ai_protocol       = pai->ai_protocol;
    hints.ai_addrlen        = 0;
    hints.ai_addr           = NULL;
    hints.ai_canonname      = NULL;
    hints.ai_next           = NULL;

    if ((i = getaddrinfo(NULL, srcportname, &hints, &rai))) {
	fprintf(stderr, "getaddrinfo in bind_src_port: %s\n", gai_strerror(i));
	if (i == EAI_SYSTEM) {
	    perror("getaddrinfo");
	}
	exit(1);
    }
    
    sockin_addr = rai->ai_addr;
    namelen = rai->ai_addrlen;

    if ((bind(s, sockin_addr, namelen)) < 0) {
        perror("Bind");
        exit(1);
    }

}

// set TOS, DSCP or class
void set_tos(int s, int tos) {
    struct sockaddr sa;
    int res = sizeof(sa);

    if (tos != 0) {
        if ((getsockname(s, &sa, (socklen_t *)&res)) < 0) {
            perror("Getsockname");
            exit(1);
        }
        switch(sa.sa_family) {
            case AF_INET:
                res = setsockopt(s, IPPROTO_IP, IP_TOS, &tos, (socklen_t)sizeof(tos));
                break;
            case AF_INET6:
                res = setsockopt(s, IPPROTO_IPV6, IPV6_TCLASS, &tos, (socklen_t)sizeof(tos));
                break;
            default:
                printf("Warning: TOS/Class ignored for this protocol family (%d)\n", sa.sa_family);
                res = 0;
        }
        if (res != 0) {
            perror("Setsockopt TOS");
            exit(1);
        }
    }
}

/* Everything related to timers */ 

int        intervals_cal[NBCAL];
int        ntics = 0;
struct timeval before;


void
alarm_calibrate()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    intervals_cal[ntics] = (now.tv_sec-before.tv_sec)*1000000 +
                           now.tv_usec-before.tv_usec;
    ntics++;
    before=now;
    if (ntics < NBCAL) {
    sigact.sa_handler = alarm_calibrate;
    if (sigaction(SIGALRM, &sigact, NULL) < 0) {
        perror("Sigaction");
        exit(1);
    }
    }
}


/*
 *  Comparaison de deux entiers
 */

int intcomp(pi,pj)
    int *pi, *pj;
{
    return (*pi-*pj);
}

void init_timer(int  timer_interval, struct itimerval * p_itv,sigset_t * p_oset){
        p_itv->it_interval.tv_sec = 0;
        p_itv->it_interval.tv_usec = timer_interval;
        p_itv->it_value.tv_sec = 0;
        p_itv->it_value.tv_usec = timer_interval;
        calibrate_timer(*p_itv, p_oset);
}

/*
 *  Timer calibration and interrupt masking
 */

void
calibrate_timer(struct itimerval itv,sigset_t * p_oset)
{
    int time=0;
    int i;
    sigset_t nset;

    fprintf(stderr,"Calibrating timer...\n");
    sigact.sa_handler = alarm_calibrate;
    if (sigaction(SIGALRM, &sigact, NULL) < 0) {
        perror("Sigaction");
        exit(1);
    }
    // critical section to protect timer setup until next call to sigsuspend, SIGALARM masked
    sigemptyset(&nset);
    sigaddset(&nset, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &nset, p_oset) < 0) {
        perror("sigaction"); exit(1);
    }
    if (setitimer(ITIMER_REAL, &itv, 0) < 0) {
        perror("setitimer"); exit(1);
    }
    
    gettimeofday(&before, NULL);
    while (ntics < NBCAL) {
    // SIGALARM unmasked
        sigsuspend(p_oset);
    // back to critical section here, SIGALARM masked
    }
    //  sort intervals table
    qsort((void *)intervals_cal, (size_t)NBCAL, (size_t)sizeof(int), intcomp);
    //  intervalle moyen en excluant les 10 plus petits et les 10 plus grands
    for (i = 20; i < NBCAL-20; i++)
        time += intervals_cal[i];
    time = time/(NBCAL-40);
    fprintf(stderr,"Actual timer duration = %d microseconds\n", time);
}

/*
 *  Adds an interval in microseconds to a timeval
 */

void
add_time(sum, t1, delta)
    struct timeval *sum, *t1;
    int delta;
{
    int dsec, dusec;
    
    dsec = delta/1000000;
    dusec = delta%1000000;
    sum->tv_sec = t1->tv_sec+dsec;
    sum->tv_usec = t1->tv_usec+dusec;
    if (sum->tv_usec >= 1000000) {
        sum->tv_usec -= 1000000;
        sum->tv_sec++;
    }
    else if (sum->tv_usec < 0) {
        sum->tv_usec += 1000000;
        sum->tv_sec--;
    }
}
