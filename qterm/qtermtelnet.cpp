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

#include <stdlib.h>
#include <ctype.h>
#include <qsocket.h>


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
	{ TSWOPT,	TCANY,		TSDATA,		&QTermTelnet::do_notsup		},

	{ TSDOPT,	TONAWS,		TSDATA,		&QTermTelnet::will_naws		},
	{ TSDOPT,	TOTERMTYPE,	TSDATA,		&QTermTelnet::will_termtype	},
	{ TSDOPT,	TOTXBINARY,	TSDATA,		&QTermTelnet::will_txbinary	},
	{ TSDOPT,	TCANY,		TSDATA,		&QTermTelnet::will_notsup	},

	{ FSINVALID,	TCANY,		FSINVALID,	&QTermTelnet::tnabort		},
};


struct fsm_trans QTermTelnet::substab[] = {
        /* State        Input           Next State      Action  */
        /* ------       ------          -----------     ------- */
	{ SS_START,		TOTERMTYPE,	SS_TERMTYPE,	&QTermTelnet::no_op		},
	{ SS_START,		TCANY,		SS_END,			&QTermTelnet::no_op		},

	{ SS_TERMTYPE,	TT_SEND,	SS_END,			&QTermTelnet::subtermtype	},
	{ SS_TERMTYPE,	TCANY,		SS_END,			&QTermTelnet::no_op		},
	
	{ SS_END,		TCANY,		SS_END,			&QTermTelnet::no_op		},
	{ FSINVALID,	TCANY,		FSINVALID,		&QTermTelnet::tnabort	},
};



/*------------------------------------------------------------------------
 * Constructor
 *------------------------------------------------------------------------
 */
QTermTelnet::QTermTelnet( QCString cstrTermType, int rows, int columns, bool isSSH, const char * sshuser, const char * sshpasswd )
{
	term = new char[21];
	int i;
	for(i=0;i<21;i++) term[i]='\000';	//clean up, need???
	sprintf(term, cstrTermType);

	wx=columns;
	wy=rows;
	
	done_naws=0;
	synching = 0;
	doecho = 0;
	sndbinary = 0;
	rcvbinary = 0;
	noga = 0;
	termtype = 0;
	naws = 0;

	raw_size = 0;	

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
	connect ( socket, SIGNAL( SocketState(int) ),
		this, SIGNAL( TelnetState(int) ) );
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
	done_naws=0;
	synching = 0;
	doecho = 0;
	sndbinary = 0;
	rcvbinary = 0;
	noga = 0;
	termtype = 0;
	naws = 0;

	socket->connectToHost( hostname, portnumber );
	// host name resolving
	emit TelnetState( TSRESOLVING );
}

void QTermTelnet::windowSizeChanged(int x, int y)
{
	wx=x;
	wy=y;
	if(bConnected)
	{
		naws = 0;

		qWarning("sent NAWS");
		char cmd[10];
		cmd[0] = (char)TCIAC;
		cmd[1] = (char)TCSB;
		cmd[2] = (char)TONAWS;
		cmd[3] = (char)(short(wx)>>8);
		cmd[4] = (char)(short(wx)&0xff);
		cmd[5] = (char)(short(wy)>>8);
		cmd[6] = (char)(short(wy)&0xff);
		cmd[7] = (char)TCIAC;
		cmd[8] = (char)TCSE;
		write(cmd, 9);
/*
		char cmd[4];
		cmd[0] = (char)TCIAC;
		cmd[1] = (char)TCWILL;
		cmd[2] = (char)TONAWS;
		write(cmd, 3);
*/
	}
}

/*------------------------------------------------------------------------
 * set proxy
 *-----------------------------------------------------------------------
 */
void QTermTelnet::setProxy( int nProxyType, bool bAuth,
			const QString& strProxyHost, Q_UINT16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd)
{
	socket->setProxy(nProxyType, bAuth,
			strProxyHost, uProxyPort,
			strProxyUsr, strProxyPwd);
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
	bConnected = true;
	emit TelnetState( TSHOSTCONNECTED );
}
/*------------------------------------------------------------------------
 * SLOT closed
 *------------------------------------------------------------------------
 */

void QTermTelnet::closed()
{
	bConnected = false;


	emit TelnetState( TSCLOSED );
}
/*------------------------------------------------------------------------
 * SLOT delayCloseFinished
 *------------------------------------------------------------------------
 */

void QTermTelnet::delayCloseFinished()
{
	bConnected = false;
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
	//qDebug("read size: %d", raw_size);

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
	if ( rsize > 0 || raw_size > 0)
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
	if (naws) {
		if (option_cmd == TCDO)
			return 0;
	} else if (option_cmd == TCDONT)
		return 0;


	naws = !naws;

	putc_down(TCIAC);
	if (naws)
		putc_down(TCWILL);
	else
		putc_down(TCWONT);
	putc_down((char)c);

	putc_down(TCIAC);
	putc_down(TCSB);
	putc_down(TONAWS);
	putc_down((char)(short(wx)>>8));
	putc_down((char)(short(wx)&0xff));
	putc_down((char)(short(wy)>>8));
	putc_down((char)(short(wy)&0xff));
	putc_down(((char)TCIAC));
	putc_down((char)TCSE);

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

