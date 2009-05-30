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

#include "qtermframe.h"
#include "qtermconfig.h"
#include "qtermglobal.h"
#include "qterm.h"

#include <QApplication>

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
#include <sys/stat.h>
#include <errno.h>
#include <locale.h>
#endif

#include <stdio.h>

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

static void qtMessageHandler(QtMsgType type, const char *msg)
{
        fprintf(stderr, "%s\n", msg);
        if (type == QtFatalMsg)
                abort();
}

int main( int argc, char ** argv )
{

    QApplication a( argc, argv );
    a.setApplicationName("QTerm");
    qInstallMsgHandler(qtMessageHandler);

    if( !Global::instance()->isOK() )
    {
        return -1;
    }

    QTerm::Frame * mw = new QTerm::Frame();
    mw->setWindowTitle( "QTerm "+QString(QTERM_VERSION) );
    mw->setWindowIcon( QPixmap(Global::instance()->pathLib()+"pic/qterm.png") );
    mw->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    int res = a.exec();

    return res;
}
