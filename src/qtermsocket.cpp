#include "qterm.h"
#include "qtermsocket.h"
#include "hostinfo.h"

// #include <q3socket.h>
//Added by qt3to4:
#include <QByteArray>
#include <QTcpSocket>
#include <QtGlobal>

#if !defined(Q_OS_BSD4) && !defined(Q_OS_FREEBSD_) \
            && !defined(Q_OS_MACOS) && !defined(Q_OS_DARWIN)
#include <malloc.h>
#endif

#include <stdlib.h>

#if  defined(Q_OS_SOLARIS)
#include <alloca.h>
#endif

namespace QTerm
{

/* hack from wget/http.c */
static int
parse_http_status_line (const char *line, const char **reason_phrase_ptr);


const char wingate_enter = 'J'&0x1f;

//==============================================================================
//SocketPrivate
//==============================================================================
SocketPrivate::SocketPrivate(QObject * parent)
	:QObject(parent)
{
	m_socket = new QTcpSocket(this);

	// proxy related
	proxy_type = NOPROXY;
	proxy_state = 0;
	bauth = false;
	
	connect(m_socket, SIGNAL(connected()), this, SLOT(socketConnected()));
	connect(m_socket, SIGNAL(hostFound()), this, SIGNAL(hostFound()));
	connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(connectionClosed()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
#else
	connect(m_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
#endif
}

SocketPrivate::~SocketPrivate()
{
	delete m_socket;
}

void SocketPrivate::setProxy(int nProxyType, bool bAuth,
			const QString& strProxyHost, quint16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd)
{
	proxy_type=nProxyType;
	if ( proxy_type==NOPROXY )
		return;
	bauth		= bAuth;
	proxy_host	= strProxyHost;
	proxy_port	= uProxyPort;
	proxy_usr	= strProxyUsr;
	proxy_pwd	= strProxyPwd;
}
void SocketPrivate::socketConnected()
{
	QByteArray strPort;
	QByteArray command(9,0);

    QByteArray proxyauth;
    QByteArray request;
	int len=0;
	
	switch( proxy_type )
	{
	case NOPROXY:		// no proxy
		emit connected();
		return;
	case WINGATE:		// Wingate Proxy
		strPort.setNum( port );
		writeBlock( host.toLocal8Bit() );
		writeBlock( " " );
		writeBlock( strPort );
		writeBlock( &wingate_enter );// CTRL+J
		emit SocketState( TSHOSTCONNECTED );
		return;
	case SOCKS4:		// Socks4 Proxy
		command.resize(9);
		command[0]='\x04';
		command[1]='\x01';
		memcpy(command.data()+2,&addr_host.sin_port,2);
		memcpy(command.data()+4,&addr_host.sin_addr,4);
		writeBlock(command);
		proxy_state = 1;
		emit SocketState( TSPROXYCONNECTED );
		return;
	case SOCKS5:		// Socks5 Proxy
		if( bauth )
		{
			command.resize(4);
			command[0]='\x05';
			command[1]='\x02';
			command[2]='\x02';
			command[3]='\x00';
			writeBlock(command);
		}
		else
		{
			command.resize(3);
			command[0]='\x05';
			command[1]='\x01';
			command[2]='\x00';
			writeBlock(command);
		}
		proxy_state=1;
		emit SocketState( TSPROXYCONNECTED );
		return;
	case HTTP:
        if(bauth) {
            QByteArray ba;
            ba += proxy_usr.toLocal8Bit();
            ba += ":";
            ba += proxy_pwd.toLocal8Bit();
            proxyauth.append("Proxy-Authorization: Basic ");
            proxyauth.append(ba.toBase64());
        }

        request += "CONNECT " + host.toLatin1() + ":" + QByteArray::number(port) + " HTTP/1.0\r\n";
        if (bauth)
            request += proxyauth + "\r\n";

		writeBlock(request);
		proxy_state=1;
		emit SocketState( TSPROXYCONNECTED );
		return;
	default:
		emit SocketState( TSHOSTCONNECTED );
		return;		
	}		
}

void SocketPrivate::socketReadyRead()
{
	if(proxy_type==NOPROXY || proxy_state==0) {
		emit readyRead();
		return;
	}

	int nbytes,nread;
	QByteArray from_socket;
	// get the data size
   	nbytes=bytesAvailable();
	if (nbytes <= 0)   return;
		
	//resize input buffer
	from_socket.resize(0);
		
	//read data from socket to from_socket
	from_socket = readBlock(nbytes);
	nread = from_socket.size();
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
			emit SocketState( TSPROXYERROR );
			return;
		}

		if( from_socket.at(1)==90 )
			emit connected();
		else
			emit SocketState( TSPROXYFAIL );
		proxy_state=0;
		return;
	}

	// HTTP Proxy Reply
	if( proxy_type==HTTP && proxy_state!=0 )
	{
		const char *error;
		int statcode = parse_http_status_line(from_socket.data(), &error);
		if( statcode>= 200 && statcode<300 )
			emit connected();
		else
			emit SocketState( TSPROXYFAIL );
		proxy_state=0;
		return;
	}

	// Socks5 Proxy Reply
	if( proxy_type==SOCKS5 && proxy_state!=0 )
	{
		socks5_reply(from_socket,nread);
		return;
	}
}

void SocketPrivate::flush()
{
	m_socket->flush();
}

QAbstractSocket::SocketState SocketPrivate::state()
{
	return m_socket->state();
}

HostInfo * SocketPrivate::hostInfo()
{
	return m_hostInfo;
}

void SocketPrivate::connectToHost(HostInfo * hostInfo)
{
	m_hostInfo = hostInfo;
	host=m_hostInfo->hostName();
	port=m_hostInfo->port();
	addr_host.sin_port=htons(port);

	if( proxy_type == NOPROXY )
	{
		m_socket->connectToHost(host, port);
	} else if ( proxy_type == HTTP ) {
		m_socket->connectToHost(proxy_host, proxy_port);
	} else
	{
		//get ip addr from name
		struct hostent * hostent;
	#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
		WSADATA wsd;
		if (WSAStartup(0x202,&wsd) != 0)
		{
			emit SocketState(TSEINIWINSOCK);
			WSACleanup( );
			return;
		}
	#endif
/*		QHostAddress ha(host);
		if(ha.isIPv4Address())
		{
			hostent = gethostbyaddr( (char *)host, host.length(), AF_INET );
		}
		else
*/		{
			hostent = gethostbyname( host.toLocal8Bit() );
		}

		if( hostent==NULL )
		{
			emit SocketState(TSEGETHOSTBYNAME);
			return;
		}
		//now only ipv4 support
		memcpy(&addr_host.sin_addr,hostent->h_addr,4);
		m_socket->connectToHost( proxy_host, proxy_port );
	}

}

void SocketPrivate::close()
{
	m_socket->close();
}

QByteArray SocketPrivate::readBlock(unsigned long maxlen)
{
	return m_socket->read(maxlen);
}

long SocketPrivate::writeBlock(const QByteArray & data)
{
	return m_socket->write(data);
}

unsigned long SocketPrivate::bytesAvailable()
{
	return m_socket->bytesAvailable();
}

/*------------------------------------------------------------------------
 * connect command for socks5
 *------------------------------------------------------------------------
 */
void SocketPrivate::socks5_connect()
{
	QByteArray command (10,0);
	command[0]='\x05';
	command[1]='\x01';
	command[2]='\x00';
	command[3]='\x01';
	memcpy( command.data()+4, &addr_host.sin_addr,4 );
	memcpy( command.data()+8, &addr_host.sin_port,2 ); 
	writeBlock( command );
}
/*------------------------------------------------------------------------
 * authentation command for socks5
 *------------------------------------------------------------------------
 */
void SocketPrivate::socks5_auth()
{
	int ulen = proxy_usr.length();
	int plen = proxy_pwd.length();
	QByteArray command(3+ulen+plen,0);
    command.append("  ");
    command.append(proxy_usr.toLocal8Bit());
    command.append(proxy_pwd.toLocal8Bit());
	command[0]='\x01';
	command[1]=ulen;
	command[2+ulen] = plen;
	writeBlock( command );
}
/*------------------------------------------------------------------------
 * reply  from socks5
 *------------------------------------------------------------------------
 */
void SocketPrivate::socks5_reply( const QByteArray& from_socket, int nread )
{
	if( proxy_state==1 )	// Socks5 Proxy Replay 1
	{
		if( nread!=2 )
		{
			proxy_state=0;
			emit SocketState( TSPROXYERROR );
			return;
		}
		switch( from_socket.at(1) )
		{
		case '\x00':	// no authentation needed
			socks5_connect();
			proxy_state=3;
			emit SocketState( TSCONNECTVIAPROXY );
			return;
		case '\x02':	//need user/password
			socks5_auth();
			proxy_state=2;
			emit SocketState( TSPROXYAUTH );
			return;
		case '\xff':
			proxy_state=0;
			emit SocketState( TSPROXYFAIL );
			return;
		default:
			proxy_state=0;
			emit SocketState( TSPROXYFAIL );
			return;
		}			
	}

	else if( proxy_state==2)	//Socks5 Proxy Replay 2
	{
		if( nread!=2 )
		{
			proxy_state=0;
			emit SocketState( TSPROXYERROR );
			return;
		}
		

		if( from_socket.at(1)!='\00')
		{
			proxy_state=0;
			emit SocketState( TSPROXYFAIL );
			return;
		}
		socks5_connect();
		proxy_state=3;
		emit SocketState( TSCONNECTVIAPROXY );
		return;
	}
	else if( proxy_state==3)	//Socks5 Proxy Replay 3
	{
		proxy_state=0;
		if( nread!=10 )
		{
			emit SocketState( TSPROXYERROR );
			return;
		}
		if( from_socket.at(1)!='\00' )
		{
			emit SocketState( TSPROXYFAIL );
			return;
		}		
		emit connected();
		return;	
	}	
	else
	{
		proxy_state=0;
		return;
	}
}

/* hack from wget/http.c */
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
  for (mjr = 0; QChar(*line).isDigit(); line++)
    mjr = 10 * mjr + (*line - '0');
  if (*line != '.' || p == line)
    return -1;
  ++line;

  /* Calculate minor HTTP version.  */
  p = line;
  for (mnr = 0; QChar(*line).isDigit(); line++)
    mnr = 10 * mnr + (*line - '0');
  if (*line != ' ' || p == line)
    return -1;
  /* Wget will accept only 1.0 and higher HTTP-versions.  The value of
     minor version can be safely ignored.  */
  if (mjr < 1)
    return -1;
  ++line;

  /* Calculate status code.  */
  if (!(QChar(*line).isDigit() && QChar(line[1]).isDigit() && QChar(line[2]).isDigit()))
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


//==============================================================================
//TelnetSocket
//==============================================================================

TelnetSocket::TelnetSocket()
	: Socket()
{
	d_socket = new SocketPrivate();
	connect(d_socket, SIGNAL(connected()), this, SIGNAL(connected()));
	connect(d_socket, SIGNAL(hostFound()), this, SIGNAL(hostFound()));
	connect(d_socket, SIGNAL(connectionClosed()), this, SIGNAL(connectionClosed()));
	connect(d_socket, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
	connect(d_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
	connect(d_socket, SIGNAL(SocketState(int)), this, SIGNAL(SocketState(int)));
}

TelnetSocket::~TelnetSocket()
{
	delete d_socket;
}

void TelnetSocket::flush()
{
	d_socket->flush();
}

void TelnetSocket::setProxy( int nProxyType, bool bAuth,
			const QString& strProxyHost, quint16 uProxyPort,
			const QString& strProxyUsr, const QString& strProxyPwd)
{
	d_socket->setProxy(nProxyType, bAuth,
			strProxyHost, uProxyPort,
			strProxyUsr, strProxyPwd);
}

void TelnetSocket::connectToHost(HostInfo * hostInfo)
{
	d_socket->connectToHost(hostInfo);
}

void TelnetSocket::close()
{
	d_socket->close();
}

QByteArray TelnetSocket::readBlock(unsigned long maxlen)
{
	return d_socket->readBlock(maxlen);
}

long TelnetSocket::writeBlock(const QByteArray & data)
{
	return d_socket->writeBlock(data);
}

unsigned long TelnetSocket::bytesAvailable()
{
	return d_socket->bytesAvailable();
}

} // namespace QTerm

#include <moc_qtermsocket.cpp>

