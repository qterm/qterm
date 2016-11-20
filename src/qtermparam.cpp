/*******************************************************************
 * FILENAME:    qtermparam.cpp
 * REVISION:    2002.11.20 first created
 *
 * AUTHOR:  kingson
 ******************************************************************/
/******************************************************************
 *          NOTE
 * This file may be used, distributed and modified without limitation.
 *******************************************************************/
#include "qtermparam.h"
namespace QTerm
{
Param::Param()
{
	m_mapParam["name"] = "SMTH";
	m_mapParam["addr"] = "smth.org";
	m_mapParam["port"] = 23;
	m_mapParam["hosttype"] = 0; // 0--BBS 1--*NIX	
	m_mapParam["autologin"] = false;
	m_mapParam["prelogin"] = "";
	m_mapParam["user"] = "";
	m_mapParam["password"] = "";
	m_mapParam["postlogin"] = "";
	
	m_mapParam["bbscode"] = "GBK";
	m_mapParam["displaycode"] = 0;
	m_mapParam["autofont"] = true;
	m_mapParam["alwayshighlight"] = false;
	m_mapParam["ansicolor"] = true;
	m_mapParam["asciifont"] = "Monospace";
	m_mapParam["generalfont"] = "Monospace";
	m_mapParam["fontsize"] = 14;
	m_mapParam["schemefile"] = "";
	m_mapParam["opacity"] = 100;
	m_mapParam["blinkcursor"] = true;
	
	m_mapParam["termtype"] = "vt102";
	m_mapParam["keytype"] = 0;
	m_mapParam["keyboardprofile"] = "";
	m_mapParam["column"] = 80;
	m_mapParam["row"] = 24;
	m_mapParam["scroll"] = 240;
	m_mapParam["cursor"] = 0; // 0--Block 1--Underline 2--I Type
	m_mapParam["escape"] = "^[^[[";
	
	m_mapParam["protocol"] = 0; // 0--Telnet 1--SSH1 2--SSH2
	m_mapParam["proxytype"] = 0; // 0--None 1--Wingate 2--SOCKS4 3--SOCKS5 4--HTTP
	m_mapParam["proxyaddr"] = "";
	m_mapParam["proxyport"] = 0;
	m_mapParam["proxyauth"] = false;
	m_mapParam["proxyuser"] = "";
	m_mapParam["proxypassword"] = "";

	m_mapParam["maxidle"] = 180;
	m_mapParam["antiidlestring"] = "^@";
	m_mapParam["bautoreply"] = true;
	m_mapParam["replykey"] = "^Z";
	m_mapParam["autoreply"] = "(QTerm) Sorry, I am not around";
	m_mapParam["reconnect"] = false;
	m_mapParam["interval"] = 2;
	m_mapParam["retrytimes"] = -1;
	m_mapParam["loadscript"] = false;
	m_mapParam["scriptfile"] = "";

	m_mapParam["menutype"] = 2;
	m_mapParam["menucolor"] = QColor(0,65,132);
	m_mapParam["sshuser"] = "";
	m_mapParam["sshpass"] = "";
	m_mapParam["sshpublickeyfile"] = "";
	m_mapParam["sshprivatekeyfile"] = "";
	m_mapParam["sshpassphrase"] = "";
	m_mapParam["sshhostkey"] = "";

}

Param::Param( const Param & param )
{
	m_mapParam = param.m_mapParam;
}

Param::~Param()
{
}

Param& Param::operator=(const Param& param)
{
	m_mapParam = param.m_mapParam;
	return *this;
}

} // namespace QTerm
