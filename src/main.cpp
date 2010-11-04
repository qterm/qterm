/*******************************************************************************
FILENAME:      main.cpp
REVISION:      2001.10.4 first created.

AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/


// remove this when use configure

#include "uaocodec.h"
#include "qtermframe.h"
#include "qtermconfig.h"
#include "qtermglobal.h"
#include "qterm.h"

#include <QtGlobal>
#include <QApplication>

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
#include <sys/stat.h>
#include <errno.h>
#include <locale.h>
#endif

#include <stdio.h>

//==============================================================================
// Crash Handler: Save the backtrace info to /var/tmp as qterm.pid.timestamp
// The line number can be retrieved using addr2line
//==============================================================================

#ifdef Q_OS_LINUX
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <execinfo.h>
#include <signal.h>

#define QTERM_SIG_MAX_RETS 50
#define QTERM_SIG_LOG_DIR "/var/tmp"

// keep big data static to keep sig handler's stack usage to a minimum
static void *_rets[QTERM_SIG_MAX_RETS];
static char _sig_fname[PATH_MAX+1];
static char _buf[256];
void sig_fatal_init(int sig);
void sig_fatal_finish();
void sig_fatal_init();

void sig_fatal_handler (int sig)
{
    int num, fd, i;

    i = 0;

    sig_fatal_finish ();

    chdir (QTERM_SIG_LOG_DIR);

    if ((fd = creat (_sig_fname, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
        i = errno;
        fd = STDERR_FILENO;
    }

    num = backtrace (_rets, QTERM_SIG_MAX_RETS);

    sprintf (_buf, "Hit with signal %d! Stack trace of last %d functions:\n",
            sig, num);
    write (fd, _buf, strlen (_buf));

    backtrace_symbols_fd (_rets, num, fd);

    close (fd);

    abort ();
}

void sig_fatal_init ()
{
    time_t ut;

    time (&ut);
    strftime (_buf, sizeof (_buf), "%Y%m%d.%H%M%S", localtime (&ut));
    snprintf (_sig_fname, sizeof (_sig_fname), "%s/%s.%d.%s",
            QTERM_SIG_LOG_DIR, "qterm", getpid(), _buf);

    if (access (QTERM_SIG_LOG_DIR, F_OK))
        mkdir (QTERM_SIG_LOG_DIR, S_IRWXU | S_IRWXG | S_IRWXO);

    signal (SIGSEGV, sig_fatal_handler);
    signal (SIGBUS, sig_fatal_handler);
    signal (SIGILL, sig_fatal_handler);
    signal (SIGABRT, sig_fatal_handler);
    signal (SIGFPE, sig_fatal_handler);
}

void sig_fatal_finish ()
{
    signal (SIGSEGV, SIG_DFL);
    signal (SIGBUS, SIG_DFL);
    signal (SIGILL, SIG_DFL);
    signal (SIGABRT, SIG_DFL);
    signal (SIGFPE, SIG_DFL);
}

#endif //Q_OS_LINUX

using namespace QTerm;

#ifdef Q_WS_WIN
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
int main(int argc, char **argv);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                 LPSTR lpCmdLine, int nCmdShow)
{
  return main(__argc, __argv);
}
#endif

int main( int argc, char ** argv )
{
    if ( strcmp( qVersion(), QT_MIN_VERSION ) < 0 ) {
        qFatal("QTerm requires Qt %s or above to run! Qt installed in your system is %s.", QT_MIN_VERSION, qVersion());
    }
    //QApplication::setGraphicsSystem("raster");

    QApplication a( argc, argv );
    a.setApplicationName("QTerm");

#ifdef Q_OS_LINUX
    sig_fatal_init();
#endif

    if( !Global::instance()->isOK() )
    {
        return -1;
    }


    // Registrate the new codec
    UAOCodec hackCodec;

    QTerm::Frame * mw = new QTerm::Frame();
    mw->setWindowTitle( "QTerm "+QString(QTERM_VERSION) );
    mw->setWindowIcon( QIcon(":/pic/qterm.png") );
    mw->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    a.connect( &a, SIGNAL(commitDataRequest(QSessionManager &)), mw, SLOT(saveAndDisconnect()), Qt::DirectConnection);
    int res = a.exec();

    return res;
}
