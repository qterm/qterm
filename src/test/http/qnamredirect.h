/*
 * Copyright (c) 2009 Nokia Corporation
 */

#ifndef QNAMREDIRECT_H
#define QNAMREDIRECT_H

#include <QtGui/QMainWindow>
#include <QtGui/QFrame>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QPointer>
#include <QtCore/QUrl>

class QNAMRedirect : public QMainWindow
{
    Q_OBJECT

public:
	QNAMRedirect(QWidget *parent = 0);
	~QNAMRedirect();

private slots:
	void doRequest();
	void replyFinished(QNetworkReply* reply);

private:
	QPointer<QNetworkAccessManager> _qnam;
	QUrl _originalUrl;
	QUrl _urlRedirectedTo;

	QPointer<QLabel> _textContainer;

	QNetworkAccessManager* createQNAM();
	QUrl redirectUrl(const QUrl& possibleRedirectUrl,
	                 const QUrl& oldRedirectUrl) const;
};

#endif // QNAMREDIRECT_H
