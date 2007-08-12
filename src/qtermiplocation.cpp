//***************************************************************************
//* made by cyber@thuee.org						    *
//***************************************************************************
#include "qtermiplocation.h"
//Added by qt3to4:

#include <QString>
#include <QRegExp>
#include <QDir>
#include <QStringList>
#include <ctype.h>
// #include <qregexp.h>

#ifdef Q_OS_WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

// #include <qstring.h>
// #include <qdir.h>
// #include <qstringlist.h>

namespace QTerm
{

extern QString pathCfg;

IPLocation::IPLocation(QString & pathLib)
{
	f = new _ip_finder;
	f->ipfp = NULL;

	fileExist = true;
	
	//case-insensitive match
	QDir dir(pathCfg);
	QStringList files = dir.entryList(QStringList("[Qq][Qq][Ww][Rr][Yy].[Dd][Aa][Tt]"),QDir::Files);
	if(!files.isEmpty()){
		if( ( f->ipfp = fopen( (pathCfg + (files.at(0))).toLocal8Bit(), "r" ) ) == NULL ){
		qDebug( "can't open ipfile !" );
		fileExist = false;
		}
	}
	else
		fileExist =  false;

//	if( ( f->ipfp = fopen( pathLib + DEFAULT_IP_LOCATION_FILE, "r" ) ) == NULL )
//		if( ( f->ipfp = fopen( pathCfg + DEFAULT_IP_LOCATION_FILE, "r" ) ) == NULL ){
//			qDebug( "can't open ipfile !" );
//			fileExist = false;
//		}

}

IPLocation::~IPLocation()
{
	if( f->ipfp != NULL )
		fclose( f->ipfp );
}

bool IPLocation::haveFile()
{
	return fileExist;
}

uint32 IPLocation::byteArrayToInt( char *ip, int count )
{
	uint32 tmp, ret = 0L; 
	if( count <1 || count >4 )
	{
		qDebug( "error byteArrayToInt!" );
		return 0;
	}
	for( int i = 0; i < count; i++ ) 
	{
		tmp = (( uint32 )ip[ i ]) & 0x000000FF;
		ret |= ( tmp << ( 8*i ) );
	}
	    return ret;
}// _byte_array_to_int


void IPLocation::readFrom( FILE *fp, uint32 offset, char *buf, int len )
{
	if( fseek( fp, (long)offset, SEEK_SET ) ==-1 )
	{
		qDebug( " readFrom error 1 " );
		memset( buf, 0, len );
		return;
	}
	if( fread( buf, sizeof( char ), len, fp ) == 0)
	{
		qDebug( " readFrom error 2 " );
		memset( buf, 0, len );
		return;
	}
	return;
}

int IPLocation::readLineFrom( FILE *fp, uint32 offset, QString& ret_str )
{
	char str[512];
	if( fseek( fp, (long)offset, SEEK_SET ) ==-1 )
	{
		qDebug( " readLineFrom error 1 " );
		ret_str = QString();
		return -1;
	}
	if( fgets( (char *) str, 512, fp ) == NULL )
	{
		qDebug( " readLineFrom error 2 " );
		ret_str = QString();
		return -1;
	}
	ret_str = QString::fromLatin1(str);
	return(  ret_str.length() );
}

uint32 IPLocation::getString( FILE *fp, uint32 offset, uint32 lastoffset, QString& ret, unsigned int flag )
{
    char *buf;
    unsigned int fg;
    if( fp == NULL ) return 0;
    buf =(char *) calloc( 3, sizeof( char ) );
    readFrom( fp, offset, buf ,1 );
    if( buf[0] == 0x01 || buf[0] == 0x02 )
    {
        fg = buf[0];
        readFrom( fp, offset + 1, buf, 3 );
        return getString( fp, byteArrayToInt( buf, 3 ), offset, ret, fg );
    }
    else
    {
        readLineFrom( fp, offset, ret );
    }
    switch ( flag ) {
        case 0x01:  return 0; 
        case 0x02:  return lastoffset + 4; 
        default:   return offset + strlen(ret.toLatin1()) + 1;
    }// switch
}


void IPLocation::getCountryCity( FILE *fp, uint32 offset, QString& country, QString& city )
{
	uint32 next_offset;
	if( fp == NULL ) return ;
	next_offset = getString( fp, offset, 0L, country, 0 );
	if( next_offset == 0 ) city = "";
	else getString( fp, next_offset, 0L, city, 0 );
	return;
}

void IPLocation::setIpRange( int rec_no, _ip_finder *f )
{
	char *buf;
	uint32 offset;
	if( f == NULL ) return;
	buf = (char *)calloc( 7, sizeof( char ) );
	offset = f->offset_first_start_ip + rec_no * 7 ;

	readFrom(f->ipfp, offset, buf, 7);
	f->cur_start_ip  = byteArrayToInt( buf, 4 );
	f->offset_cur_end_ip = byteArrayToInt( buf + 4, 3 );

	readFrom(f->ipfp, f->offset_cur_end_ip, buf, 4 );
	f->cur_end_ip = byteArrayToInt( buf, 4 );

}// _set_ip_range

bool IPLocation::getLocation( QString& url, QString& country, QString& city )
{
	int rec, record_count, B, E;
	char *buf;
	uint32 ip;
	#ifdef	Q_OS_WIN32
	uint32 ipValue = inet_addr( (const char*)url.toLatin1() );
	#else
	in_addr_t ipValue = inet_addr( (const char*)url.toLatin1() );
	#endif
	if( ipValue == -1 )
		return false;
	else
		ip = ntohl(ipValue);

	buf = (char *)calloc( 4, sizeof( char ) );
	memset( buf ,0 ,4 );
	readFrom( f->ipfp, 0L, (char *)buf, 4 );
	f->offset_first_start_ip = byteArrayToInt( (char *)buf, 4 );
	readFrom( f->ipfp, 4L, (char *)buf, 4 );
	f->offset_last_start_ip  = byteArrayToInt( (char *)buf, 4 );

	record_count = (f->offset_last_start_ip - f->offset_first_start_ip) / 7;
	if (record_count <= 1) return FALSE;

	// search for right range
	B = 0;  E = record_count;
	while (B < E-1) 
	{
		rec = (B + E) / 2; 
		setIpRange( rec, f ) ;
		if (ip == f->cur_start_ip ) { B = rec; break;}
		if (ip > f->cur_start_ip ) B= rec; else E= rec;
	}
	setIpRange( B, f );

	if (f->cur_start_ip <= ip && ip <= f->cur_end_ip) 
	{
		getCountryCity(f->ipfp, f->offset_cur_end_ip + 4, country, city);
		//country.replace( country.find( "CZ88.NET", 0, FALSE ), 8, "" );
		if( (rec =city.indexOf( "CZ88.NET", 0, Qt::CaseInsensitive )) >= 0 )
			city.replace( rec, 8, "" );
			
	} 
	else 
	{// not in this range... miss
		country = "unkown"; city = "";
	}// if ip_start<=ip<=ip_end
	return TRUE;
}
}
