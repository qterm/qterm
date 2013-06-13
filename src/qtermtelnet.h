#ifndef QTERMTELNET_H
#define QTERMTELNET_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtNetwork/QAbstractSocket>

#ifndef u_char
#define u_char uchar
#endif

/* TELNET Command Codes: */
/* Hints: These NVT control characters are sent from client to server, So
         the client side will not receive these commands */
#define TCSB  (u_char)250 /* Start Subnegotiation  */
#define TCSE  (u_char)240 /* End Of Subnegotiation */
#define TCNOP  (u_char)241 /* No Operation   */
#define TCDM  (u_char)242 /* Data Mark (for Sync)  */
#define TCBRK  (u_char)243 /* NVT Character BRK  */
#define TCIP  (u_char)244 /* Interrupt Process  */
#define TCAO  (u_char)245 /* Abort Output   */
#define TCAYT  (u_char)246 /* "Are You There?" Function */
#define TCEC  (u_char)247 /* Erase Character  */
#define TCEL  (u_char)248 /* Erase Line   */
#define TCGA  (u_char)249 /* "Go Ahead" Function  */
#define TCWILL  (u_char)251 /* Desire/Confirm Will Do Option*/
#define TCWONT  (u_char)252 /* Refusal To Do Option  */
#define TCDO  (u_char)253 /* Request To Do Option  */
#define TCDONT  (u_char)254 /* Request NOT To Do Option */
#define TCIAC  (u_char)255 /* Interpret As Command Escape */

/* Telnet Option Codes: */
#define TOTXBINARY (u_char)  0 /* TRANSMIT-BINARY option */
#define TOECHO  (u_char)  1 /* ECHO Option   */
#define TONOGA  (u_char)  3 /* Suppress Go-Ahead Option */
#define TOTERMTYPE (u_char) 24 /* Terminal-Type Option  */
#define TONAWS  (u_char) 31 /* Window  Size */

/* Network Virtual Printer Special Characters: */
/* In normal situations, these characters will to translated into local
   control characters , then pass to upper layer term. But in our situation,
   we can pass them to term directly */

#define VPLF  '\n' /* Line Feed    */
#define VPCR  '\r' /* Carriage Return   */
#define VPBEL  '\a' /* Bell (attention signal)  */
#define VPBS  '\b' /* Back Space    */
#define VPHT  '\t' /* Horizontal Tab   */
#define VPVT  '\v' /* Vertical Tab    */
#define VPFF  '\f' /* Form Feed    */

/* Keyboard Command Characters: */

/* Option Subnegotiation Constants: */
#define TT_IS  0 /* TERMINAL_TYPE option "IS" command */
#define TT_SEND  1 /* TERMINAL_TYPE option "SEND" command */

/* Telnet Socket-Input FSM States: */
#define TSDATA   0 /* normal data processing  */
#define TSIAC   1 /* have seen IAC   */
#define TSWOPT   2 /* have seen IAC-{WILL/WONT}  */
#define TSDOPT   3 /* have seen IAC-{DO/DONT}  */
#define TSSUBNEG  4 /* have seen IAC-SB   */
#define TSSUBIAC  5 /* have seen IAC-SB-...-IAC  */

#define NTSTATES  6 /* # of TS* states   */


// Telnet Option Subnegotiation FSM States:
#define SS_START 0  // initial state
#define SS_TERMTYPE 1  // TERMINAL_TYPE option subnegotiation
#define SS_END  2  // state after all legal input
#define NSSTATES 3  // # of SS_* states 

#define FSINVALID 0xff  // an invalid state number  
#define NCHRS  256  // number of valid characters 
#define TCANY  (NCHRS+1) // match any character  

#define TINVALID 0xff  // an invalid transition index  

#define CTRL(c) ((c)&0x1f)

namespace QTerm
{
//  decleration
class Telnet;
// actionFunc is a pointer, point to a Telnet's func
typedef int (Telnet::*ptrActionFunc)(int c);

//fsm struct
struct fsm_trans {
    u_char ft_state;  // current state
    short ft_char;  // input character
    u_char ft_next;  // next state
    ptrActionFunc ft_action; // action to take
};


/*------------------------------------------------------------------------------
* Telnet class definition
*-------------------------------------------------------------------------------
*/

class Socket;
class HostInfo;

class Telnet : public QObject
{
    Q_OBJECT

public:
    Telnet(const QString & termtype, int rows, int columns, bool isSSH) ;
    ~Telnet();

    void setProxy(int nProxyType, //0-no proxy; 1-wingate; 2-sock4; 3-socks5
                  bool bAuth, // if authentation needed
                  const QString& strProxyHost, quint16 uProxyPort,
                  const QString& strProxyUsr, const QString& strProxyPwd);
    void connectHost(HostInfo * hostInfo);
    int  read(char * data, uint maxlen);
    int  write(const char * data, uint len);
    void close();    // User close the connection

    int raw_len();
    int read_raw(char *data, uint maxlen);

public slots:
    void windowSizeChanged(int, int);

signals:
    void readyRead(int);  // There are datas to be read out
    void TelnetState(int);  // The  state telnet, defined as TSXXXX in qterm.h
private slots:
    void connected();
    void socketReadyRead();
    void showError(QAbstractSocket::SocketError);
    void hostFound();
    void delayCloseFinished();
    void closed();
protected:
    //init structure fsm
    void init_telnet();
    void fsmbuild();
    void fsminit(u_char fsm[][NCHRS], struct fsm_trans ttab[], int nstates);

    //actions
    int tcdm(int);
    int recopt(int);
    int no_op(int);
    int do_echo(int);
    int do_notsup(int);
    int do_noga(int);
    int do_txbinary(int);
    int will_notsup(int);
    int will_txbinary(int);
    int will_termtype(int);
    int will_naws(int);
    int subopt(int);
    int subtermtype(int);
    int subend(int);
    int soputc(int);
    int ttputc(int);
    int tnabort(int);

    //utility functions
    int xputc_up(char);
    int xputs_up(char *);
    void putc_down(u_char);


private:
    // Boolean Flags
    char synching, doecho, sndbinary, rcvbinary;
    char    noga;
    char naws;
    u_char option_cmd; // has value WILL, WONT, DO, or DONT

    char termtype; // non-zero if received "DO TERMTYPE"
    QByteArray term;  // terminal name

    /* // BBS don't need control signals
           // Special keys - Terminal control characters
    // need work...
    static const  char t_flushc=CTRL('S');  // Abort Output  i.e:(^S)
    static const  char  t_intrc=CTRL('C');  // Interrupt  i.e:(^C)
    static const  char t_quitc=CTRL('\\');  // Quit   i.e:(^\)
    static const  char sg_erase=CTRL('?');  // Erase a character  i.e:(^?)
    static const  char sg_kill=CTRL('U');  // Kill a line   i.e:(^U)
           */

    // FSM stuffs
    static struct fsm_trans ttstab[];
    int ttstate;
    u_char ttfsm[NTSTATES][NCHRS];

    static struct fsm_trans substab[];
    int substate;
    u_char subfsm[NSSTATES][NCHRS];

    // socket stuffs
    Socket *socket;

    //Pointers to internal buffers
    //
    //             |-->from_socket-->process-->to_ansi-->|
    //  socket<--->                                      <---> ansi decode
    //             |<---to_socket<--process<--from_ansi--|
    //
    QByteArray from_socket, to_ansi, from_ansi, to_socket;
    uint rsize; // size of to_ansi buffer
    uint wsize; // size of to_socket buffer


    // for test
    int wx, wy;
    int done_naws;
    bool d_isSSH;
    bool bConnected;
    uint raw_size;
};

} // namespace QTerm

#endif // QTERMTELNET_H

