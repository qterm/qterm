#ifndef QTERMHTTP_H
#define QTERMHTTP_H

#include <QtCore/QObject>
#include <QtNetwork/QHttp>

class QWidget;
class QTextCodec;

namespace QTerm
{
class Http : public QObject
{
    Q_OBJECT

public:
    Http(QWidget*,QTextCodec *);
    ~Http();

    void getLink(const QString&, bool);
protected slots:
    void cancel();
    void httpDone(bool);
    void httpRead(int, int);
    void httpResponse(const QHttpResponseHeader &);
    void previewImage(const QString&);

signals:
    void done(QObject*);
    void message(const QString &);
    void percent(int);

protected:
    QHttp m_httpDown;
    QString m_strHttpFile;
    bool m_bPreview;
    bool m_bExist;
    QTextCodec * m_codec;
};

} // namespace QTerm

#endif
