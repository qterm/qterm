#include <Python.h>
#include "qtermbuffer.h"
#include "qtermtelnet.h"
#include "qtermwindow.h"
#include "qtermtextline.h"

#include <qmessagebox.h>
#include <qpoint.h>

static int initialised = 0;

// copy current artcle
static PyObject *qterm_copyArticle(PyObject *, PyObject *args)
{
	long lp;
	int line;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;

	QCString cstr = ((QTermWindow*)lp)->downloadArticle();

	PyObject *py_text = PyString_FromString(cstr);

	Py_INCREF(py_text);
	return py_text;
}

static PyObject *qterm_delay(PyObject *, PyObject *args)
{
	int delay;
	
	if (!PyArg_ParseTuple(args, "i", &delay))
		return NULL;

//	qApp->processEvent(delay);

	Py_INCREF(Py_None);
	return Py_None;
}

// caret x
static PyObject *qterm_caretX(PyObject *, PyObject *args)
{
	long lp;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;
	int x = ((QTermWindow*)lp)->m_pBuffer->caret().x();
	PyObject * py_x =Py_BuildValue("i",x);
	Py_INCREF(py_x);
	return py_x;
}

// caret y
static PyObject *qterm_caretY(PyObject *, PyObject *args)
{
	long lp;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;
	int y = ((QTermWindow*)lp)->m_pBuffer->caret().y();
	PyObject * py_y =Py_BuildValue("i",y);
	Py_INCREF(py_y);
	return py_y;

}

// columns
static PyObject *qterm_columns(PyObject *, PyObject *args)
{
	long lp;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;

	int columns = ((QTermWindow*)lp)->m_pBuffer->columns();
	PyObject * py_columns = Py_BuildValue("i",columns);
	
	Py_INCREF(py_columns);
	return py_columns;

}

// rows
static PyObject *qterm_rows(PyObject *, PyObject *args)
{
	long lp;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;
	
	int rows = ((QTermWindow*)lp)->m_pBuffer->line();
	PyObject *py_rows = Py_BuildValue("i",rows);

	Py_INCREF(py_rows);
	return py_rows;
}

// sned string to server
static PyObject *qterm_sendString(PyObject *, PyObject *args)
{
	char *pstr;
	long lp;
	int len;

	if (!PyArg_ParseTuple(args, "ls", &lp, &pstr))
	{
		QMessageBox::information( NULL, "QTerm",
		"Failed To Initialized Python");
		return NULL;
	}
	len = strlen(pstr);

	((QTermWindow*)lp)->m_pTelnet->write(pstr,len);

	Py_INCREF(Py_None);
	return Py_None;
}

// same as above except parsing string first "\n" "^p" etc
static PyObject *qterm_sendParsedString(PyObject *, PyObject *args)
{
	char *pstr;
	long lp;
	int len;

	if (!PyArg_ParseTuple(args, "ls", &lp, &pstr))
		return NULL;
	len = strlen(pstr);
	
	((QTermWindow*)lp)->sendParsedString(pstr);

	Py_INCREF(Py_None);
	return Py_None;
}

// get text at line
static PyObject *qterm_getText(PyObject *, PyObject *args)
{
	long lp;
	int line;
	if (!PyArg_ParseTuple(args, "li", &lp, &line))
		return NULL;
	QCString cstr = ((QTermWindow*)lp)->m_pBuffer->screen(line)->getText();

	PyObject *py_text = PyString_FromString(cstr);

	Py_INCREF(py_text);
	return py_text;
}

// get text with attributes
static PyObject *qterm_getAttrText(PyObject *, PyObject *args)
{
	long lp;
	int line;
	if (!PyArg_ParseTuple(args, "li", &lp, &line))
		return NULL;

	QCString cstr = ((QTermWindow*)lp)->m_pBuffer->screen(line)->getAttrText();

	PyObject *py_text = PyString_FromString(cstr);

	Py_INCREF(py_text);
	return py_text;
}

static PyObject *qterm_isConnected(PyObject *, PyObject *args)
{
	long lp;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;
	
	bool connected = ((QTermWindow*)lp)->isConnected();
	PyObject * py_connected =Py_BuildValue("i",connected?1:0);

	Py_INCREF(py_connected);
	return py_connected;
}
static PyObject *qterm_disconnect(PyObject *, PyObject *args)
{
	long lp;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;
	
	((QTermWindow*)lp)->disconnect();
	
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *qterm_reconnect(PyObject *, PyObject *args)
{
	long lp;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;
	
	((QTermWindow*)lp)->reconnect();
	
	Py_INCREF(Py_None);
	return Py_None;
}


static struct PyMethodDef qterm_methods[] = {
	{"delay",			(PyCFunction)qterm_delay,				METH_VARARGS,	NULL},

	{"copyArticle",		(PyCFunction)qterm_copyArticle,			METH_VARARGS,	NULL},

	{"getText",			(PyCFunction)qterm_getText,				METH_VARARGS,	NULL},
	{"getAttrText",		(PyCFunction)qterm_getAttrText,			METH_VARARGS,	NULL},

	{"sendString",		(PyCFunction)qterm_sendString,			METH_VARARGS,	NULL},
	{"sendParsedString",(PyCFunction)qterm_sendParsedString,	METH_VARARGS,	NULL},

	{"caretX",			(PyCFunction)qterm_caretX,				METH_VARARGS,	NULL},
	{"caretY",			(PyCFunction)qterm_caretY,				METH_VARARGS,	NULL},

	{"columns",			(PyCFunction)qterm_columns,				METH_VARARGS,	NULL},
	{"rows",			(PyCFunction)qterm_rows,				METH_VARARGS,	NULL},
	
	{"isConnected",		(PyCFunction)qterm_isConnected,			METH_VARARGS,	NULL},
	{"disconnect",		(PyCFunction)qterm_disconnect,			METH_VARARGS,	NULL},
	{"reconnect",		(PyCFunction)qterm_reconnect,			METH_VARARGS,	NULL},

	{NULL,	 			(PyCFunction)NULL, 						0, 				NULL}
};


int Python_Init(void)
{

	PyObject *mod;
	PyObject *dict;

	if (initialised > 0)
		return 0;	// python interpreter has been inited 
 
	Py_Initialize();

	/* create the module and add the functions */		
	mod = Py_InitModule4("qterm", qterm_methods,
			NULL,(PyObject*)NULL,PYTHON_API_VERSION);
	
	if ( PyErr_Occurred() )
		return -1;	// init failed

	initialised ++;
				// init successfully
    return 0;
}		

int Python_Finalize(void)
{
	initialised --;
	if(initialised==0)
		Py_Finalize();
	return 0;
}
int DoPythonCommand( const char * cmd )
{

	PyRun_SimpleString( (char *)(cmd) );
	return 0; 
}


int DoPythonFile( const char * filename )
{
    static char buffer[1024];
    const char *file = filename;

    char *p;

    /* Have to do it like this. PyRun_SimpleFile requires you to pass a
     * stdio file pointer, but Vim and the Python DLL are compiled with
     * different options under Windows, meaning that stdio pointers aren't
     * compatible between the two. Yuk.
     *
     * Put the string "execfile('file')" into buffer. But, we need to
     * escape any backslashes or single quotes in the file name, so that
     * Python won't mangle the file name.
	 * ---- kafa
     */
    strcpy(buffer, "execfile('");
    p = buffer + 10; /* size of "execfile('" */

    while (*file && p < buffer + (1024 - 3))
    {
	if (*file == '\\' || *file == '\'')
	    *p++ = '\\';
	*p++ = *file++;
    }

    /* If we didn't finish the file name, we hit a buffer overflow */
    if (*file != '\0')
	return -1;

    /* Put in the terminating "')" and a null */
    *p++ = '\'';
    *p++ = ')';
    *p++ = '\0';

    /* Execute the file */
    return DoPythonCommand(buffer);
}
