/*******************************************************************************
FILENAME:	qterm.h
REVISION:	2001.8.12 first created.
         
AUTHOR:		smartfish kafa

*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/


#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#else 
#define VERSION	"0.3.7"
#define QTERM_DATADIR "/usr/share/qterm"
#define QTERM_BINDIR  "/usr/bin"
#endif

#ifndef BUFSIZE
#define BUFSIZE (1024)
#endif

//assume the sreen is 80*24
#define CHARNUM 80
#define LINENUM 24
#include <qtextcodec.h>

//code convert
#define G2U(s) ( QTextCodec::codecForName("GBK")->toUnicode(s) )
#define U2G(s) ( QTextCodec::codecForName("GBK")->fromUnicode(s) )
#define B2U(s) ( QTextCodec::codecForName("Big5")->toUnicode(s) )
#define U2B(s) ( QTextCodec::codecForName("Big5")->fromUnicode(s) )


// every character has a 16-bit description
// low 	 8 for colors
// hight 8 for attributes
#define COLORMASK 0x00ff
#define ATTRMASK  0xff00

// set
#define SETCOLOR(a) ( (a) & COLORMASK )
#define SETATTR(a) ( ( (a) << 8 ) & ATTRMASK )
// get
#define GETCOLOR(a) ( (a) & COLORMASK )
#define GETATTR(a) ( ( (a) & ATTRMASK ) >> 8 )

// color 8-bit
// 0-2	background
//	0	black
//	1	red
//	2	green
//	3	yellow
//	4	blue
//	5	magenta
//	6	cyan
//	7	white
// 4-6	foreground
//	0	black
//	1	red	
//	2	green
//	3	yellow
//	4	blue
//	5	magenta
//	6	cyan
//	7	white
// 3	highlight
// 	0	no
// 	1	highlight
#define FGMASK 0x0f
#define BGMASK 0xf0
#define HIGHLIGHTMASK 0x08

//set color
#define SETFG(c) 	( (c) & FGMASK )
#define SETBG(c) 	( ( (c) << 4 ) & BGMASK )
#define SETHIGHLIGHT(c) ( (c) | HIGHLIGHTMASK );
//get color
#define GETFG(c) 	( (c) & FGMASK )
#define GETBG(c) 	( ( (c) & BGMASK ) >> 4 )
#define GETHIGHLIGHT(c) ( (c) & HIGHLIGHTMASK )

//reverse bg and fg color
#define REVERSECOLOR(c) ( SETBG( GETFG(c) ) | SETFG( GETBG(c) ) | GETHIGHLIGHT(c) )

// mask for attr
#define BOLDMASK	0x01
#define DIMMASK		0x02
#define UNDERLINEMASK	0x08
#define BLINKMASK	0x10
#define RAPIDBLINKMASK 	0x20
#define REVERSEMASK	0x40
#define INVISIBLEMASK	0x80

//set attributes
#define SETBOLD(a) 	( (a) | BOLDMASK )
#define SETDIM(a) 	( (a) | DIMMASK )
#define SETUNDERLINE(a) ( (a) | UNDERLINEMASK )
#define SETBLINK(a) 	( (a) | BLINKMASK )
#define SETRAPIDBLINK(a) ( (a) | RAPIDBLINKMASK )
#define SETREVERSE(a) 	( (a) | REVERSEMASK )
#define SETINVISIBLE(a) ( (a) | INVISIBLEMASK )

//get attributes
#define GETBOLD(a) 	( (a) & BOLDMASK )
#define GETDIM(a) 	( (a) & BLINKMASK )
#define GETUNDERLINE(a) ( (a) & UNDERLINEMASK )
#define GETBLINK(a) 	( (a) & BLINKMASK )
#define GETRAPIDBLINK(a) ( (a) & RAPIDBLINKMASK )
#define GETREVERSE(a) 	( (a) & REVERSEMASK )
#define GETINVISIBLE(a) ( (a) & INVISIBLEMASK )

//default color 
#define NO_COLOR ( SETFG(7) | SETBG(0) )
//default attribute
#define NO_ATTR 0x04

// other definations
#ifndef NULL
#define NULL 0
#endif

// some keys
#define CHAR_CR 	0x0d	// ^M
#define CHAR_LF		0x0a	// ^J
#define CHAR_FF 	0x0c	// ^L
#define CHAR_TAB 	0x09	// ^I
#define CHAR_BS		0x08	// ^H
#define CHAR_BELL	0x07	// ^G
#define CHAR_ESC	0x1b	//

#define CHAR_NORMAL	-1

#define TSRESOLVING		30
#define TSHOSTFOUND		31
#define TSHOSTNOTFOUND	32
#define TSCONNECTING	33
#define TSHOSTCONNECTED	34
#define TSPROXYCONNECTED	35
#define TSPROXYAUTH		36
#define TSPROXYFAIL		38
#define TSREFUSED		39
#define TSREADERROR		40
#define TSCLOSED		41
#define TSCLOSEFINISH	42
#define TSCONNECTVIAPROXY	43
#define TSEGETHOSTBYNAME	44
#define TSEINIWINSOCK	45
#define TSERROR			46
#define TSPROXYERROR	47
#define	TSWRITED		48

#define NOPROXY			0
#define WINGATE			1
#define SOCKS4			2
#define SOCKS5			3
#define HTTP			4

// UI ID 
#define ID_FILE_CONNECT		0x00
#define ID_FILE_DISCONNECT	0x01
#define ID_FILE_ADDRESS		0x02
#define ID_FILE_QUICK		0x03
#define ID_FILE_EXIT		0x04

#define ID_EDIT_COPY		0x10
#define ID_EDIT_PASTE		0x11
#define ID_EDIT_COLOR		0x12
#define	ID_EDIT_RECT		0x13
#define	ID_EDIT_AUTO		0x14
#define ID_EDIT_WW			0x15

#define ID_EDIT_ESC_NO		0x17
#define ID_EDIT_ESC_ESC		0x18
#define ID_EDIT_ESC_U		0x19
#define ID_EDIT_ESC_CUS		0x1a

#define ID_EDIT_CODEC_GBK	0x1b
#define ID_EDIT_CODEC_BIG5	0x1c


#define ID_VIEW_FONT		0x20
#define	ID_VIEW_COLOR		0x21
#define	ID_VIEW_REFRESH		0x22
#define	ID_VIEW_LANG		0x23
#define ID_VIEW_FULL		0x24
#define ID_VIEW_BOSS		0x25
#define ID_VIEW_SCROLL_HIDE	0x26
#define ID_VIEW_SCROLL_LEFT	0x27
#define ID_VIEW_SCROLL_RIGHT 0x28
#define ID_VIEW_SWITCH		0x29
#define ID_VIEW_STATUS		0x3a

#define	ID_OPTION_CURRENT	0x30
#define ID_OPTION_DEFAULT	0x31
#define ID_OPTION_PREF		0x32

#define ID_SPEC_ANTI		0x40
#define ID_SPEC_AUTO		0x41
#define	ID_SPEC_MESSAGE		0x42
#define ID_SPEC_BEEP		0x43
#define ID_SPEC_MOUSE		0x44
#define ID_SPEC_ARTICLE		0x45

#define ID_SCRIPT_RUN		0x50
#define ID_SCRIPT_STOP		0x51

#define ID_HELP_ABOUT		0x60
#define ID_HELP_HOMEPAGE	0x61


#endif	//QTERM_H

