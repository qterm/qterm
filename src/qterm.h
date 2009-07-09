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
#include "config.h"
#else
#define QTERM_VERSION   "0.5.6"
#endif

#ifndef BUFSIZE
#define BUFSIZE (1024)
#endif

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

#define DAE_FINISH  10001
#define DAE_TIMEOUT 10002

#define PYE_ERROR   10003
#define PYE_FINISH  10004

// some keys
#define CHAR_CR 	0x0d	// ^M
#define CHAR_LF		0x0a	// ^J
#define CHAR_FF 	0x0c	// ^L
#define CHAR_VT 	0x0b	// ^K
#define CHAR_TAB 	0x09	// ^I
#define CHAR_BS		0x08	// ^H
#define CHAR_BELL	0x07	// ^G
#define CHAR_ESC	0x1b	//

#define CHAR_NORMAL	-1

// telnet state
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

// proxy type
#define NOPROXY			0
#define WINGATE			1
#define SOCKS4			2
#define SOCKS5			3
#define HTTP			4

#endif	//QTERM_H

