#ifndef QTERMPARAM_H
#define QTERMPARAM_H

// #include <qstring.h>
// #include <qcolor.h>
#include <QString>
#include <QColor>
namespace QTerm
{
class Param;

class Param
{

public:
	Param();
	Param( const Param& );
	~Param();

	Param& operator=(const Param&);
// General
	// Name
	QString m_strName;
	// Address
	QString m_strAddr;
	// Port
	quint16 m_uPort;
	// Host Type	
	int m_nHostType;	// 0--BBS 1--*NIX
	// Auto Login
	bool m_bAutoLogin;
	// Pre Login
	QString m_strPreLogin;
	// User Name
	QString m_strUser;
	// Password
	QString m_strPasswd;
	// Post Login
	QString m_strPostLogin;
// Display
	 // 0--GBK  1--BGI5
	int m_nBBSCode; 
	// 0--GBK 1--BIG5
	int m_nDispCode; 
	// Auto Change Font When Window Resized
	bool m_bAutoFont;
	// Always Highlight
	bool m_bAlwaysHighlight;
	// ANSI Color
	bool m_bAnsiColor;
	// Font Name
	QString m_strFontName;
	// Font Size
	int m_nFontSize;
	// Background Color
	QColor m_clrBg;
	// Foreground Color
	QColor m_clrFg;
	// Schema File
	QString m_strSchemaFile;
// Terminal
	// Terminal Type
	QString m_strTerm;
	// Key Type
	int m_nKey;			// 0--BBS 1--XTERM 2--VT100
	// Columns & Rows
	int m_nCol, m_nRow;
	// Scroll Lines
	int m_nScrollLines;
	// Curor Type
	int m_nCursorType; // 0--Block 1--Underline 2--I Type
	// the esacpe string
	QString m_strEscape;
// Connection
	// Proxy Type
	int m_nProxyType; // 0--None 1--Wingate 2--SOCKS4 3--SOCKS5 4--HTTP
	// Address
	QString m_strProxyHost;
	// Port
	quint16 m_uProxyPort;
	// Authentation
	bool m_bAuth;
	// User Name
	QString m_strProxyUser;
	// Password
	QString m_strProxyPasswd;
	// Protocol
	int m_nProtocolType; // 0--Telnet 1--SSH1 2--SSH2
// Misc
	// Max Idle Time %s
	int m_nMaxIdle;
	QString m_strReplyKey;
	// Send When Idle
	QString m_strAntiString;
	// wether autoreply
	bool m_bAutoReply;
	// Auto Reply Messages
	QString m_strAutoReply;
	// Reconnect When Disconnected By Host
	bool m_bReconnect;
	// Reconnect Interval (s)
	int m_nReconnectInterval;
	// Retry times
	int m_nRetry;	// -1 -- infinite
// Mouse
	int m_nMenuType;	// 0--underline 1--reverse 2--color
	QColor m_clrMenu;
// Script
	bool m_bLoadScript;
	QString m_strScriptFile;
};

} // namespace QTerm

#endif // QTERMPARAM_H
