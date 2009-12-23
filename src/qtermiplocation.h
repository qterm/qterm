#ifndef QTERMIPLOCATION_H
#define QTERMIPLOCATION_H

#include <stdio.h>
#include <stdlib.h>
//Added by qt3to4:


#define DEFAULT_IP_LOCATION_FILE "QQWry.dat"

class QString;
typedef unsigned long uint32;
namespace QTerm
{
struct _ip_finder {
    uint32     offset_first_start_ip;      // first abs offset of start ip
    uint32     offset_last_start_ip;       // last abs offset of start ip
    uint32     cur_start_ip;               // start ip of current search range
    uint32     cur_end_ip;                 // end ip of current search range
    uint32     offset_cur_end_ip;          // where is the current end ip saved
    FILE *ipfp;                        // IO Channel to read file
};// struct _ip_finder

class IPLocation
{
public:
    IPLocation(QString& pathLib);
    ~IPLocation();
    _ip_finder *f;
    bool getLocation(QString& url, QString& country, QString& city);
    bool haveFile();
protected:
    bool fileExist;
    uint32 byteArrayToInt(char *ip, int count);
    void readFrom(FILE *fp, uint32 offset, char *buf, int len);
    int readLineFrom(FILE *fp, uint32 offset, QString& ret_str);
    uint32 getString(FILE *fp, uint32 offset, uint32 lastoffset, QString& str, unsigned int flag);
    void getCountryCity(FILE *fp, uint32 offset, QString& country, QString& city);
    void setIpRange(int rec_no, _ip_finder *f);
};

} // namespace QTerm

#endif      //QTERMIPLOOKER_H
