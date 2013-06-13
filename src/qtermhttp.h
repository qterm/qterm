#ifndef QTERMHTTP_H
#define QTERMHTTP_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class QWidget;
class QTextCodec;

namespace QTerm
{
class Canvas;
class Http : public QObject
{
    Q_OBJECT

public:
    Http(QWidget*,QTextCodec *);
    ~Http();

    void getLink(const QString&, bool);
protected slots:
    void cancel();
    void httpDone();
    void httpRead(qint64 bytesRead, qint64 totlaBytes);
    void httpHeader();
    void previewImage(const QString&);

signals:
    void done(QObject*);
    void message(const QString &);
    void percent(int);

protected:
    QNetworkAccessManager m_httpDown;
    QNetworkReply *m_httpReply;
    QUrl m_url;   
    QString m_strHttpFile;
    bool m_bPreview;
    bool m_bExist;
    QTextCodec * m_codec;
    Canvas * m_pCanvas;
};

} // namespace QTerm

#endif
