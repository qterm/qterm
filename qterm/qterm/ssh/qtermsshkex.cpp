#include "qtermsshkex.h"
#include "qtermsshmd5.h"

//==============================================================================
//QTermSSH1Kex
//==============================================================================

QTermSSH1Kex::QTermSSH1Kex()
{
	d_first_kex = true;
	d_state = QTermSSH1Kex::BEFORE_PUBLICKEY;
}

QTermSSH1Kex::~QTermSSH1Kex()
{
}

void QTermSSH1Kex::initKex(QTermSSHPacketReceiver * packet, QTermSSHPacketSender * output)
{
	d_incomingPacket = packet;
	d_outcomingPacket = output;
	d_incomingPacket->disconnect(this);
	connect(d_incomingPacket, SIGNAL(packetAvaliable(int)), this, SLOT(handlePacket(int)));
	d_state = QTermSSH1Kex::BEFORE_PUBLICKEY;
	emit reKex();
}
void QTermSSH1Kex::handlePacket(int type)
{
	switch (d_state) {
		case QTermSSH1Kex::BEFORE_PUBLICKEY:
			makeSessionKey();
			d_state = QTermSSH1Kex::SESSIONKEY_SENT;
			break;
		case QTermSSH1Kex::SESSIONKEY_SENT:
			if (type != SSH1_SMSG_SUCCESS) {
				emit kexError("Kex exchange failed!");
				break;
			}
			emit kexOK();
			d_state = QTermSSH1Kex::KEYEX_OK;
			break;
		case QTermSSH1Kex::KEYEX_OK:
			break;
		default:
			return;
	}
}
			
void QTermSSH1Kex::makeSessionKey()
{
	int i;
	BIGNUM * key;
	u_int32_t rand;
	int bits;
	int rbits;
	
	if (d_incomingPacket->packetType() != SSH1_SMSG_PUBLIC_KEY) {
		emit kexError("startKex: First packet is not public key");
		return;
	}
	d_incomingPacket->getBuffer((char *) d_cookie, 8);

	// Get the public key.
	d_servKey = new QTermSSHRSA;
	bits = d_incomingPacket->getInt();
	d_incomingPacket->getBN(d_servKey->d_rsa->e);
	d_incomingPacket->getBN(d_servKey->d_rsa->n);

	rbits = BN_num_bits(d_servKey->d_rsa->n);
	if (bits != rbits) {
		qDebug("Warning: Server lies about size of server public key: actual size: %d vs. anounced: %d\n", rbits, bits);
		qDebug("Warning: This may be due to an old implementation of ssh.\n");
	}

	// Get the host key.
	d_hostKey = new QTermSSHRSA;
	bits = d_incomingPacket->getInt();
	d_incomingPacket->getBN(d_hostKey->d_rsa->e);
	d_incomingPacket->getBN(d_hostKey->d_rsa->n);

	rbits = BN_num_bits(d_hostKey->d_rsa->n);
	if (bits != rbits) {
		qDebug("Warning: Server lies about size of server public key: actual size: %d vs. anounced: %d\n", rbits, bits);
		qDebug("Warning: This may be due to an old implementation of ssh.\n");
	}

	// Get protocol flags.
	d_servFlag = d_incomingPacket->getInt();
	d_sciphers = d_incomingPacket->getInt();
	d_sauth = d_incomingPacket->getInt();
	
	// Debug.
	// qDebug("Server flags: %d, %d, %d\n", d_servFlag, d_sciphers, d_sauth);

	if ((d_sciphers & (1 << SSH_CIPHER_3DES)) == 0)
		qDebug("server do not support my cipher");
	
	makeSessionId();
	
	// Generate an encryption key for the session. The key is a 256 bit
	// random number, interpreted as a 32-byte key, with the least
	// significant 8 bits being the first byte of the key.
	
	for (i = 0; i < 32; i++) {
		if (i % 4 == 0)
			rand = random();
		d_sessionkey[i] = (rand & 0xff);
		rand >>= 8;
	}

	key = BN_new();
	
	BN_set_word(key, 0);
	for (i = 0; i < 32; i++) {
		BN_lshift(key, key, 8);
		if (i < 16)
			BN_add_word(key, d_sessionkey[i]^d_sessionid[i]);
		else
			BN_add_word(key, d_sessionkey[i]);
	}

	if (BN_cmp(d_servKey->d_rsa->n, d_hostKey->d_rsa->n) < 0) {
		d_servKey->publicEncrypt(key, key);
		d_hostKey->publicEncrypt(key, key);
	}
	else {
		d_hostKey->publicEncrypt(key, key);
		d_servKey->publicEncrypt(key, key);
	}

	delete d_hostKey;
	delete d_servKey;

	d_outcomingPacket->startPacket(SSH1_CMSG_SESSION_KEY);
	d_outcomingPacket->putByte(SSH_CIPHER_3DES);
	d_outcomingPacket->putBuffer((const char *)d_cookie, 8);
	d_outcomingPacket->putBN(key);

	BN_free(key);

	d_outcomingPacket->putInt(1);
	d_outcomingPacket->write();
	
	emit startEncryption(d_sessionkey);
}

void QTermSSH1Kex::makeSessionId()
{
	u_char * p;
	QTermSSHMD5 * md5;
	int servlen, hostlen;

	md5 = new QTermSSHMD5;
	servlen = BN_num_bytes(d_servKey->d_rsa->n);
	hostlen = BN_num_bytes(d_hostKey->d_rsa->n);
	
	p = new u_char[servlen + hostlen];

	BN_bn2bin(d_hostKey->d_rsa->n, p);
	BN_bn2bin(d_servKey->d_rsa->n, p + hostlen);
	md5->update(p, servlen + hostlen);
	md5->update(d_cookie, 8);
	md5->final(d_sessionid);
	delete md5;
	delete [] p;
}
