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

#ifdef HAVE_PYTHON
#include <Python.h>
#endif

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
#include <sys/stat.h>
#include <errno.h>
#include <locale.h>
#endif

#include <stdio.h>

using namespace QTerm;

#ifdef HAVE_PYTHON
PyThreadState * mainThreadState;
#endif //HAVE_PYTHON

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

    //qApp=&a;

    if( !Global::instance()->isOK() )
    {
        return -1;
    }

#ifdef HAVE_PYTHON
      // initialize Python
    Py_Initialize();
    // initialize thread support
    PyEval_InitThreads();
    mainThreadState = NULL;
    // save a pointer to the main PyThreadState object
    mainThreadState = PyThreadState_Get();


    // add path
    PyRun_SimpleString("import sys\n");
    QString pathCmd;
    // pathLib/script
    pathCmd = "sys.path.insert(0,'";
    pathCmd += Global::instance()->pathLib()+"script')";
    PyRun_SimpleString(strdup(pathCmd));

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
    // $HOME/.qterm/script, override
    pathCmd = "sys.path.insert(0,'";
    pathCmd += pathCfg+"script')";
    PyRun_SimpleString(strdup(pathCmd));
#endif
    // release the lock
    PyEval_ReleaseLock();
#endif // HAVE_PYTHON


    QTerm::Frame * mw = new QTerm::Frame();
    mw->setWindowTitle( "QTerm "+QString(QTERM_VERSION) );
    mw->setWindowIcon( QPixmap(Global::instance()->pathLib()+"pic/qterm.png") );
    //a.setMainWidget(mw);
    mw->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    int res = a.exec();

#ifdef HAVE_PYTHON
    // shut down the interpreter
    PyInterpreterState * mainInterpreterState = mainThreadState->interp;
    // create a thread state object for this thread
    PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
    PyThreadState_Swap(myThreadState);
    PyEval_AcquireLock();
    Py_Finalize();
#endif // HAVE_PYTHON
    return res;
}
