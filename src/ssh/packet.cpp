//
// C++ Implementation: sshpacket
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "packet.h"
#include "crc32.h"
#include <openssl/rand.h>

#ifdef SSH_DEBUG
#include <QtDebug>
#endif

uint32_t get_u32(const void * vp)
{
    uint32_t v;
    const uint8_t * p = (const uint8_t *) vp;
    v  = (uint32_t) p[0] << 24;
    v |= (uint32_t) p[1] << 16;
    v |= (uint32_t) p[2] << 8;
    v |= (uint32_t) p[3];
    return v;
}

uint16_t get_u16(const void *vp)
{
    const uint8_t *p = (const uint8_t *) vp;
    uint16_t v;

    v  = (uint16_t) p[0] << 8;
    v |= (uint16_t) p[1];

    return (v);
}

void put_u32(void * vp, uint32_t v)
{
    uint8_t * p = (uint8_t *) vp;

    p[0] = (uint8_t)(v >> 24) & 0xff;
    p[1] = (uint8_t)(v >> 16) & 0xff;
    p[2] = (uint8_t)(v >> 8) & 0xff;
    p[3] = (uint8_t) v & 0xff;
}

void put_u16(void * vp, uint16_t v)
{
    uint8_t * p = (uint8_t *) vp;
    p[0] = (uint8_t)(v >> 8) & 0xff;
    p[1] = (uint8_t) v & 0xff;
}

#ifdef SSH_DEBUG
void dumpData(const QByteArray & data)
{
    int size = data.size();
    qDebug() << "dump " << size << " bytes";
    for (int i = 0; i < size; i++) {
        printf("%02x ", (unsigned char) data[i]);
        if (i > 1 && (i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}
#endif

namespace QTerm
{
SSH2InBuffer::SSH2InBuffer(SocketPrivate * plainSocket, QObject * parent)
        : QObject(parent), m_in(), m_out(), m_buf(this),
        m_sequenceNumber(0),m_incompletePacket()
{
    m_buf.setBuffer(&m_out);
    m_buf.open(QBuffer::ReadWrite);
    m_socket = plainSocket;
    m_transport = NULL;
    if (m_socket != NULL)
        connect(m_socket, SIGNAL(readyRead()), this, SLOT(parseData()));
}

SSH2InBuffer::~SSH2InBuffer()
{}

void SSH2InBuffer::parseData()
{
    if (m_socket == NULL)
        return;
    int blockSize = m_transport == NULL ? 8 : m_transport->blockSize();
    int macLen = m_transport == NULL ? 0 : m_transport->macLen();
    qint64 nbyte = m_socket->bytesAvailable();
    QByteArray from_socket = m_socket->readBlock(nbyte);

    // Add data into the buffer
    m_in.append(from_socket);

    // Parse packet
    while (m_in.size() > 8) {

//   qDebug() << n << "bytes available";

        // ----------------------------------------------------------------------
        // | n = packet length | p = padding length |  payload  | padding | mac |
        // ----------------------------------------------------------------------
        // |         4         |          1         | n - p - 5 |    p    |     |
        // ----------------------------------------------------------------------

        QByteArray firstBlock;
        if (m_incompletePacket.isEmpty()) {
            if (m_transport != NULL)
                firstBlock = m_transport->crypt(m_in.left(blockSize));
            else
                firstBlock = m_in.left(blockSize);
        } else {
            firstBlock = m_incompletePacket;
        }
#ifdef SSH_DEBUG
        dumpData(firstBlock);
#endif
        // TODO: sanity check for packet length
        int length = get_u32(firstBlock.data());
        uint8_t padding = firstBlock[4];

//   qDebug() << "packet length " << length << " padding length " << padding;

        // TODO: die gracefully
        if (length < 0) {
            qDebug("something is wrong");
            emit error("SSH2InBuffer: negative packet length");
            return;
        }
        if (length > m_in.size()) {
            qDebug("packet not complete");
            m_incompletePacket = firstBlock;
            return;
        }

        // Get the total encrypted data
        QByteArray source;
        if ((length + 4) > blockSize) {
            if (m_transport != NULL)
                source = m_transport->crypt(m_in.mid(blockSize, length + 4 - blockSize));
            else
                source = m_in.mid(blockSize, length + 4 - blockSize);
        }

        QByteArray target = firstBlock + source;
        if (m_transport != NULL) {
            QByteArray tmp(4, 0);
            put_u32(tmp.data(), m_sequenceNumber);
            QByteArray cMac = m_transport->mac(tmp + target);
            QByteArray sMac = m_in.mid(length + 4, macLen);
            // FIXME: failure handler
            if (cMac != sMac)
                qDebug("mac does not match");
        }

        m_out = target.mid(5, length - padding - 1);
        m_buf.reset();
        m_incompletePacket.clear();
        int flag = m_out[0];
        // TODO: remove the mac;
        if (m_transport != NULL)
            m_in.remove(0, length + 4 + macLen);
        else
            m_in.remove(0, length + 4);

//   qDebug() << "Read " << nread << "bytes data";
//   dumpData ( m_out );

        m_sequenceNumber++;
        emit packetReady(flag);
    }
}

uint32_t SSH2InBuffer::getUInt32()
{
    char tmp[4];
    m_buf.read(tmp, 4);
    return get_u32(tmp);
}

uint8_t SSH2InBuffer::getUInt8()
{
    uint8_t v;
    m_buf.getChar((char*) &v);
    return v;
}

QByteArray SSH2InBuffer::getData(uint32_t size)
{
    QByteArray data(size, 0);
    m_buf.read(data.data(), size);
    return data;
}

QByteArray SSH2InBuffer::getString()
{
    uint32_t length = getUInt32();
    return getData(length);
}

void SSH2InBuffer::getBN(BIGNUM * value)
{
    QByteArray buf = getString();

    if (buf.size() > 0 && (buf[0] & 0x80)) {
        qDebug("getBN: negative numbers not supported");
        return;
    }

    if (buf.size() > 8 * 1024) {
        qDebug("getBN: cannot handle BN of size: %d", buf.size());
        return;
    }

    if (BN_bin2bn((const unsigned char *) buf.data(), buf.size(), value) == NULL) {
        qDebug("getBN: BN_bin2bn failed");
        return;
    }
}

bool SSH2InBuffer::atEnd()
{
    if (m_buf.bytesAvailable() > 0) {
        qDebug("WARNING: extra data");
        return false;
    }
#ifdef SSH_DEBUG
    qDebug() << "At the end of the buffer";
#endif
    return true;
}

QByteArray & SSH2InBuffer::buffer()
{
    return m_out;
}

SSH1InBuffer::SSH1InBuffer(SocketPrivate * plainSocket, QObject * parent)
        : QObject(parent), m_in(), m_out(), m_buf(this)
{
    m_buf.setBuffer(&m_out);
    m_buf.open(QBuffer::ReadWrite);
    m_socket = plainSocket;
    m_encryption = NULL;
    if (m_socket != NULL)
        connect(m_socket, SIGNAL(readyRead()), this, SLOT(parseData()));
}

SSH1InBuffer::~SSH1InBuffer()
{}

uint32_t SSH1InBuffer::getUInt32()
{
    char tmp[4];
    m_buf.read(tmp, 4);
    return get_u32(tmp);
}

uint8_t SSH1InBuffer::getUInt8()
{
    uint8_t v;
    m_buf.getChar((char*) &v);
    return v;
}

QByteArray SSH1InBuffer::getData(uint32_t size)
{
    QByteArray data(size, 0);
    m_buf.read(data.data(), size);
    return data;
}

QByteArray SSH1InBuffer::getString()
{
    uint32_t length = getUInt32();
    return getData(length);
}

void SSH1InBuffer::getBN(BIGNUM * value)
{
    char tmp[2];
    m_buf.read(tmp, 2);
    int bits = get_u16(tmp);
    int bytes = (bits + 7) / 8;
    QByteArray buf = getData(bytes);

#ifdef SSH_DEBUG
    qDebug() << "BN length: " << bytes;
#endif
//  if ( buf.size() > 0 && ( buf[0] & 0x80 ) ) {
//   qDebug() << "getBN: negative numbers not supported";
//   return;
//  }

    if (buf.size() > 8 * 1024) {
        qDebug("getBN: cannot handle BN of size: %d", buf.size());
        return;
    }

    if (BN_bin2bn((const unsigned char *) buf.data(), buf.size(), value) == NULL) {
        qDebug("getBN: BN_bin2bn failed");
        return;
    }
}

bool SSH1InBuffer::atEnd()
{
    if (m_buf.bytesAvailable() > 0) {
        qDebug("WARNING: extra data");
        return false;
    }
#ifdef SSH_DEBUG
    qDebug() << "At the end of the buffer";
#endif
    return true;
}

QByteArray & SSH1InBuffer::buffer()
{
    return m_out;
}

void SSH1InBuffer::parseData()
{
    if (m_socket == NULL)
        return;
//  int blockSize = m_transport == NULL ? 8 : m_transport->blockSize();
//  int macLen = m_transport == NULL ? 0 : m_transport->macLen();
    qint64 nbyte = 0;
    while ((nbyte = m_socket->bytesAvailable()) > 0) {
        QByteArray from_socket = m_socket->readBlock(nbyte);

        // Add data into the buffer
        m_in.append(from_socket);
//      dumpData ( m_in );

        // -------------------------------------------------------
        // | n = packet length | p = padding |  payload  | crc32 |
        // -------------------------------------------------------
        // |         4         |    1 - 7    |   n - 4   |   4   |
        // -------------------------------------------------------
        // |                   |           total len             |
        // -------------------------------------------------------
        while (m_in.size() > 4) {
            int length = get_u32(m_in.data());
            int totalLen = (length + 8) & ~7;
            int padding = totalLen - length;
#ifdef SSH_DEBUG
            qDebug() << "length: " << length << "total length: " << totalLen << "padding: " << padding;
#endif

            if (totalLen > m_in.size()) {
                qDebug("packet not complete");
                return;
            }

            QByteArray plain;
//      qDebug() << "Encrypted data without the length";
//      dumpData(m_in.mid(4,totalLen + 4));
            if (m_encryption == NULL)
                plain = m_in.mid(4, totalLen);
            else
                plain = m_encryption->crypt(m_in.mid(4, totalLen));
            // the Padding, Packet type, and Data fields
            uint32_t mycrc = ssh_crc32((unsigned char *) plain.left(totalLen - 4).data(), totalLen - 4);
            m_out = plain.mid(padding, length - 4);
#ifdef SSH_DEBUG
            qDebug() << "Decrypted data";
#endif
//      dumpData ( m_out );
//      dumpData ( m_in.mid ( totalLen, 4 ) );
            uint32_t gotcrc = get_u32(plain.mid(totalLen - 4, 4));
            if (gotcrc != mycrc)
                // TODO: die gracefully
                qDebug("crc32 check error!");
            m_buf.reset();
            int flag = m_out[0];
            m_in.remove(0, totalLen + 4);
            emit packetReady(flag);
        }
    }
}

SSH2OutBuffer::SSH2OutBuffer(SocketPrivate * plainSocket, QObject * parent)
        : QObject(parent), m_in(), m_buf(this),
        m_sequenceNumber(0)
{
    m_buf.setBuffer(&m_in);
    m_buf.open(QBuffer::ReadWrite);
    m_transport = NULL;
    m_socket = plainSocket;
}

SSH2OutBuffer::~SSH2OutBuffer()
{}

void SSH2OutBuffer::startPacket()
{
    m_in.resize(0);
    m_buf.reset();
}

void SSH2OutBuffer::startPacket(uint8_t flag)
{
    startPacket();
    putUInt8(flag);
}

void SSH2OutBuffer::putUInt32(uint32_t v)
{
    char tmp[4];
    put_u32(tmp, v);
    m_buf.write(tmp, 4);
}

void SSH2OutBuffer::putUInt8(uint8_t v)
{
    m_buf.putChar(v);
}

void SSH2OutBuffer::putData(const QByteArray & data)
{
    m_buf.write(data);
}

void SSH2OutBuffer::putString(const QByteArray & string)
{
    putUInt32(string.size());
    if (string.size() == 0)
        return;
    else
        putData(string);
}

void SSH2OutBuffer::putBN(const BIGNUM * value)
{
    u_int bytes;
    int oi;
    u_int hasnohigh = 0;

    if (BN_is_zero(value)) {
        putUInt32(0);
        return;
    }

    if (value->neg) {
        qDebug("putBN: negative numbers not supported");
        return;
    }
    bytes = BN_num_bytes(value) + 1; /* extra padding byte */
    if (bytes < 2) {
        qDebug("putBN: BN too small");
        return;
    }
    QByteArray buf(bytes - 1, 0);
//   buf[0] = 0x00;
    /* Get the value of in binary */
    oi = BN_bn2bin(value, (unsigned char *) buf.data());
    buf.insert(0, (char) 0);
    if (oi < 0 || (u_int) oi != bytes - 1) {
        qDebug("buffer_put_bignum2_ret: BN_bn2bin() failed: oi != bin_size");
        return;
    }
    hasnohigh = (buf[1] & 0x80) ? 0 : 1;
    putString(buf.mid(hasnohigh));
    return;
}

void SSH2OutBuffer::sendPacket()
{
    int blockSize;
    if (m_transport != NULL)
        blockSize = m_transport->blockSize();
    else
        blockSize = 8;
    int len = m_in.size();
#ifdef SSH_DEBUG
    qDebug() << "len: " << len;
#endif
    int totalLen;
    QByteArray tmp(4, 0);
    uint8_t padding = blockSize - ((len + 5) % blockSize);
    if (padding < 4)
        padding += blockSize;
#ifdef SSH_DEBUG
    qDebug() << "padding: " << padding << " bytes";
#endif
    QByteArray paddingData(padding,0);
    if (m_transport != NULL)
        RAND_bytes((unsigned char *) paddingData.data(), padding);
    QByteArray plain = m_in+paddingData;
    totalLen = plain.size() + 1;
    plain.insert(0, padding);
    put_u32(tmp.data(), totalLen);
    plain.insert(0, tmp);
//   for ( int i = 3; i >= 0; i-- )
//    packet.insert ( 0, ( char ) tmp[i] );
    //dumpData ( packet );
    QByteArray packet;
    if (m_transport != NULL) {
        packet = m_transport->crypt(plain);
        put_u32(tmp.data(), m_sequenceNumber);
        packet += m_transport->mac(tmp + plain);
    } else
        packet = plain;
    qint64 nwrite = m_socket->writeBlock(packet);
    m_sequenceNumber++;
    if (nwrite < packet.size())
        qDebug("packet write too small");
}

QByteArray & SSH2OutBuffer::buffer()
{
    return m_in;
}

SSH1OutBuffer::SSH1OutBuffer(SocketPrivate * plainSocket, QObject * parent)
        : QObject(parent), m_in(), m_buf(this)
{
    m_buf.setBuffer(&m_in);
    m_buf.open(QBuffer::ReadWrite);
    m_encryption = NULL;
    m_socket = plainSocket;
}

SSH1OutBuffer::~SSH1OutBuffer()
{}

void SSH1OutBuffer::startPacket()
{
    m_in.resize(0);
    m_buf.reset();
}

void SSH1OutBuffer::startPacket(uint8_t flag)
{
    startPacket();
    putUInt8(flag);
}

void SSH1OutBuffer::putUInt32(uint32_t v)
{
    char tmp[4];
    put_u32(tmp, v);
    m_buf.write(tmp, 4);
}

void SSH1OutBuffer::putUInt8(uint8_t v)
{
    m_buf.putChar(v);
}

void SSH1OutBuffer::putData(const QByteArray & data)
{
    m_buf.write(data);
}

void SSH1OutBuffer::putString(const QByteArray & string)
{
    putUInt32(string.size());
    if (string.size() == 0)
        return;
    else
        putData(string);
}

void SSH1OutBuffer::putBN(const BIGNUM * value)
{
    int bits = BN_num_bits(value);
    int bin_size = (bits + 7) / 8;
    QByteArray buf(bin_size, 0);
    int oi;
    char msg[2];

    /* Get the value of in binary */
    oi = BN_bn2bin(value, (uint8_t*) buf.data());
    if (oi != bin_size) {
        qDebug("BN_bn2bin() failed");
        return;
    }

    /* Store the number of bits in the buffer in two bytes, msb first. */
    put_u16(msg, bits);
    m_buf.write(msg, 2);
    /* Store the binary data. */
    putData(buf);
}

void SSH1OutBuffer::sendPacket()
{
    int len = m_in.size() + 4; // CRC32
    int padding = 8 - (len % 8);
#ifdef SSH_DEBUG
    qDebug() << len << padding;
#endif
    QByteArray tmp(padding, 0);
    if (m_encryption != NULL)
        RAND_bytes((unsigned char *) tmp.data(), padding);
    QByteArray plain = tmp + m_in;
    tmp.resize(4);
    uint32_t mycrc = ssh_crc32((unsigned char *) plain.data(), plain.size());
    put_u32(tmp.data(), mycrc);
//  dumpData(tmp);
    // TODO: encryption
    QByteArray packet;
    if (m_encryption == NULL)
        packet = plain + tmp;
    else
        packet = m_encryption->crypt(plain + tmp);

    tmp.resize(4);
    put_u32(tmp.data(), len);
    packet.insert(0, tmp);

    qint64 nwrite = m_socket->writeBlock(packet);
//  m_socket->flush();

    if (nwrite < packet.size())
        qDebug("packet write too small");
}

QByteArray & SSH1OutBuffer::buffer()
{
    return m_in;
}

}

#include "moc_packet.cpp"

