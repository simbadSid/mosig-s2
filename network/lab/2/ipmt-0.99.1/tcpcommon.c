/***********************************************************
 *  
 * $Id: tcpcommon.c 61 2011-10-24 10:02:17Z rousseau $
 *
 * Created by Martin Heusse on 19/11/07.
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

#include <sys/time.h>

#include "config.h"
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#ifndef HAVE_GETADDRINFO
#include "getaddrinfo.h"
#endif
#include "tcpcommon.h"
#include "ipmt.h"


struct sockaddr *sockin_addr;


int
ipmt_connect(char * hostname, char * portname, char * srcportname, int bufsz)
{
    int i, s;

    struct addrinfo hints,
		    *rai = NULL;
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

    hints.ai_flags          = 0;
    hints.ai_socktype       = SOCK_STREAM;
    hints.ai_protocol       = IPPROTO_TCP;
    hints.ai_addrlen        = 0;
    hints.ai_addr           = NULL;
    hints.ai_canonname      = NULL;
    hints.ai_next           = NULL;

    if ( (i = getaddrinfo(hostname, portname, &hints, &rai)) ) {
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
	fprintf(stderr,"IPv6 protocol\n");
    }
    else if (rai->ai_family == AF_INET) {
	fprintf(stderr,"IPv4 protocol\n");
    }
    else {
	fprintf(stderr,"Unknown protocol family");
    }
#else
    fprintf(stderr,"only IPv4 protocol available\n");
#endif

    if ((s = socket(rai->ai_family, rai->ai_socktype, rai->ai_protocol)) < 0)
    {
	perror("Socket");
	exit(1);
    }



    if(*srcportname != 0)
    {
        bind_src_port(s,srcportname, rai);
    }


    if (bufsz != 0) {
	if (setsockopt(s, SOL_SOCKET, SO_SNDBUF,
		(char *) &bufsz, sizeof(bufsz)) < 0) {
	    perror("setsockopt");
	    exit(2);
	}
    }

    /*
     * Connect to the remote socket
     */
    if (connect(s, rai->ai_addr, rai->ai_addrlen) < 0) {
	perror("connect");
	close(s);
	exit(2);
    }
    return s;
}

int
ipmt_listen(char * portname, int bufsz)
{
    int sa,
	i;
    unsigned int  rlen;
    int on = 1;

    char lportname[NI_MAXSERV];
    struct addrinfo hints,*res=NULL;

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

    hints.ai_flags          = AI_PASSIVE;
    hints.ai_socktype       = SOCK_STREAM;
    hints.ai_protocol       = IPPROTO_TCP;
    hints.ai_addrlen        = 0;
    hints.ai_addr           = NULL;
    hints.ai_canonname      = NULL;
    hints.ai_next           = NULL;
	
    if (getaddrinfo(NULL, portname, &hints, &res) != 0 ) {
    	perror("Getaddrinfo");
	exit(1);
    }

#ifdef IPv6
    //  look for IPv6
    while (res->ai_family != AF_INET6 && res->ai_next != NULL) {
	res = res->ai_next;
    }
    if (res->ai_family == AF_INET6) {
	fprintf(stderr,"IPv6%s protocol\n", force_ipv6? "" : " (and IPv4)");
    }
    else if (res->ai_family == AF_INET) {
	fprintf(stderr,"IPv4 protocol\n");
    }
    else {
	fprintf(stderr,"Unknown protocol family\n");
    }
#else
    fprintf(stderr,"only IPv4 protocol available\n");
#endif
    sockin_addr = res->ai_addr;
    rlen = res->ai_addrlen;

    if ((sa = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
	perror("Socket");
	exit(1);
    }
#ifdef IPv6
#ifdef IPV6_BINDV6ONLY
    if (res->ai_family == AF_INET6 && !force_ipv6) {
	int only = 0;
	if (setsockopt(sa, IPPROTO_IPV6, IPV6_BINDV6ONLY,
		&only, sizeof only) < 0) {
	    perror("setsockopt not IPv6 only");
	    exit(1);
	}
    }
#endif
#endif

    // Allow to immediately relaunch server on same port
	if (setsockopt(sa, SOL_SOCKET, SO_REUSEADDR,
		&on, sizeof(int)) < 0) {
	    perror("setsockopt");
	    exit(2);
	}


    if (bind(sa, sockin_addr, rlen)< 0) {
	perror("Bind");
	exit(1);
    }

    if ((getsockname(sa, sockin_addr, &rlen)) < 0) {
	perror("Getsockname");
	exit(1);
    }

    if ((i = getnameinfo(sockin_addr, rlen, NULL, 0,
		    lportname, sizeof(lportname),  NI_NUMERICSERV)) < 0) {
	fprintf(stderr,"getnameinfo: %s\n", gai_strerror(i));
	exit(2);
    }

    fprintf(stderr, "Using port %s\n", lportname);
	
    if (bufsz != 0) {
	setsockopt(sa, SOL_SOCKET, SO_RCVBUF,
		   (char *) &bufsz, sizeof(bufsz));
    }
	
    if (listen(sa, 0) < 0) {
	perror("Listen");
	exit(1);
    }
    return sa;
}


