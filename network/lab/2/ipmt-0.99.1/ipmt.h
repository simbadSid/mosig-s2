/**************************************************************************
 *  
 * $Id: ipmt.h 61 2011-10-24 10:02:17Z rousseau $
 *
 * Created by Gilles Berger Sabbatel
 *
 * global definitions for the ipmt package
 *
 * Copyright LIG Laboratory (2002) Gilles Berger Sabbatel
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
 **************************************************************************/

#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#endif
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#ifndef HAVE_GETADDRINFO
#include "getaddrinfo.h"
#endif
#ifdef HAVE_LINUX_WIRELESS_H
#include <net/if.h>
#include <linux/types.h>
#include <linux/wireless.h>
#endif
#include <stdlib.h>
#include <libgen.h>

#define bcopy(src,dest,len)     (memmove(dest, src, len))

#define K			1000
#define SIZEBUF		(64*1024)	//  buffer size
#define MAGIC_SIZE	16		//  size of magic packet
#define	MAX_OCTETS	(64*1024)	//  maximum packet size
#define SMOOTH_NB	10		//  smooth window size
#define DEFAULT_PKT_SZ_4  1472
#define DEFAULT_PKT_SZ_6  1452
#define DEFAULT_RATE_INTERVAL   20      //  interval for rate control (ms)

//  Default interval between interrupts. 
// This does not seem to be limited by the kernel HZ ??? gnah ?
#define MIN_INTERVAL 50
#define NBCAL   200


#ifndef TRUE
#define TRUE   	1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#define TEST_TCP 0
#define TEST_UDP 1

#define Kbits(nbpkt)	(((8*(double)(pkt_sz*nbpkt)))/K)

char    *hostname;	// Host destination
char	buf[SIZEBUF];
char    interface_name[256];
char    execname[256];

unsigned  nb_sent;	// nb of sent pkts

struct timeval  start_t,	/* heure au debut du test */
                stop_t,		/* heure de fin du test */
                end_t,		/* heure a la fin du test */
                current_t,	/* temps intermediaire */
                display_t,	/* heure d'affichage des stats. */
				next_event;         // echeance du prochain envoi
				
struct timeval	rate_control_t;      // heure du dernier controle de la vitesse

int posterieur(struct timeval t1, struct timeval t2);

char sender_disp; // Decide if stats should be displayed on sender side


int 	    duration,		 	// test duration
            s,		    	// id socket
            bitrate,        	// wireless bitrate
	    	total10,    	// pour le lissage
            rate_interval,
            new_bitrate;
unsigned int    pkt_sz,     	// packet size in bytes
        		b_burstsz ;      // bursts size (bytes)

// Structure utilisee pour "lisser" les mesures 
struct {
    int    nb_pkt;
    struct timeval  time;
} smooth_win[SMOOTH_NB];

int			c;			// cycle courant (lissage)
int         nb_pkt;		// Nb of packets to send ( -1 : infinite)

#ifdef AF_INET6
#define IPv6
#endif
#define FIRST_IN_BURST 	1
#define LAST_IN_BURST	2
#define FIRST_IN_BATCH 	4
#define LAST_IN_BATCH	8

int sa ; // passive socket   
int force_ipv4,
    force_ipv6;

struct pkt_hdr {
    uint32_t seqnum;
    uint32_t ts_pkt_sec;
    uint32_t ts_pkt_usec;
    char flags;
} *pkt_hdr;

struct sigaction sigact;

void terminate();

double kbits(unsigned int nbpkt);
void finish();
void display_stats();
void add_time(struct timeval *sum,struct timeval *t1, int delta);

double t_elapse();

void bind_src_port(int s,char* srcportname, struct addrinfo *pai);
void set_tos(int s, int tos);
int opt_to_num(char *string, char *msg);

/* Everything related to timers  */
void init_timer(int timer_interval, struct itimerval * p_itv,sigset_t * p_oset);
void
calibrate_timer(struct itimerval itv,sigset_t * p_oset);

void alarm_catcher();
