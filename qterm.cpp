#include "qterm.h"

#ifdef HAVE_PYTHON
#include <Python.h>
#endif

#include <QtCore>
#include <QtGui>

/******************************************************************************
 *
 *	Global Variables
 *
 *****************************************************************************/

QString pathSys;	// system-wide config files
QString pathUsr;	// user's config files

#ifdef HAVE_PYTHON
PyThreadState * mainThreadState;
#endif //HAVE_PYTHON


/******************************************************************************
 *
 *	Global Helper Functions
 *
 *****************************************************************************/

void runProgram(const QString& cmd)
{
	QString strCmd=cmd;
	#ifndef Q_OS_WIN32
	strCmd += " &";
	#endif
    system(strCmd.toLocal8Bit().data());
}

QString getOpenFileName(const QString& filter, QWidget * widget)
{
}

QString getSaveFileName(const QString& filename, QWidget* widget)
{
}

void clearDir( const QString& path )
{
}

int checkPath( const QString& path )
{
	QDir dir(path);
	if( ! dir.exists() )
	{
		if( !dir.mkdir(path) )
		{
			qWarning("Failed to create directory %s", path.toLocal8Bit().data());
			return -1;
		}
	}
	return 0;
}

int checkFile( const QString& dst, const QString& src )
{
	QDir dir;
	if(dir.exists(dst))
		return 0;
	
	if( !dir.exists(src) )
	{
		qCritical("QTerm failed to find %s.\n"
			"Please copy it from the source tarball to %s\n", 
			src.toLocal8Bit().data(), dst.toLocal8Bit().data() );
		return -1;
	}
		
	if ( !QFile::copy(src, dst) )
	{
		qCritical("QTerm failed to copy %s to %s. \n"
				"Please copy it manually. \n", 
			(const char*)src.toLocal8Bit(), (const char *)dst.toLocal8Bit());
		return -1;
	}
	return 0;
}

int iniWorkingDir()
{
#ifdef Q_OS_MACX
	pathUsr = QDir::homepath()+"/Library/QTerm/";
	if(checkPath(pathUsr)==-1)
		return -1;
	pathSys=QApplication::applicationDirPath()+"/";
#elif defined Q_OS_WIN32
	pathSys=QApplication::applicationDirPath()+"/";
	pathUsr=pathSys;
#else // Other *NIX system
	pathUsr=QDir::homePath()+"/.qterm/";
	if(checkPath(pathUsr)==-1)
		return -1;
//	pathSys=QTERM_DATADIR"/";
#endif

	// directories
	if(checkPath(pathUsr+"schema")==-1)
		return -1;
	
	// configuration files
	if(checkFile(pathUsr+"qterm.cfg",pathSys+"qterm.cfg")==-1)
		return -1;

	if(checkFile(pathUsr+"address.cfg",pathSys+"address.cfg")==-1)
		return -1;

	return 0;
}

int iniSettings()
{
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, pathUsr);
	return 0;
}

// Python related functions
int iniPython()
{
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
	pathCmd += pathSys+"script')";
	PyRun_SimpleString(strdup(pathCmd.toLocal8Bit().data()));

#ifndef Q_OS_WIN32
	// $HOME/.qterm/script, override 
	pathCmd = "sys.path.insert(0,'";
	pathCmd += pathUsr+"script')";
	PyRun_SimpleString(strdup(pathCmd.toLocal8Bit().data()));
#endif	
	
	// release the lock
	PyEval_ReleaseLock();

#endif // HAVE_PYTHON
}

int finPython()
{	
#ifdef HAVE_PYTHON
	// shut down the interpreter
	PyInterpreterState * mainInterpreterState = mainThreadState->interp;
	// create a thread state object for this thread
	PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
	PyThreadState_Swap(myThreadState);
	PyEval_AcquireLock();
	Py_Finalize();
#endif // HAVE_PYTHON
}
