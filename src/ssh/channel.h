//
// C++ Interface: sshchannel
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SSH_CHANNEL_H
#define SSH_CHANNEL_H

#include <stdint.h>
#include <QtCore/QObject>

namespace QTerm
{
class SSH2InBuffer;
class SSH2OutBuffer;
class SSH1InBuffer;
class SSH1OutBuffer;
/**
 @author hooey <hephooey@gmail.com>
*/
class SSH2Channel : public QObject
{
    Q_OBJECT
public:
    SSH2Channel(SSH2InBuffer * in, SSH2OutBuffer * out, QObject *parent = 0);

    ~SSH2Channel();
    void openChannel();
    QByteArray readData(int id, unsigned long size);
    void writeData(int id, const QByteArray & data);
    unsigned long bytesAvailable(int id);
signals:
    void dataReady(int id);
    void newChannel(int id);
private slots:
    void channelPacketReceived(int flag);
private:
    typedef struct Channel_t
    {
        uint32_t localID;
        uint32_t localWindow;
        uint32_t localPacketSize;
        uint32_t remoteID;
        uint32_t remoteWindow;
        uint32_t remotePacketSize;
        QByteArray data;
    }
    Channel;
    void channelOpened();
    void adjustWindow(uint id, uint size);
    void remoteWindowAdjusted();
    void requestPty(uint id);
    void receiveData();
    QList<Channel * > m_channelList;
    SSH2InBuffer * m_in;
    SSH2OutBuffer * m_out;
};

class SSH1Channel : public QObject
{
    Q_OBJECT
public:
    SSH1Channel(SSH1InBuffer * in, SSH1OutBuffer * out, QObject *parent = 0);
    ~SSH1Channel();
    QByteArray readData(unsigned long size);
    void writeData(const QByteArray & data);
    unsigned long bytesAvailable();
signals:
    void dataReady();
private slots:
    void channelPacketReceived(int flag);
private:
    enum Status
    {
        RequestPty, Interactive, None
    };
    void requestPty();
    void receiveData();
    Status m_status;
    SSH1InBuffer * m_in;
    SSH1OutBuffer * m_out;
    QByteArray m_data;
};

}

#endif
