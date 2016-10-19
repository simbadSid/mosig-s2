

bin_PROGRAMS = udpmt udptarget tcpmt tcptarget
udpmt_SOURCES = udpmt.c common.c tcpcommon.c ipmt.h testfile.c testfile.h test-grammar.y test-parser.l
udptarget_SOURCES = udptarget.c target.c target.h ipmt.h
tcpmt_SOURCES = tcpmt.c common.c tcpcommon.c ipmt.h tcpcommon.h testfile.c testfile.h test-grammar.y test-parser.l
tcptarget_SOURCES = tcptarget.c target.c common.c tcpcommon.c target.h ipmt.h tcpcommon.h
dist_man1_MANS = udpmt.1 udptarget.1 tcpmt.1 tcptarget.1 
dist_doc_DATA = test-file.txt Licence_CeCILL_V2-en.txt Licence_CeCILL_V2-fr.txt display.R LISEZ_MOI 

BUILT_SOURCES = test-grammar.h

udpmt_LDADD = @LIBOBJS@
udptarget_LDADD = @LIBOBJS@
tcpmt_LDADD = @LIBOBJS@
tcptarget_LDADD = @LIBOBJS@

AM_CFLAGS = -Wall
AM_YFLAGS = -d 

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = Tests/test-tcp.sh Tests/test-udp.sh Tests/test-tcp-passive.sh Tests/common.sh \
    Tests/test-tcp-tf.sh Tests/test-tcp-bidir.sh 

TESTS_ENVIRONMENT = TESTDIR=${srcdir}/Tests PATH=$(builddir):${PATH}
TESTS = Tests/test-tcp.sh Tests/test-udp.sh Tests/test-tcp-passive.sh Tests/test-tcp-tf.sh Tests/test-tcp-bidir.sh

