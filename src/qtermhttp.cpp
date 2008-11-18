#include "qtermhttp.h"
#include "qtermcanvas.h"
#include "qterm.h"
#include "qtermconfig.h"
#include "qtermglobal.h"
#include "qtermframe.h"
#include "imageviewer.h"

//Added by qt3to4:
#include <QString>
#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QDataStream>
#include <QUrl>
#include <QRegExp>
#include <QtCore/QProcess>


// #include <qapplication.h>
// #include <qstring.h>
// #include <qfileinfo.h>
// #include <qmessagebox.h>
// #include <qdatastream.h>
// #include <q3url.h>
// #include <qregexp.h>
namespace QTerm
{

Http::Http(QWidget *p)
{
// 	m_pDialog = NULL;
	parent = p;
	connect(&m_httpDown, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
	connect(&m_httpDown, SIGNAL(dataReadProgress(int,int)), 
				this, SLOT(httpRead(int,int)));
	connect(&m_httpDown, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)), 
				this, SLOT(httpResponse(const QHttpResponseHeader&)));
}

Http::~Http()
{
// 	if(m_pDialog!=NULL)
// 		delete m_pDialog;
}

void Http::cancel()
{
	m_httpDown.abort();

	// remove unsuccessful download
	if(QFile::exists(m_strHttpFile))
		QFile::remove(m_strHttpFile);

	emit done(this);
}

void Http::getLink(const QString& url, bool preview)
{
	m_bExist = false;
	m_bPreview = preview;
	QUrl u(url);
	if(u.isRelative() || u.scheme() == "file")
	{
		previewImage(u.path());
		emit done(this);
		return;
	}

//	QString path = url.mid(url.find(u.host(),false) + u.host().length());
//	QString path = url.mid(url.find('/',
//							url.find(u.host(),false),
//							false));

	if(QFile::exists(Global::instance()->pathCfg()+"hosts.cfg"))
	{
		Config conf(Global::instance()->pathCfg()+"hosts.cfg");
		QString strTmp = conf.getItemValue("hosts",u.host());
		if(!strTmp.isEmpty())
		{
			QString strUrl = url;
			strUrl.replace(QRegExp(u.host(),Qt::CaseInsensitive),strTmp);
			u = strUrl;
		}
	}
	m_strHttpFile = QFileInfo(u.path()).fileName();
	m_httpDown.setHost(u.host(),u.port(80));
	m_httpDown.get(u.path()+"?"+u.encodedQuery());
}

void Http::httpResponse( const QHttpResponseHeader& hrh)
{
	QString ValueString;
	QString filename;

	ValueString = hrh.value("Content-Length");
	int FileLength = ValueString.toInt();
	
	ValueString = hrh.value("Content-Disposition");
//	ValueString = ValueString.mid(ValueString.find(';') + 1).stripWhiteSpace();
//	if(ValueString.lower().find("filename") == 0)
//	m_strHttpFile = ValueString.mid(ValueString.find('=') + 1).stripWhiteSpace();
	if(ValueString.right(1)!=";")
		ValueString+=";";
	QRegExp re("filename=.*;", Qt::CaseInsensitive);
	re.setMinimal(true); //Dont FIXME:this will also split filenames with ';' inside, does anyone really do this?
	int pos=re.indexIn(ValueString);
	if(pos!=-1)
		m_strHttpFile = ValueString.mid(pos+9,re.matchedLength()-10);
        filename = m_strHttpFile = G2U(m_strHttpFile.toLatin1());

	if(m_bPreview)
	{
		QString strPool = Global::instance()->m_pref.strPoolPath;
		
		m_strHttpFile = strPool + m_strHttpFile;

		QFileInfo fi(m_strHttpFile);
		
		int i=1;
		QFileInfo fi2=fi;
		while(fi2.exists())
		{
			// all the same 
			if(fi2.size()==FileLength)
			{
				m_bExist = true;
				m_httpDown.abort();
				break;
			}
			else
			{
				m_strHttpFile = QString("%1/%2(%3).%4") 
							.arg(fi.path()) 
							.arg(fi.completeBaseName())
							.arg(i)
							.arg(fi.suffix());
				fi2.setFile(m_strHttpFile);
				i++;
			}
		}
		fi.setFile(m_strHttpFile);
		
		QString strExt=fi.suffix().toLower();
		if(strExt=="jpg" || strExt=="jpeg" || strExt=="gif" || 
			strExt=="mng" || strExt=="png" || strExt=="bmp")
			m_bPreview=true;
		else
			m_bPreview=false;
	}
	else
	{
		QString strSave = Global::instance()->getSaveFileName(m_strHttpFile, NULL);
		// no filename specified which means the user canceled this download
		if(strSave.isEmpty())
		{
			m_httpDown.abort();
			emit done(this);
			return;
		}
		m_strHttpFile = strSave;
	}
	QTerm::StatusBar::instance()->newProgressOperation(this)
		.setDescription( filename )
		.setAbortSlot( this, SLOT(cancel()) )
		.setMaximum( 100 );
	QTerm::StatusBar::instance()->resetMainText();
	connect(this, SIGNAL(done(QObject*)), StatusBar::instance(), SLOT(endProgressOperation(QObject *)));
	connect(this, SIGNAL(percent(int)), StatusBar::instance(), SLOT(setProgress(int)));
}


void Http::httpRead(int done, int total)
{
	QByteArray ba = m_httpDown.readAll();
	QFile file(m_strHttpFile);
	if(file.open(QIODevice::ReadWrite | QIODevice::Append))
	{
		QDataStream ds(&file);
		ds.writeRawData(ba,ba.size());
		file.close();
	}
	if(total!=0)
		//m_pDialog->setProgress(done,total);
		emit percent(done*100/total);
}

void Http::httpDone(bool err)
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
				deleteLater();
				return;
		}
	}

	if(m_bPreview) {
		QString strPool = Global::instance()->m_pref.strPoolPath;
		previewImage(m_strHttpFile);
		QFileInfo fi = QFileInfo(m_strHttpFile);
		ImageViewer::genThumb(Global::instance()->pathPic()+"pic/shadow.png", strPool, fi.fileName());
	}
	else
		emit message("Download one file successfully");
	
	emit done(this);
}

void Http::previewImage(const QString& filename)
{
	QString strViewer = Global::instance()->fileCfg()->getItemValue("preference","image");

	if(strViewer.isEmpty())
	{
		Canvas *pCanvas = new Canvas();
		pCanvas->loadImage(filename);
		pCanvas->show();
	}
	else
	{
		QString strCmd = strViewer+" \""+filename+"\"";
        QProcess::startDetached(strCmd);
	}
}

} // namespace QTerm

#include <qtermhttp.moc>
