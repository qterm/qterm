//
// C++ Interface: sshpacket
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SSH_PACKET_H
#define SSH_PACKET_H

#include <QObject>
#include <QByteArray>
#include <QBuffer>

#include <openssl/bn.h>
#include "transport.h"
#include "qtermsocket.h"

namespace QTerm
{

/**
 @author hooey <hephooey@gmail.com>
*/
class SSH2InBuffer : public QObject
{
    Q_OBJECT
public:
    SSH2InBuffer(QTermSocketPrivate * plainSocket, QObject * parent = 0);
    ~SSH2InBuffer();
    u_int32_t getUInt32();
    u_char getUInt8();
    QByteArray getData(uint length);
    QByteArray getString();
    void getBN(BIGNUM * bn);
    bool atEnd();
    QByteArray & buffer();

    void setTransport(SSH2Transport* theValue)
    {
        delete m_transport;
        m_transport = theValue;
    }

signals:
    void packetReady(int flag);
    void error(const QString & message);

private slots:
    void parseData();

private:
    QByteArray m_in;
    QByteArray m_out;
    QBuffer m_buf;
    QTermSocketPrivate * m_socket;
    SSH2Transport * m_transport;
    u_int32_t m_sequenceNumber;
};

class SSH1InBuffer : public QObject
{
    Q_OBJECT
public:
    SSH1InBuffer(QTermSocketPrivate * plainSocket, QObject * parent = 0);
    ~SSH1InBuffer();
    u_int32_t getUInt32();
    u_char getUInt8();
    QByteArray getData(uint length);
    QByteArray getString();
    void getBN(BIGNUM * bn);
    bool atEnd();
    QByteArray & buffer();
    void setEncryption(SSH1Encryption* theValue)
    {
        delete m_encryption;
        m_encryption = theValue;
    }

signals:
    void packetReady(int flag);

private slots:
    void parseData();

private:
    QByteArray m_in;
    QByteArray m_out;
    QBuffer m_buf;
    QTermSocketPrivate * m_socket;
    SSH1Encryption * m_encryption;
};

class SSH2OutBuffer : public QObject
{
public:
    SSH2OutBuffer(QTermSocketPrivate * plainSocket, QObject * parent = 0);
    ~SSH2OutBuffer();
    void startPacket();
    void startPacket(u_char flag);
    void putUInt32(u_int32_t v);
    void putUInt8(u_char v);
    void putData(const QByteArray & data);
    void putString(const QByteArray & string);
    void putBN(const BIGNUM * bn);
    void sendPacket();
    QByteArray & buffer();   // TODO: ugly

    void setTransport(SSH2Transport* theValue)
    {
        delete m_transport;
        m_transport = theValue;
    }

private:
    QByteArray m_in;
    QBuffer m_buf;
    QTermSocketPrivate * m_socket;
    SSH2Transport * m_transport;
    u_int32_t m_sequenceNumber;
};


class SSH1OutBuffer : public QObject
{
public:
    SSH1OutBuffer(QTermSocketPrivate * plainSocket, QObject * parent = 0);
    ~SSH1OutBuffer();
    void startPacket();
    void startPacket(u_char flag);
    void putUInt32(u_int32_t v);
    void putUInt8(u_char v);
    void putData(const QByteArray & data);
    void putString(const QByteArray & string);
    void putBN(const BIGNUM * bn);
    void sendPacket();
    QByteArray & buffer();   // TODO: ugly

    void setEncryption(SSH1Encryption* theValue)
    {
        delete m_encryption;
        m_encryption = theValue;
    }

private:
    QByteArray m_in;
    QBuffer m_buf;
    QTermSocketPrivate * m_socket;
    SSH1Encryption * m_encryption;
//  u_int32_t m_sequenceNumber;
};

}
#endif // SSH_PACKET_H
