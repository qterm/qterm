/*******************************************************************************
FILENAME:	qtermtelnet.cpp
REVISION:	2001.8.12 first created.
         
AUTHOR:		smartfish kafa
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/
#include "qtermtelnet.h"
#include "qterm.h"
#include <stdio.h>
#include "qtermsocket.h"
#ifndef _NO_SSH_COMPILED
#include "ssh/qtermsshsocket.h"
#endif
#include <qcstring.h>

#if !defined(Q_OS_BSD4) && !defined(_OS_FREEBSD_) \
	&& !defined(Q_OS_MACX) && !defined(Q_OS_DARWIN)
#include <malloc.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <qsocket.h>

/* hack from wget/http.c */
#define BASE64_LENGTH(len) (4 * (((len) + 2) / 3))
static void
base64_encode (const char *s, char *store, int length);
static char *
basic_authentication_encode (const char *user, const char *passwd,
							     const char *header);
static int
parse_http_status_line (const char *line, const char **reason_phrase_ptr);


const char QTermTelnet::wingate_enter = CTRL('J');

struct fsm_trans QTermTelnet::ttstab[] = {
	/* State	Input		Next State	Action	*/
	/* ------	------		-----------	-------	*/
	{ TSDATA,	TCIAC,		TSIAC,		&QTermTelnet::no_op		},
	{ TSDATA,	TCANY,		TSDATA,		&QTermTelnet::ttputc 		},
	{ TSIAC,	TCIAC,		TSDATA,		&QTermTelnet::ttputc		},
	{ TSIAC,	TCSB,		TSSUBNEG,	&QTermTelnet::no_op		},
/* Telnet Commands */
	{ TSIAC,	TCNOP,		TSDATA,		&QTermTelnet::no_op		},
	{ TSIAC,	TCDM,		TSDATA,		&QTermTelnet::tcdm		},
/* Option Negotiation */
	{ TSIAC,	TCWILL,		TSWOPT,		&QTermTelnet::recopt		},
	{ TSIAC,	TCWONT,		TSWOPT,		&QTermTelnet::recopt		},
	{ TSIAC,	TCDO,		TSDOPT,		&QTermTelnet::recopt		},
	{ TSIAC,	TCDONT,		TSDOPT,		&QTermTelnet::recopt		},
	{ TSIAC,	TCANY,		TSDATA,		&QTermTelnet::no_op		},
/* Option Subnegotion */
	{ TSSUBNEG,	TCIAC,		TSSUBIAC,	&QTermTelnet::no_op		},
	{ TSSUBNEG,	TCANY,		TSSUBNEG,	&QTermTelnet::subopt		},
	{ TSSUBIAC,	TCSE,		TSDATA,		&QTermTelnet::subend		},
	{ TSSUBIAC,	TCANY,		TSSUBNEG,	&QTermTelnet::subopt		},

	{ TSWOPT,	TOECHO,		TSDATA,		&QTermTelnet::do_echo		},
	{ TSWOPT,	TONOGA,		TSDATA,		&QTermTelnet::do_noga		},
	{ TSWOPT,	TOTXBINARY,	TSDATA,		&QTermTelnet::do_txbinary	},
	{ TSWOPT,	TCANY,		TSDATA,		&QTermTelnet::do_notsup	},

//	{ TSDOPT,	TONAWS,		TSDATA,		&QTermTelnet::will_naws	},
	{ TSDOPT,	TOTERMTYPE,	TSDATA,		&QTermTelnet::will_termtype	},
	{ TSDOPT,	TOTXBINARY,	TSDATA,		&QTermTelnet::will_txbinary	},
	{ TSDOPT,	TCANY,		TSDATA,		&QTermTelnet::will_notsup	},

	{ FSINVALID,	TCANY,		FSINVALID,	&QTermTelnet::tnabort		},
};


struct fsm_trans QTermTelnet::substab[] = {
        /* State        Input           Next State      Action  */
        /* ------       ------          -----------     ------- */
	{ SS_START,	TOTERMTYPE,	SS_TERMTYPE,	&QTermTelnet::no_op		},
	{ SS_START,	TCANY,		SS_END,		&QTermTelnet::no_op		},

	{ SS_TERMTYPE,	TT_SEND,	SS_END,		&QTermTelnet::subtermtype	},
	{ SS_TERMTYPE,	TCANY,		SS_END,		&QTermTelnet::no_op		},

	{ SS_END,	TCANY,		SS_END,		&QTermTelnet::no_op		},
	{ FSINVALID,	TCANY,		FSINVALID,	&QTermTelnet::tnabort		},
};



/*------------------------------------------------------------------------
 * Constructor
 *------------------------------------------------------------------------
 */
QTermTelnet::QTermTelnet( QCString cstrTermType, bool isSSH, const char * sshuser, const char * sshpasswd )
{
	term = new char[21];
	int i;
	for(i=0;i<21;i++) term[i]='\000';	//clean up, need???
	sprintf(term, cstrTermType);

	wx=80;
	wy=24;
	done_naws=0;

	synching = 0;
	doecho = 0;
	sndbinary = 0;
	rcvbinary = 0;
	noga = 0;
	termtype = 0;
	naws = 0;
	raw_size = 0;	

	// proxy related
	proxy_type = NOPROXY;
	proxy_state = 0;
	bauth = false;
	// init buffers
	from_socket = new QByteArray();
	to_ansi     = new QByteArray();
	from_ansi   = new QByteArray();
	to_socket   = new QByteArray();
	
	// create socket
	d_isSSH = isSSH;
#ifndef _NO_SSH_COMPILED
	if (d_isSSH)
		socket = new QTermSSHSocket(sshuser, sshpasswd);
	else
#endif
		socket = new QTermTelnetSocket();
	
	// connect signal and slots
	connect ( socket, SIGNAL( connected() ),
	      	this, SLOT( connected() ) );
	connect ( socket, SIGNAL( readyRead() ),
		this, SLOT( socketReadyRead() ) );
	connect ( socket, SIGNAL( error( int ) ),
		this, SLOT( showError( int ) ) );
	connect ( socket, SIGNAL( hostFound() ),
		this, SLOT( hostFound() ) );
	connect ( socket, SIGNAL( delayedCloseFinished() ),
		this, SLOT( delayCloseFinished() ) );
	connect ( socket, SIGNAL( connectionClosed() ),
		this, SLOT( closed() ) );
	// Init telnet, mainly the FSMs	
	init_telnet();
}

/*------------------------------------------------------------------------
 * destructor
 *------------------------------------------------------------------------
 */
QTermTelnet::~QTermTelnet()
{
	// delete objects
	delete [] term;
    	delete socket;
	delete from_socket;
	delete to_ansi;
	delete from_ansi;
	delete to_socket;
}



/*------------------------------------------------------------------------
 * init_telnet
 *------------------------------------------------------------------------
 */
void QTermTelnet::init_telnet()
{
	fsmbuild();	/* setup FSMs */
}


/*------------------------------------------------------------------------
 * fsmbuild - build the Finite State Machine data structures
 *------------------------------------------------------------------------
 */
void QTermTelnet::fsmbuild()
{
	fsminit(ttfsm, ttstab, NTSTATES);
	ttstate = TSDATA;

	fsminit(subfsm, substab, NSSTATES);
	substate = SS_START;

}

/*------------------------------------------------------------------------
 * fsminit - Finite State Machine initializer
 *------------------------------------------------------------------------
 */
void QTermTelnet::fsminit(u_char fsm[][NCHRS], struct fsm_trans ttab[], int nstates)
{
	struct fsm_trans	*pt;
	int			sn, ti, cn;

	for (cn=0; cn<NCHRS; ++cn)
		for (ti=0; ti<nstates; ++ti)
			fsm[ti][cn] = TINVALID;

	for (ti=0; ttab[ti].ft_state != FSINVALID; ++ti) {
		pt = &ttab[ti];
		sn = pt->ft_state;
		if (pt->ft_char == TCANY) {
			for (cn=0; cn<NCHRS; ++cn)
				if (fsm[sn][cn] == TINVALID)
					fsm[sn][cn] = ti;
		} else
			fsm[sn][pt->ft_char] = ti;
	}
	/* set all uninitialized indices to an invalid transition */
	for (cn=0; cn<NCHRS; ++cn)
		for (ti=0; ti<nstates; ++ti)
			if (fsm[ti][cn] == TINVALID)
				fsm[ti][cn] = ti;
}

/*------------------------------------------------------------------------
 * connect to host
 *------------------------------------------------------------------------
 */
void QTermTelnet::connectHost(const QString& hostname, Q_UINT16 portnumber)
{
	host=hostname;
	port=portnumber;
	addr_host.sin_port=htons(portnumber);
	
	if( proxy_type == NOPROXY )
	{
		 socket->connectToHost( host, port );
	}
	else
	{
		//get ip addr from name
		struct hostent * hostent;
	#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
		WSADATA wsd;
		if (WSAStartup(0x202,&wsd) != 0)
		{
			emit TelnetState(TSEINIWINSOCK);
			WSACleanup( );
			return;
		}
	#endif
		hostent = gethostbyname( host );
		if( hostent==NULL )
		{
			emit TelnetState(TSEGETHOSTBYNAME);
			return;
		}
		//now only ipv4 support
		memcpy(&addr_host.sin_addr,hostent->h_addr,4);

		socket->connectToHost( proxy_host, proxy_port );
	}
	// host name resolving
	emit TelnetState( TSRESOLVING );

}

void QTermTelnet::setWindowSize( int x, int y )
{
	wx=x;
	wy=y;

	char cmd[10];
	
	cmd[0] = (char)TCIAC;
	cmd[1] = (char)TCSB;
	cmd[2] = (char)TONAWS;
	cmd[3] = (char)(short(x)>>8);
	cmd[4] = (char)(short(x)%0xff);
	cmd[5] = (char)(short(y)>>8);
	cmd[6] = (char)(short(y)%0xff);
	cmd[7] = (char)TCIAC;
	cmd[8] = (char)TCSE;

	write(cmd, 9);
}

/*------------------------------------------------------------------------
 * set proxy
 *-----------------------------------------------------------------------
 */
void QTermTelnet::setProxy( int nProxyType, bool bAuth,
			const QString& strProxyHost, Q_UINT16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd)
{
#ifndef _NO_SSH_COMPILED
	if (d_isSSH)
		return;
#endif
	proxy_type=nProxyType;
	if ( proxy_type==NOPROXY )
		return;
	bauth		= bAuth;
	proxy_host	= strProxyHost;
	proxy_port	= uProxyPort;
	proxy_usr	= strProxyUsr;
	proxy_pwd	= strProxyPwd;

}
/*------------------------------------------------------------------------
 * close connection
 *-----------------------------------------------------------------------
 */

void QTermTelnet::close()
{
	socket->close();
}



/*------------------------------------------------------------------------
 * SLOT connected
 *------------------------------------------------------------------------
 */
void QTermTelnet::connected()
{
	QCString strPort;
	char command[9];
	memset(command,0,9);

	char *proxyauth;
	char *request;
	int len=0;
	
	switch( proxy_type )
	{
	case NOPROXY:		// no proxy
		emit TelnetState( TSHOSTCONNECTED );
		return;
	case WINGATE:		// Wingate Proxy
		strPort.setNum( port );
		write( host.local8Bit(), host.length() );
		write( " ",1 );
		write( strPort, strPort.length() );
		write( &wingate_enter,1 );// CTRL+J
		emit TelnetState( TSHOSTCONNECTED );
		return;
	case SOCKS4:		// Socks4 Proxy
		command[0]='\x04';
		command[1]='\x01';
		memcpy(&command[2],&addr_host.sin_port,2);
		memcpy(&command[4],&addr_host.sin_addr,4);
		write(command,9);
		proxy_state = 1;
		emit TelnetState( TSPROXYCONNECTED );
		return;
	case SOCKS5:		// Socks5 Proxy
		if( bauth )
		{
			command[0]='\x05';
			command[1]='\x02';
			command[2]='\x02';
			command[3]='\x00';
			write(command,4);
		}
		else
		{
			command[0]='\x05';
			command[1]='\x01';
			command[2]='\x00';
			write(command,3);
		}
		proxy_state=1;
		emit TelnetState( TSPROXYCONNECTED );
		return;
	case HTTP:
		proxyauth=NULL;
		if(bauth)
			proxyauth = basic_authentication_encode
				(proxy_usr, proxy_pwd, "Proxy-Authorization");

		len = proxyauth!=NULL?strlen(proxyauth):0;
	
		request = new char[host.length()+len+81];
		
		sprintf(request,
					"CONNECT %s:%u HTTP/1.0\n"
					"%s\n", 
					host.latin1(),port,
					proxyauth!=NULL?proxyauth:"");

		write(request, strlen(request));
		delete [] request;
		free(proxyauth);
		proxy_state=1;
		emit TelnetState( TSPROXYCONNECTED );
		return;
	default:
		emit TelnetState( TSHOSTCONNECTED );
		return;		
	}		
}
/*------------------------------------------------------------------------
 * SLOT closed
 *------------------------------------------------------------------------
 */

void QTermTelnet::closed()
{
	emit TelnetState( TSCLOSED );
}
/*------------------------------------------------------------------------
 * SLOT delayCloseFinished
 *------------------------------------------------------------------------
 */

void QTermTelnet::delayCloseFinished()
{
	emit TelnetState( TSCLOSEFINISH );
}
/*------------------------------------------------------------------------
 * SLOT hostFound
 *------------------------------------------------------------------------
 */
void QTermTelnet::hostFound()
{
	emit TelnetState( TSHOSTFOUND );
}
/*------------------------------------------------------------------------
 * SLOT error
 *------------------------------------------------------------------------
 */
void QTermTelnet::showError( int index )
{

	switch ( index )
	{
	case QSocket::ErrConnectionRefused:
		emit TelnetState( TSREFUSED );
		break;
	case QSocket::ErrHostNotFound:
		emit TelnetState( TSHOSTNOTFOUND );
		break;
	case QSocket::ErrSocketRead:
		emit TelnetState( TSREADERROR );
		break;
	default:
		emit TelnetState( TSERROR );;
		break;
	}

}

/*------------------------------------------------------------------------
 * SLOOT socketReadyRead -
 * 	when socket has data to upload, it send out readyRead() signal, it
 * 	invokes this SLOT to read data in, do telnet decode job, and send out
 * 	readyRead() SIGNAL to upper layer
 *------------------------------------------------------------------------
 */
void QTermTelnet::socketReadyRead()
{
	int nbytes,nread;
		
	// get the data size
    	nbytes=socket->bytesAvailable();
	if (nbytes <= 0)   return;
		
    raw_size=nbytes;

	//resize input buffer
	from_socket->resize(nbytes);
		
	//read data from socket to from_socket
	nread=socket->readBlock( from_socket->data(), nbytes);
	
	//do some checks
	if ( nread <= 0 ) {
		qWarning("Reading from socket:nread<=0");
		return;
	}
	if ( nread > nbytes ) {
		qWarning("Reading overflow from socket:nread>nbytes");
		return;
	}
	
	// Socks4 Proxy Reply
	if( proxy_type==SOCKS4 && proxy_state==1 )
	{
		if( nread!=8 )
		{
			emit TelnetState( TSPROXYERROR );
			return;
		}

		if( from_socket->at(1)==90 )
			emit TelnetState( TSHOSTCONNECTED );
		else
			emit TelnetState( TSPROXYFAIL );
		proxy_state=0;
		return;
	}

	// HTTP Proxy Reply
	if( proxy_type==HTTP && proxy_state!=0 )
	{
		const char *error;
		int statcode = parse_http_status_line(from_socket->data(), &error);
		if( statcode>= 200 && statcode<300 )
			emit TelnetState( TSHOSTCONNECTED );
		else
			emit TelnetState( TSPROXYFAIL );
		proxy_state=0;
		return;
	}

	// Socks5 Proxy Reply
	if( proxy_type==SOCKS5 && proxy_state!=0 )
	{
		socks5_reply(nread);
		return;
	}


	//resize output buffer
	to_ansi->resize(2*nread);
	to_socket->resize(4*nread);

	rsize=0;
	wsize=0;
		
	// do telnet decode job...
	struct fsm_trans	*pt;
	int			i, ti;

	u_char c;
	for (i=0; i<nread; ++i) {
		c = (u_char)(from_socket->at(i));
		ti = ttfsm[ttstate][c];
		pt = &ttstab[ti];
		(this->*( pt->ft_action ))((int)c);
		ttstate = pt->ft_next;
	}
	
	
	// flush the to_socket buffer, it contain response to server
	if ( wsize > 0 ){
		socket->writeBlock(to_socket->data(), wsize);
		socket->flush();	
	}
										
	/* send SIGNAL readyRead() with the size of data available*/					
	if ( rsize > 0 )
		emit readyRead(rsize);	
			
}

int QTermTelnet::raw_len()
{
    return raw_size;
}

/*------------------------------------------------------------------------
 * actions
 *------------------------------------------------------------------------
 */
int QTermTelnet::read_raw(char * data, uint maxlen)
{
    //do some checks
    if(data==0) {
        qWarning("read: NULL pointer");
        return -1;
    }
    if(maxlen < raw_size) {
        /* we need all data be read out in one read */
        qWarning("read: upper layer accept buffer too small");
        return -1;
    }

    //do it, memcpy( destination, source, size)
    memcpy(data, from_socket->data(), raw_size);
    return raw_size;
}


/*------------------------------------------------------------------------
 * actions
 *------------------------------------------------------------------------
 */
int QTermTelnet::read(char * data, uint maxlen)
{
	//do some checks
	if(data==0) {
		qWarning("read: NULL pointer");
		return -1;
	}
	if(maxlen < rsize) {
		/* we need all data be read out in one read */
		qWarning("read: upper layer accept buffer too small");
		return -1;
	}
	
	//do it, memcpy( destination, source, size)
	memcpy(data, to_ansi->data(), rsize);
	return rsize;
}

/*------------------------------------------------------------------------
 * actions
 *------------------------------------------------------------------------
 */


/*------------------------------------------------------------------------
 * writeBlock
 * 	write data from data-> to socket, the length of data is len
 *------------------------------------------------------------------------
 */
int QTermTelnet::write(const char * data, uint len)
{
	// accept data, (This seems can be removed????)
	from_ansi->resize(len);
	memcpy(from_ansi->data(), data, len);
	
	// resize output buffer
	to_socket->resize(2*len);
	wsize=0;

	// process keyboard input
	// because we use GUI, there is no need to support a "command mode"
	// So the keyboard-input FSM is simplied.
	
	uint	i;
	
	u_char c;	// for gcc's happy :)
	for (i=0; i<len; ++i) {
		c = (u_char)(from_ansi->at(i));
		soputc((int)c);
	}
	
	//flush socket
	socket->writeBlock(to_socket->data(), wsize);
	socket->flush();	

	emit TelnetState(TSWRITED);

	return 0;
}


/*------------------------------------------------------------------------
 * Trans functions
 * All trans functions use from_socket, to_ansi, to_socket buffers, and
 * rsize, wsize .
 *------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------
 * tcdm - handle the telnet "DATA MARK" command (marks end of SYNCH)
 *------------------------------------------------------------------------
 */
int QTermTelnet::tcdm(int)
{
	if (synching > 0)
		synching--;
	return 0;
}

/*------------------------------------------------------------------------
 * rcvurg - receive urgent data input (indicates a telnet SYNCH)
 *------------------------------------------------------------------------
 */
/*
int QTermTelnet::rcvurg(int sig)
{
	synching++;
}
*/

/*------------------------------------------------------------------------
 * recopt - record option type
 *------------------------------------------------------------------------
 */

int QTermTelnet::recopt(int c)
{
	option_cmd = c;
	return 0;
}

/*------------------------------------------------------------------------
 * no_op - do nothing
 *------------------------------------------------------------------------
 */

int QTermTelnet::no_op(int)
{
	return 0;
}

/*------------------------------------------------------------------------
 * do_echo - handle TELNET WILL/WON'T ECHO option
 *------------------------------------------------------------------------
 */
int QTermTelnet::do_echo(int c)
{
	if (doecho) {
		if (option_cmd == TCWILL)
			return 0;	/* already doing ECHO		*/
	} else if (option_cmd == TCWONT)
		return 0;		/* already NOT doing ECHO	*/

	doecho = !doecho;
	
        putc_down(TCIAC);
        if (doecho)
                 putc_down(TCDO);
        else
                 putc_down(TCDONT);
  	putc_down((char)c);
	return 0;
}

/*------------------------------------------------------------------------
 * putc_down - put a character in to_socket buffer.
 * wsize represent the number of bytes in to_socket buffer, and the buffer
 * is addressed from 0, NOT 1.
 *------------------------------------------------------------------------
 */
void QTermTelnet::putc_down(u_char c)
{
	// check overflow
	if ( (wsize+1) > to_socket->size() ) {
		qWarning("putc_down : to_socket buffer overflow");
		return;
	}
	
	// put it in the buffer
	to_socket->at(wsize) = c;
	wsize++;	
	return;
}

/*------------------------------------------------------------------------
 * do_notsup - handle an unsupported telnet "will/won't" option
 *------------------------------------------------------------------------
 */

int QTermTelnet::do_notsup(int c)
{
	putc_down(TCIAC);
	putc_down(TCDONT);
	putc_down((char)c);
	return 0;
}

/*------------------------------------------------------------------------
 * do_noga - don't do telnet Go-Ahead's
 *------------------------------------------------------------------------
 */

int QTermTelnet::do_noga(int c)
{
	if (noga) {
		if (option_cmd == TCWILL)
			return 0;
	} else if (option_cmd == TCWONT)
		return 0;
		
	noga = !noga;
	
	putc_down(TCIAC);
	if (noga)
		putc_down(TCDO);
	else
		putc_down(TCDONT);
	putc_down((char)c);
	return 0;
}

/*------------------------------------------------------------------------
 * do_txbinary - handle telnet "will/won't" TRANSMIT-BINARY option
 *------------------------------------------------------------------------
 */

int QTermTelnet::do_txbinary(int c)
{
	if (rcvbinary) {
		if (option_cmd == TCWILL)
			return 0;
	} else if (option_cmd == TCWONT)
		return 0;
		
	rcvbinary = !rcvbinary;
	
	putc_down(TCIAC);
	if (rcvbinary)
		putc_down(TCDO);
	else
		putc_down(TCDONT);
	putc_down((char)c);
	return 0;
}

/*------------------------------------------------------------------------
 * will_notsup - handle an unsupported telnet "do/don't" option
 *------------------------------------------------------------------------
 */

int QTermTelnet::will_notsup(int c)
{
	putc_down(TCIAC);
	putc_down(TCWONT);
	putc_down((char)c);
	return 0;
}

/*------------------------------------------------------------------------
 * will_txbinary - handle telnet "do/don't" TRANSMIT-BINARY option
 *------------------------------------------------------------------------
 */

int QTermTelnet::will_txbinary(int c)
{
	if (sndbinary) {
		if (option_cmd == TCDO)
			return 0;
	} else if (option_cmd == TCDONT)
		return 0;
		
	sndbinary = !sndbinary;
	
	putc_down(TCIAC);
	if (sndbinary)
		putc_down(TCWILL);
	else
		putc_down(TCWONT);
	putc_down((char)c);
	return 0;
}

/*------------------------------------------------------------------------
 * will_termtype - handle telnet "do/don't" TERMINAL-TYPE option
 *------------------------------------------------------------------------
 */
int QTermTelnet::will_termtype(int c)
{
	if (termtype) {
		if (option_cmd == TCDO)
			return 0;
	} else if (option_cmd == TCDONT)
		return 0;
		
	termtype = !termtype;
				
	putc_down(TCIAC);
	
	if (termtype)
		putc_down(TCWILL);
	else
		putc_down(TCWONT);
		
	putc_down((char)c);

	// Do NOT use this assume! some foolish BBS not response the request	
	/* The client expects that once the remote application receives
	   terminal type information it will send control sequences for
	   the terminal, which cannot be sent using the NVT encoding, So
	   change the transfer mode to binary in both directions */
	 /* set up binary data path; send WILL, DO */
/*	if (termtype) {	
		option_cmd = TCWILL;
		do_txbinary(TOTXBINARY);
		option_cmd = TCDO;
		will_txbinary(TOTXBINARY);
	}
*/	
	return 0;
}

int QTermTelnet::will_naws(int c)
{
	done_naws = 1;

	if (naws) {
		if (option_cmd == TCDO)
			return 0;
	} else if (option_cmd == TCDONT)
		return 0;

	naws = !naws;

	if (naws)
	{
		if(!done_naws)
		{
			putc_down(TCIAC);
			putc_down(TCWILL);
			putc_down((char)c);
		}
		setWindowSize( wx, wy );
	}
	
	return 0;
}
/*------------------------------------------------------------------------
 * subopt - do option subnegotiation FSM transitions
 *------------------------------------------------------------------------
 */
int QTermTelnet::subopt(int c)
{
	struct	fsm_trans	*pt;
	int			ti;

	ti = subfsm[substate][c];
	pt = &substab[ti];
	(this->*(pt->ft_action))(c);
	substate = pt->ft_next;
	return 0;
}

/*------------------------------------------------------------------------
 * subtermtype - do terminal type option subnegotation
 *------------------------------------------------------------------------
 */

int QTermTelnet::subtermtype(int)
{
	char *i;
	/* have received IAC.SB.TERMTYPE.SEND */

	putc_down(TCIAC);
	putc_down(TCSB);
	putc_down(TOTERMTYPE);
	putc_down(TT_IS);
	
	//write term type string
	//fputs(term, sfp);
	for(i=term; (*i)!='\000'; i++)
		putc_down(*i);
	
	putc_down(TCIAC);
	putc_down(TCSE);
	return 0;
}

/*------------------------------------------------------------------------
 * subend - end of an option subnegotiation; reset FSM
 *------------------------------------------------------------------------
 */

int QTermTelnet::subend(int)
{
	substate = SS_START;
	return 0;
}

/*------------------------------------------------------------------------
 * soputc - move a character from the keyboard to the socket
 * 	    convert an character into the NVT encoding and send it
 *	    through the socket to the server.
 *------------------------------------------------------------------------
 */

int QTermTelnet::soputc(int c)
{
	if (sndbinary) {
		if (c == TCIAC)
			putc_down(TCIAC); /* byte-stuff IAC	*/
		putc_down(c);
		return 0;
	}
	
	//c &= 0x7f;	/* 7-bit ASCII only ???*/
	// Convert local special characters to NVT characters
	/* // BBS don't need control signals
	if (c == t_intrc || c == t_quitc) {	// Interrupt		
		qWarning(" t_intrc ");
		putc_down(TCIAC);
		putc_down(TCIP);
	} else if (c == sg_erase) {		// Erase Char		
		qWarning(" sg_erase ");
		putc_down(TCIAC);
		putc_down(TCEC);
	} else if (c == sg_kill) {		// Erase Line		
		qWarning(" sg_kill ");
		putc_down(TCIAC);
		putc_down(TCEL);
	} else if (c == t_flushc) {		// Abort Output		
		qWarning(" t_flushc ");
		putc_down(TCIAC);
		putc_down(TCAO);
	} else  */
		putc_down((char)c);
	
	return 0;
}

/*------------------------------------------------------------------------
 * xputc - putc to upper layer with optional file scripting
 *------------------------------------------------------------------------
 */
int QTermTelnet::xputc_up(char ch)
{
	/*if (scrfp)
		(void) putc(ch, scrfp);*/
	
	if( (rsize+1) > to_ansi->size() ) {
		qWarning("xputc_up : Buffer to_ansi overflow");
		return -1;
	}
	
	to_ansi->at(rsize) = ch;
	rsize++;
		
	return 0;
}

/*------------------------------------------------------------------------
 * xfputs - fputs with optional file scripting
 *------------------------------------------------------------------------
 */
int QTermTelnet::xputs_up(char *str)
{
	/*if (scrfp)
		fputs(str, scrfp);*/
		
	char *i;
	for ( i=str; (*i)!='\000'; i++)
		xputc_up(*i);
	return 0;
}

/*------------------------------------------------------------------------
 * ttputc - print a single character on a Network Virtual Terminal
 *------------------------------------------------------------------------
 */
int QTermTelnet::ttputc(int c)
{
	if (rcvbinary) {
		xputc_up((char)c);	/* print uninterpretted	*/
		return 0;
	}
		/* no data, if in SYNCH	*/
/*
	if (synching)		
		return 0;
*/	
	/* QTermTelnet doesnot interpret NVT code, provide datas to upper
	   layer directly. So, <cr><lf>	will not be replaced with <lf>
	*/
	xputc_up((char)c);
	
	return 0;
}

/*------------------------------------------------------------------------
 * invalid state reached, aborted
 *------------------------------------------------------------------------
 */
int QTermTelnet::tnabort(int)
{
	qWarning("invalid state reached, aborted");
//	exit(-1);
	return -1;
}
/*------------------------------------------------------------------------
 * connect command for socks5
 *------------------------------------------------------------------------
 */
void QTermTelnet::socks5_connect()
{
	char * command = new char[10];
	memset(command,0,10);
	command[0]='\x05';
	command[1]='\x01';
	command[2]='\x00';
	command[3]='\x01';
	memcpy( command+4, &addr_host.sin_addr,4 );
	memcpy( command+8, &addr_host.sin_port,2 ); 
	write( command, 10 );
	delete []command;
}
/*------------------------------------------------------------------------
 * authentation command for socks5
 *------------------------------------------------------------------------
 */
void QTermTelnet::socks5_auth()
{
	int ulen = proxy_usr.length();
	int plen = proxy_pwd.length();
	char * command = new char[3+ulen+plen];
	sprintf((char *)command,"  %s %s",proxy_usr.local8Bit().data(),
					proxy_pwd.local8Bit().data());
	command[0]='\x01';
	command[1]=ulen;
	command[2+ulen] = plen;
	write( command,3+ulen+plen );
	delete []command;	
}
/*------------------------------------------------------------------------
 * reply  from socks5
 *------------------------------------------------------------------------
 */
void QTermTelnet::socks5_reply( int nread )
{
	if( proxy_state==1 )	// Socks5 Proxy Replay 1
	{
		if( nread!=2 )
		{
			proxy_state=0;
			emit TelnetState( TSPROXYERROR );
			return;
		}
		switch( from_socket->at(1) )
		{
		case '\x00':	// no authentation needed
			socks5_connect();
			proxy_state=3;
			emit TelnetState( TSCONNECTVIAPROXY );
			return;
		case '\x02':	//need user/password
			socks5_auth();
			proxy_state=2;
			emit TelnetState( TSPROXYAUTH );
			return;
		case '\xff':
			proxy_state=0;
			emit TelnetState( TSPROXYFAIL );
			return;
		default:
			proxy_state=0;
			emit TelnetState( TSPROXYFAIL );
			return;
		}			
	}

	else if( proxy_state==2)	//Socks5 Proxy Replay 2
	{
		if( nread!=2 )
		{
			proxy_state=0;
			emit TelnetState( TSPROXYERROR );
			return;
		}
		

		if( from_socket->at(1)!='\00')
		{
			proxy_state=0;
			emit TelnetState( TSPROXYFAIL );
			return;
		}
		socks5_connect();
		proxy_state=3;
		emit TelnetState( TSCONNECTVIAPROXY );
		return;
	}
	else if( proxy_state==3)	//Socks5 Proxy Replay 3
	{
		proxy_state=0;
		if( nread!=10 )
		{
			emit TelnetState( TSPROXYERROR );
			return;
		}
		if( from_socket->at(1)!='\00' )
		{
			emit TelnetState( TSPROXYFAIL );
			return;
		}		
		emit TelnetState( TSHOSTCONNECTED );
		return;	
	}	
	else
	{
		proxy_state=0;
		return;
	}
}

/* hack from wget/http.c */
/* How many bytes it will take to store LEN bytes in base64.  */
#define BASE64_LENGTH(len) (4 * (((len) + 2) / 3))

/* Encode the string S of length LENGTH to base64 format and place it
   to STORE.  STORE will be 0-terminated, and must point to a writable
   buffer of at least 1+BASE64_LENGTH(length) bytes.  */
static void
base64_encode (const char *s, char *store, int length)
{
  /* Conversion table.  */
  static char tbl[64] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
  };
  int i;
  unsigned char *p = (unsigned char *)store;

  /* Transform the 3x8 bits to 4x6 bits, as required by base64.  */
  for (i = 0; i < length; i += 3)
    {
      *p++ = tbl[s[0] >> 2];
      *p++ = tbl[((s[0] & 3) << 4) + (s[1] >> 4)];
      *p++ = tbl[((s[1] & 0xf) << 2) + (s[2] >> 6)];
      *p++ = tbl[s[2] & 0x3f];
      s += 3;
    }
  /* Pad the result if necessary...  */
  if (i == length + 1)
    *(p - 1) = '=';
  else if (i == length + 2)
    *(p - 1) = *(p - 2) = '=';
  /* ...and zero-terminate it.  */
  *p = '\0';
}

/* Create the authentication header contents for the `Basic' scheme.
   This is done by encoding the string `USER:PASS' in base64 and
   prepending `HEADER: Basic ' to it.  */
static char *
basic_authentication_encode (const char *user, const char *passwd,
			     const char *header)
{
  char *t1, *t2, *res;
  int len1 = strlen (user) + 1 + strlen (passwd);
  int len2 = BASE64_LENGTH (len1);

  t1 = (char *)alloca (len1 + 1);
  sprintf (t1, "%s:%s", user, passwd);
  t2 = (char *)alloca (1 + len2);
  base64_encode (t1, t2, len1);
  res = (char *)malloc (len2 + 11 + strlen (header));
  sprintf (res, "%s: Basic %s\r\n", header, t2);

  return res;
}

/* Parse the HTTP status line, which is of format:

   HTTP-Version SP Status-Code SP Reason-Phrase

   The function returns the status-code, or -1 if the status line is
   malformed.  The pointer to reason-phrase is returned in RP.  */
static int
parse_http_status_line (const char *line, const char **reason_phrase_ptr)
{
  /* (the variables must not be named `major' and `minor', because
     that breaks compilation with SunOS4 cc.)  */
  int mjr, mnr, statcode;
  const char *p;

  *reason_phrase_ptr = NULL;

  /* The standard format of HTTP-Version is: `HTTP/X.Y', where X is
     major version, and Y is minor version.  */
  if (strncmp (line, "HTTP/", 5) != 0)
    return -1;
  line += 5;

  /* Calculate major HTTP version.  */
  p = line;
  for (mjr = 0; isdigit (*line); line++)
    mjr = 10 * mjr + (*line - '0');
  if (*line != '.' || p == line)
    return -1;
  ++line;

  /* Calculate minor HTTP version.  */
  p = line;
  for (mnr = 0; isdigit (*line); line++)
    mnr = 10 * mnr + (*line - '0');
  if (*line != ' ' || p == line)
    return -1;
  /* Wget will accept only 1.0 and higher HTTP-versions.  The value of
     minor version can be safely ignored.  */
  if (mjr < 1)
    return -1;
  ++line;

  /* Calculate status code.  */
  if (!(isdigit (*line) && isdigit (line[1]) && isdigit (line[2])))
    return -1;
  statcode = 100 * (*line - '0') + 10 * (line[1] - '0') + (line[2] - '0');

  /* Set up the reason phrase pointer.  */
  line += 3;
  /* RFC2068 requires SPC here, but we allow the string to finish
     here, in case no reason-phrase is present.  */
  if (*line != ' ')
    {
      if (!*line)
	*reason_phrase_ptr = line;
      else
	return -1;
    }
  else
    *reason_phrase_ptr = line + 1;

  return statcode;
}
