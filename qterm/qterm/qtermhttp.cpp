#include "qtermhttp.h"
#include "qtermcanvas.h"
#include "qterm.h"
#include "qtermconfig.h"

#include <qstring.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qdatastream.h>
#include <qurl.h>
#include <qregexp.h>

extern QString pathCfg;
extern QString fileCfg;

extern void runProgram(const QCString&);
extern QString getSaveFileName(const QString&, QWidget*);

QTermHttp::QTermHttp()
{
	m_pDialog.setCaption(tr("QTerm Http Downlader"));
	connect(&m_pDialog, SIGNAL(canceled()), this, SLOT(cancel()));

	connect(&m_httpDown, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
	connect(&m_httpDown, SIGNAL(dataReadProgress(int,int)), 
				this, SLOT(httpRead(int,int)));
	connect(&m_httpDown, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)), 
				this, SLOT(httpResponse(const QHttpResponseHeader&)));
}

QTermHttp::~QTermHttp()
{
}

void QTermHttp::cancel()
{
	m_httpDown.abort();

	// remove unsuccessful download
	if(QFile::exists(m_strHttpFile))
		QFile::remove(m_strHttpFile);

	emit done(this);
}

void QTermHttp::getLink(const QString& url, bool preview)
{
	m_bExist = false;
	m_bPreview = preview;
	
	QUrl u(url);
	if(u.isLocalFile())
	{
		qWarning("local file %s", u.path());
		previewImage(u.path());
		emit done(this);
		return;
	}

//	QString path = url.mid(url.find(u.host(),false) + u.host().length());
//	QString path = url.mid(url.find('/',
//							url.find(u.host(),false),
//							false));

	if(QFile::exists(pathCfg+"hosts.cfg"))
	{
		QTermConfig conf(pathCfg+"hosts.cfg");
		QString strTmp = conf.getItemValue("hosts",u.host().local8Bit());
		if(!strTmp.isEmpty())
		{
			QString strUrl = url;
			strUrl.replace(QRegExp(u.host(),false),strTmp);
			u = strUrl;
		}
	}
	m_strHttpFile = u.fileName();
	m_httpDown.setHost(u.host(),u.hasPort()?u.port():80);
	m_httpDown.get(u.encodedPathAndQuery());
}

void QTermHttp::httpResponse( const QHttpResponseHeader& hrh)
{
	QString ValueString;

	ValueString = hrh.value("Content-Length");
	int FileLength = ValueString.toInt();
	
	ValueString = hrh.value("Content-Disposition");
	ValueString = ValueString.mid(ValueString.find(';') + 1).stripWhiteSpace();
	if(ValueString.lower().find("filename") == 0)
		m_strHttpFile = ValueString.mid(ValueString.find('=') + 1).stripWhiteSpace();
	
	if(m_bPreview)
	{
		QTermConfig conf(fileCfg);
		QString strPool = QString::fromLocal8Bit(conf.getItemValue("preference","pool"));

		m_strHttpFile = strPool + G2U(m_strHttpFile);

		QFileInfo fi(m_strHttpFile);
		
		int i=2;
		while(fi.exists())
		{
			// all the same 
			if(fi.size()==FileLength)
			{
				m_bExist = true;
				m_httpDown.abort();
				break;
			}
			else
			{
				m_strHttpFile = QString("%1/%2(%3).%4") 
							.arg(fi.dirPath()) 
							.arg(fi.baseName(true))
							.arg(i)
							.arg(fi.extension(false));
				fi.setFile(m_strHttpFile);
			}
		}
		
		QString strExt=fi.extension(false).lower();
		if(strExt=="jpg" || strExt=="jpeg" || strExt=="gif" || 
			strExt=="mng" || strExt=="png" || strExt=="bmp")
			m_bPreview=true;
		else
			m_bPreview=false;
	}
	else
	{
		QString strSave = getSaveFileName(m_strHttpFile, NULL);
		// no filename specified which means the user canceled this download
		if(strSave.isEmpty())
		{
			m_httpDown.abort();
			emit done(this);
			return;
		}
		m_strHttpFile = strSave;
	}
	m_pDialog.setLabelText(QFileInfo(m_strHttpFile).fileName());
}


void QTermHttp::httpRead(int done, int total)
{
	QByteArray ba = m_httpDown.readAll();
	QFile file(m_strHttpFile);
	if(file.open(IO_ReadWrite | IO_Append))
	{
		QDataStream ds(&file);
		ds.writeRawBytes(ba,ba.size());
		file.close();
	}
	if(total!=0)
		m_pDialog.setProgress(done,total);
}

void QTermHttp::httpDone(bool err)
{
	if(err)
	{
		switch(m_httpDown.error())
		{
			case QHttp::Aborted:
				if(m_bExist)
					break;
				else
				{
					emit done(this);
					return;
				}
			default:
				QMessageBox::critical(NULL, tr("Download Error"),
					tr("Failed to download file"));
				delete this;
				return;
		}
	}

	if(m_bPreview)
		previewImage(m_strHttpFile);
	else
		QMessageBox::information(NULL, tr("Download Complete"),
			tr("Download one file successfully"));
	
	emit done(this);
}

void QTermHttp::previewImage(const QString& filename)
{
	QTermConfig conf(fileCfg);
	QCString cstrViewer = conf.getItemValue("preference","image");

	if(cstrViewer.isEmpty())
	{
		QTermCanvas *pCanvas = new QTermCanvas();
		pCanvas->loadImage(filename);
		pCanvas->show();
	}
	else
	{
		QCString cstrCmd = cstrViewer+" "+filename.local8Bit();
		runProgram(cstrCmd);
	}
}

