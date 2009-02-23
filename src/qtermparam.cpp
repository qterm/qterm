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
	m_strName = "SMTH";
	m_strAddr = "smth.org";
	m_uPort = 23;
	m_nHostType = 0;		// 0--BBS 1--*NIX	
	m_bAutoLogin = false;
	m_strPreLogin = "";
	m_strUser = "";
	m_strPasswd = "";
	m_strPostLogin = "";
// Display
	m_BBSCode = "UTF-8";
	m_nDispCode = 0;
	m_bAutoFont = 1;
	m_bAlwaysHighlight = 0;
	m_bAnsiColor = 1;
	m_strFontName = "Nsimsun";
	m_nFontSize = 14;
	m_clrBg = QColor(0,0,0);
	m_clrFg = QColor(198,195,198);
	m_strSchemaFile = "";
// Terminal
	m_strTerm = "vt102";
	m_nKey = 0;
	m_nCol = 80;
	m_nRow = 24;
	m_nScrollLines = 240;
	m_nCursorType = 0; // 0--Block 1--Underline 2--I Type
	m_strEscape = "^[^[[";
// Connection
	m_nProxyType = 0; // 0--None 1--Wingate 2--SOCKS4 3--SOCKS5 4--HTTP
	m_strProxyHost = "";
	m_uProxyPort = 0;
	m_bAuth = false;
	m_strProxyUser = "";
	m_strProxyPasswd = "";
	m_nProtocolType = 0; // 0--Telnet 1--SSH1 2--SSH2
// Misc
	m_nMaxIdle = 180;
	m_strReplyKey = "^Z";
	m_strAntiString = "^@";
	m_bAutoReply = false;
	m_strAutoReply = "(QTerm) Sorry, I am not around";
	m_bReconnect = false;
	m_nReconnectInterval = 3;
	m_nRetry = 0;
	m_bLoadScript = false;
	m_strScriptFile = "";
// Mouse
	m_nMenuType=2;
	m_clrMenu = QColor(0,65,132);
}

Param::Param( const Param & param )
{
	m_strName = param.m_strName;
	m_strAddr = param.m_strAddr;
	m_uPort = param.m_uPort;
	m_nHostType = param.m_nHostType; // 0--BBS 1--*NIX
	m_bAutoLogin = param.m_bAutoLogin;
	m_strPreLogin = param.m_strPreLogin;
	m_strUser = param.m_strUser;
	m_strPasswd = param.m_strPasswd;
	m_strPostLogin = param.m_strPostLogin;
// Display
	m_BBSCode = param.m_BBSCode;
	m_nDispCode = param.m_nDispCode; 
	m_bAutoFont = param.m_bAutoFont;
	m_bAlwaysHighlight = param.m_bAlwaysHighlight;
	m_bAnsiColor = param.m_bAnsiColor;
	m_strFontName = param.m_strFontName;
	m_nFontSize = param.m_nFontSize;
	m_clrBg = param.m_clrBg;
	m_clrFg = param.m_clrFg;
	m_strSchemaFile = param.m_strSchemaFile;
// Terminal
	m_strTerm = param.m_strTerm;
	m_nKey = param.m_nKey;
	m_nCol = param.m_nCol;
	m_nRow = param.m_nRow;
	m_nScrollLines = param.m_nScrollLines;
	m_nCursorType = param.m_nCursorType; // 0--Block 1--Underline 2--I Type
	m_strEscape = param.m_strEscape;	// 0--ESC ESC 1--Ctrl+u
// Connection
	m_nProxyType = param.m_nProxyType; // 0--None 1--Wingate 2--SOCKS4 3--SOCKS5 4--HTTP
	m_strProxyHost = param.m_strProxyHost;
	m_uProxyPort = param.m_uProxyPort;
	m_bAuth = param.m_bAuth;
	m_strProxyUser = param.m_strProxyUser;
	m_strProxyPasswd = param.m_strProxyPasswd;
	m_nProtocolType = param.m_nProtocolType; // 0--Telnet 1--SSH1 2--SSH2
// Misc
	m_nMaxIdle = param.m_nMaxIdle;
	m_strReplyKey = param.m_strReplyKey;
	m_strAntiString = param.m_strAntiString;
	m_bAutoReply = param.m_bAutoReply;
	m_strAutoReply = param.m_strAutoReply;
	m_bReconnect = param.m_bReconnect;
	m_nReconnectInterval = param.m_nReconnectInterval;
	m_nRetry = param.m_nRetry;
	m_bLoadScript = param.m_bLoadScript;
	m_strScriptFile = param.m_strScriptFile;
// Mouse
	m_nMenuType = param.m_nMenuType;
	m_clrMenu = param.m_clrMenu;
}

Param::~Param()
{
}

Param& Param::operator=(const Param& param)
{
	m_strName = param.m_strName;
	m_strAddr = param.m_strAddr;
	m_uPort = param.m_uPort;
	m_nHostType = param.m_nHostType; // 0--BBS 1--*NIX
	m_bAutoLogin = param.m_bAutoLogin;
	m_strPreLogin = param.m_strPreLogin;
	m_strUser = param.m_strUser;
	m_strPasswd = param.m_strPasswd;
	m_strPostLogin = param.m_strPostLogin;
// Display
	m_BBSCode = param.m_BBSCode; 
	m_nDispCode = param.m_nDispCode; 
	m_bAutoFont = param.m_bAutoFont;
	m_bAlwaysHighlight = param.m_bAlwaysHighlight;
	m_bAnsiColor = param.m_bAnsiColor;
	m_strFontName = param.m_strFontName;
	m_nFontSize = param.m_nFontSize;
	m_clrBg = param.m_clrBg;
	m_clrFg = param.m_clrFg;
	m_strSchemaFile = param.m_strSchemaFile;
// Terminal
	m_strTerm = param.m_strTerm;
	m_nKey = param.m_nKey;
	m_nCol = param.m_nCol;
	m_nRow = param.m_nRow;
	m_nScrollLines = param.m_nScrollLines;
	m_nCursorType = param.m_nCursorType; // 0--Block 1--Underline 2--I Type
	m_strEscape = param.m_strEscape;	// 0--ESC ESC 1--Ctrl+u
// Connection
	m_nProxyType = param.m_nProxyType; // 0--None 1--Wingate 2--SOCKS4 3--SOCKS5 4--HTTP
	m_strProxyHost = param.m_strProxyHost;
	m_uProxyPort = param.m_uProxyPort;
	m_bAuth = param.m_bAuth;
	m_strProxyUser = param.m_strProxyUser;
	m_strProxyPasswd = param.m_strProxyPasswd;
	m_nProtocolType = param.m_nProtocolType; // 0--Telnet 1--SSH1 2--SSH2
// Misc
	m_nMaxIdle = param.m_nMaxIdle;
	m_strReplyKey = param.m_strReplyKey;
	m_strAntiString = param.m_strAntiString;
	m_bAutoReply = param.m_bAutoReply;
	m_strAutoReply = param.m_strAutoReply;
	m_bReconnect = param.m_bReconnect;
	m_nReconnectInterval = param.m_nReconnectInterval;
	m_nRetry = param.m_nRetry;
	m_bLoadScript = param.m_bLoadScript;
	m_strScriptFile = param.m_strScriptFile;
// Mouse
	m_nMenuType = param.m_nMenuType;
	m_clrMenu = param.m_clrMenu;

	return *this;
}

} // namespace QTerm
