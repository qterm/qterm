#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qtermwindow.h"
#include "qtermbuffer.h"
#include "qtermtextline.h"
#include "qtermtelnet.h"
#include "qtermparam.h"
#include "qtermbbs.h"
#include "qtermscript.h"
#include <QByteArray>
namespace QTerm
{

Script::Script(QObject * parent)
{
    m_window = qobject_cast<Window*>(parent);
}

Script::~Script()
{
}
/*
QString Script::getArticle(PyObject *, PyObject *args)
{
	int timeout;
	int succeed=1;

	QTermWindow *pWin=(QTermWindow*)lp;

	QStringList strList;
	QString strArticle;
	while(1)
	{
		// check it there is duplicated string
		// it starts from the end in the range of one screen height
		// so this is a non-greedy match
		QString strTemp = pWin->stripWhitespace(
				pWin->m_pBuffer->screen(0)->getText());
		int i=0;
		int start=0;
		for(QStringList::Iterator it=strList.fromLast();
	it!=strList.begin(), i < pWin->m_pBuffer->line()-1; // not exceeeding the last screen
			--it, i++)
		{
			if(*it!=strTemp)
				continue;
			QStringList::Iterator it2 = it;
			bool dup=true;
			// match more to see if its duplicated
			for(int j=0; j<=i; j++, it2++)
			{
				QString str1 = pWin->stripWhitespace(
					pWin->m_pBuffer->screen(j)->getText());
				if(*it2!=str1)
				{
					dup = false;
					break;
				}
			}
			if(dup)
			{
				// set the start point
				start = i+1;
				break;
			}
		}
		// add new lines
		for(i=start;i<pWin->m_pBuffer->line()-1;i++)
			strList+=pWin->stripWhitespace(
			pWin->m_pBuffer->screen(i)->getText());

		// the end of article
		if( pWin->m_pBuffer->screen(
		pWin->m_pBuffer->line()-1)->getText().find("%") == -1 )
			break;
		// continue
		pWin->m_pTelnet->write(" ", 1);
		
		if(!pWin->m_wcWaiting.wait(timeout*1000))	// timeout
		{
			succeed=0;
			break;
		}
	}
	#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
	strArticle = strList.join("\r\n");
	#else
	strArticle = strList.join("\n");
	#endif
	
	PyObject *py_res = Py_BuildValue("si",(const char *)strArticle.toLocal8Bit(), succeed);
	
	Py_INCREF(py_res);

	return py_res;

}

static PyObject *qterm_formatError(PyObject *, PyObject *args)
{
	long lp;
	
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;

	QString strErr;
	QString filename = getErrOutputFile((QTermWindow*)lp);

	QDir d;
	if(d.exists(filename))
	{
		QFile file(filename);
		file.open(QIODevice::ReadOnly);
		QTextStream is( &file );
        while ( !is.atEnd() ) 
		{
			strErr += is.readLine(); // line of text excluding '\n'
			strErr += '\n'; 
		}
		file.close();
		d.remove( filename );
	}

	if( !strErr.isEmpty() )
	{
		((QTermWindow*)lp)->m_strPythonError = strErr;
		qApp->postEvent( (QTermWindow*)lp, new QCustomEvent(PYE_ERROR));
	}
	else
		qApp->postEvent( (QTermWindow*)lp, new QCustomEvent(PYE_FINISH));


	Py_INCREF(Py_None);
	return Py_None;
}
*/
// caret x
int Script::caretX()
{
    return m_window->m_pBuffer->caret().x();
}

// caret y
int Script::caretY()
{
    return m_window->m_pBuffer->caret().y();
}

// columns
int Script::columns()
{
    return m_window->m_pBuffer->columns();
}

// rows
int Script::rows()
{
    return m_window->m_pBuffer->columns();
}

// sned string to server
void Script::sendString(const QString & text)
{
    QByteArray encodedText = m_window->unicode2bbs(text);
    m_window->m_pTelnet->write(encodedText,encodedText.length());
}

// same as above except parsing string first "\n" "^p" etc
// FIXME: non-latin1 chars in parsedString?
void Script::sendParsedString(const QString & text)
{
    m_window->sendParsedString(text.toUtf8());
}

// get text at line
const QString Script::getText(int line)
{
    QByteArray cstr = m_window->m_pBuffer->screen(line)->getText();
    return m_window->fromBBSCodec(cstr);
}

// get text with attributes
// FIXME: am I write to convert the text through fromBBSCodec()
const QString Script::getAttrText(int line)
{
    QByteArray cstr = m_window->m_pBuffer->screen(line)->getAttrText();
    return m_window->fromBBSCodec(cstr);
}

// is host connected
bool Script::isConnected()
{
    return m_window->isConnected();
}

// disconnect from host
void Script::disconnect()
{
    m_window->disconnect();
}

// reconnect to host
void Script::reconnect()
{
    m_window->reconnect();
}
/*
// bbs encoding 0-GBK 1-BIG5
static PyObject *qterm_getBBSCodec(PyObject *, PyObject *args)
{
	long lp;
	if (!PyArg_ParseTuple(args, "l", &lp))
		return NULL;
	
	PyObject *py_codec = PyString_FromString(
					((QTermWindow*)lp)->m_param.m_nBBSCode==0?"GBK":"Big5");
	Py_INCREF(py_codec);

	return py_codec;
}
*/

// host address
const QString Script::getAddress()
{
    return m_window->m_param.m_strAddr;
}

// host port number
ushort Script::getPort()
{
    return m_window->m_param.m_uPort;
}

// connection protocol 0-telnet 1-SSH1 2-SSH2
int Script::getProtocol()
{
    return m_window->m_param.m_nProtocolType;
}

// key to reply msg
const QString Script::getReplyKey()
{
    return m_window->m_param.m_strReplyKey;
}

// url under mouse
// FIXME: non-latin char in a URL?
const QString Script::getUrl()
{
    return m_window->m_pBBS->getUrl();
}

// preview image link
void Script::previewImage(const QString & url)
{
    m_window->getHttpHelper(url,true);
}

// convert string from UTF8 to specified encoding
// FIXME: do we really need them?
/*
static PyObject *qterm_fromUTF8(PyObject *, PyObject *args)
{
	char *str, *enc;
	if (!PyArg_ParseTuple(args, "ss", &str, &enc))
		return NULL;
	QTextCodec *encodec = QTextCodec::codecForName(enc);
	QTextCodec *utf8 = QTextCodec::codecForName("utf8");
	
	PyObject *py_str = PyString_FromString(
					encodec->fromUnicode(utf8->toUnicode(str)));
	Py_INCREF(py_str);
	return py_str;
}
// convert string from specified encoding to UTF8
static PyObject *qterm_toUTF8(PyObject *, PyObject *args)
{
	char *str, *enc;
	if (!PyArg_ParseTuple(args, "ss", &str, &enc))
		return NULL;
	QTextCodec *encodec = QTextCodec::codecForName(enc);
	QTextCodec *utf8 = QTextCodec::codecForName("utf8");
	
	PyObject *py_str = PyString_FromString(
					utf8->fromUnicode(encodec->toUnicode(str)));
	Py_INCREF(py_str);
	return py_str;
}
*/

} // namespace QTerm

#include <qtermscript.moc>

