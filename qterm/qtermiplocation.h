#ifndef QTERMIPLOCATION_H
#define QTERMIPLOCATION_H

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_IP_LOCATION_FILE "QQWry.dat"
class QString;
class QCString;
typedef unsigned long uint32;

struct _ip_finder {
        uint32     offset_first_start_ip;      // first abs offset of start ip
        uint32     offset_last_start_ip;       // last abs offset of start ip
        uint32     cur_start_ip;               // start ip of current search range
        uint32     cur_end_ip;                 // end ip of current search range
        uint32     offset_cur_end_ip;          // where is the current end ip saved
        FILE *ipfp;                        // IO Channel to read file
};// struct _ip_finder

class QTermIPLocation
{
public:
        QTermIPLocation(QString& pathLib);
        ~QTermIPLocation();
        _ip_finder *f;
        bool getLocation( QString& url, QCString& country, QCString& city );
	bool haveFile();
protected:
	bool fileExist;
        uint32 byteArrayToInt( char *ip, int count );
        void readFrom( FILE *fp, uint32 offset, char *buf, int len );
        int readLineFrom( FILE *fp, uint32 offset, QCString& ret_str );
        uint32 getString( FILE *fp, uint32 offset, uint32 lastoffset, QCString& str, unsigned int flag );
        void getCountryCity( FILE *fp, uint32 offset, QCString& country, QCString& city );
	void setIpRange( int rec_no, _ip_finder *f );
};
#endif		//QTERMIPLOOKER_H
