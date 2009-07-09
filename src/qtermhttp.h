#ifndef QTERMHTTP_H
#define QTERMHTTP_H

// #include <qobject.h>
// #include <q3http.h>
// #include <q3progressdialog.h>
#include <QObject>
#include <QHttp>
// #include <QProgressDialog>
namespace QTerm
{
class Http : public QObject
{
    Q_OBJECT

public:
    Http(QWidget*);
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
//     Q3ProgressDialog *m_pDialog;
    bool m_bPreview;
    bool m_bExist;
    QWidget *parent;
};

} // namespace QTerm

#endif
