#include "qtermhttp.h"
#include "qtermcanvas.h"
#include "qterm.h"
#include "qtermconfig.h"
#include "qtermglobal.h"
#include "qtermframe.h"
#include "imageviewer.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDataStream>
#include <QtCore/QUrl>
#include <QtCore/QRegularExpression>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QMessageBox>

#include <QTextCodec>

namespace QTerm
{

Http::Http(QWidget *parent, QTextCodec * codec)
    :QObject(parent)
{
    m_codec = codec;
    m_pCanvas = NULL;
    m_httpReply = NULL;
}

Http::~Http()
{
}

void Http::cancel()
{
    if (m_httpReply)
        m_httpReply->abort();

    // remove unsuccessful download
    if (QFile::exists(m_strHttpFile))
        QFile::remove(m_strHttpFile);

    emit done(this);
}

void Http::getLink(const QString& url, bool preview)
{
    m_bExist = false;
    m_bPreview = preview;
    QUrl u(url);
    m_url = u;
    // local files need no network
    if (u.isRelative() || u.scheme() == "file") {
        previewImage(u.path());
        emit done(this);
        return;
    }

    // alternative host substitute
    if (QFile::exists(Global::instance()->pathCfg() + "hosts.cfg")) {
        Config conf(Global::instance()->pathCfg() + "hosts.cfg");
        QString strTmp = conf.getItemValue("hosts", u.host()).toString();
        if (!strTmp.isEmpty()) {
            QString strUrl = url;
            strUrl.replace(QRegularExpression(u.host(), QRegularExpression::CaseInsensitiveOption), strTmp);
            u = strUrl;
        }
    }
    m_strHttpFile = QFileInfo(u.path()).fileName();
    m_httpReply = m_httpDown.get(QNetworkRequest(u));

    connect(m_httpReply, SIGNAL(finished()), this, SLOT(httpDone()));
    connect(m_httpReply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(httpRead(qint64, qint64)));
    connect(m_httpReply, SIGNAL(metaDataChanged()),
            this, SLOT(httpHeader()));
}

void Http::httpHeader()
{
    if (m_httpReply->error() != QNetworkReply::NoError)
        return;

    if(!m_httpReply->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull())
        return;

    int FileLength = m_httpReply->header(QNetworkRequest::ContentLengthHeader).toInt();
    // extract filename if exists
    // m_httpReply->header(QNetworkRequest::ContentDispositionHeader) returns QVariant(null) as of Qt5.1 beta
    QString ValueString = m_codec->toUnicode(m_httpReply->rawHeader("Content-Disposition"));
    if (ValueString.right(1) != ";")
        ValueString += ";";
    QRegularExpression re("filename=(.*);", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match;
    //re.setMinimal(true); //Dont FIXME:this will also split filenames with ';' inside, does anyone really do this?
    int pos = ValueString.indexOf(re, 0, &match);
    if (pos != -1)
        m_strHttpFile = match.captured(1);

    if (m_bPreview) {
        QString strPool = Global::instance()->m_pref.strPoolPath;

        m_strHttpFile = strPool + m_strHttpFile;

        QFileInfo fi(m_strHttpFile);

        // append .n if local file exists
        int i = 1;
        QFileInfo fi2 = fi;
        while (fi2.exists()) {
            // all the same
            if (fi2.size() == FileLength) {
                m_bExist = true;
                m_httpReply->abort();
                break;
            } else {
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

        QString strExt = fi.suffix().toLower();
        if (strExt == "jpg" || strExt == "jpeg" || strExt == "gif" ||
                strExt == "mng" || strExt == "png" || strExt == "bmp")
            m_bPreview = true;
        else
            m_bPreview = false;
    } else {
        QString strSave = Global::instance()->getSaveFileName(m_strHttpFile, NULL);
        // no filename specified which means the user canceled this download
        if (strSave.isEmpty()) {
            m_httpReply->abort();
            emit done(this);
            return;
        }
        m_strHttpFile = strSave;
    }
    QTerm::StatusBar::instance()->newProgressOperation(this)
    .setDescription(m_strHttpFile)
    .setAbortSlot(this, SLOT(cancel()))
    .setMaximum(100);
    QTerm::StatusBar::instance()->resetMainText();
    connect(this, SIGNAL(done(QObject*)), StatusBar::instance(), SLOT(endProgressOperation(QObject *)));
    connect(this, SIGNAL(percent(int)), StatusBar::instance(), SLOT(setProgress(int)));
}


void Http::httpRead(qint64 done, qint64 total)
{
    if(!m_httpReply->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull())
        return;
    QByteArray ba = m_httpReply->readAll();
    QFile file(m_strHttpFile);
    if (file.open(QIODevice::ReadWrite | QIODevice::Append)) {
        QDataStream ds(&file);
        ds.writeRawData(ba, ba.size());
        file.close();
    }
    if (total != 0)
        //m_pDialog->setProgress(done,total);
        emit percent(done*100 / total);
}

void Http::httpDone()
{
    switch (m_httpReply->error()) {
    case QNetworkReply::NoError:
        break;
    case QNetworkReply::OperationCanceledError:
        if (m_bExist)
            break;
        else {
            emit done(this);
            return;
        }
    default:
        QMessageBox::critical(NULL, tr("Download Error"),
                              tr("Failed to download file"));
        deleteLater();
        return;
    }
    // we are redirected
    QVariant redirectionTarget = m_httpReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!redirectionTarget.isNull()) {
        m_url = m_url.resolved(redirectionTarget.toUrl());
        m_httpReply->abort();
        m_httpReply->deleteLater();
        getLink(m_url.toString(), m_bPreview);
        return;
    }

    if (m_bPreview) {
        QString strPool = Global::instance()->m_pref.strPoolPath;
        previewImage(m_strHttpFile);
    } else
        emit message("Download one file successfully");

    emit done(this);
}

void Http::previewImage(const QString& filename)
{
    QString strViewer = Global::instance()->fileCfg()->getItemValue("preference", "image").toString();

    if (strViewer.isEmpty()) {
        delete m_pCanvas;
        m_pCanvas = new Canvas(qobject_cast<QWidget*>(parent()),Qt::Dialog);
        m_pCanvas->loadImage(filename);
        m_pCanvas->show();
    } else {
        QProcess::startDetached(strViewer, QStringList(filename));
    }
}

} // namespace QTerm

#include <moc_qtermhttp.cpp>
