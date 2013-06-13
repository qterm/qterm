/*
 * Copyright (c) 2009 Nokia Corporation
 */

#include "qnamredirect.h"

QNAMRedirect::QNAMRedirect(QWidget *parent) : QMainWindow(parent) {
	_qnam = createQNAM();
	_originalUrl = "http://mitbbs.com/article2/PhotoForum/31264659_224.jpg";

	QVBoxLayout* layout = new QVBoxLayout;

#ifdef Q_OS_SYMBIAN
	setStyleSheet("QLabel, QPushButton{ font: 5pt; }");
#endif

	_textContainer = new QLabel("Click the button to test URL redirect!");
	_textContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	_textContainer->setAlignment(Qt::AlignCenter);
	_textContainer->setWordWrap(true);
	layout->addWidget(_textContainer);

	QPushButton* testButton = new QPushButton("Click here!");
	/* If the button is clicked, we'll do a request. */
	connect(testButton, SIGNAL(clicked()),
	        this, SLOT(doRequest()));
	layout->addWidget(testButton);

	QFrame* frame = new QFrame;
	frame->setLayout(layout);
	setCentralWidget(frame);
}

QNAMRedirect::~QNAMRedirect() {
	if(_qnam) {
		_qnam->deleteLater();
	}
	if(_textContainer) {
		_textContainer->deleteLater();
	}
}

QNetworkAccessManager* QNAMRedirect::createQNAM() {
	QNetworkAccessManager* qnam = new QNetworkAccessManager(this);
	/* We'll handle the finished reply in replyFinished */
	connect(qnam, SIGNAL(finished(QNetworkReply*)),
	        this, SLOT(replyFinished(QNetworkReply*)));
	return qnam;
}

void QNAMRedirect::doRequest() {
	QString text = "QNAMRedirect::doRequest doing request to ";
	text.append(this->_originalUrl.toString());
	this->_textContainer->setText(text);
	/* Let's just create network request for this predifned URL... */
	QNetworkRequest request(this->_originalUrl);
	/* ...and ask the manager to do the request. */
	QNetWorkReply reply = this->_qnam->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
             this, SLOT(updateDataReadProgress(qint64,qint64)));    
}

void QNAMRedirect::replyFinished(QNetworkReply* reply) {
	/*
	 * Reply is finished!
	 * We'll ask for the reply about the Redirection attribute
	 * http://doc.trolltech.com/qnetworkrequest.html#Attribute-enum
	 */
	QVariant possibleRedirectUrl =
	         reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

	/* We'll deduct if the redirection is valid in the redirectUrl function */
	_urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(),
	                                     _urlRedirectedTo);

	/* If the URL is not empty, we're being redirected. */
	if(!_urlRedirectedTo.isEmpty()) {
		QString text = QString("QNAMRedirect::replyFinished: Redirected to ")
		                      .append(_urlRedirectedTo.toString());
		this->_textContainer->setText(text);

		/* We'll do another request to the redirection url. */
		this->_qnam->get(QNetworkRequest(_urlRedirectedTo));
	}
	else {
		/*
		 * We weren't redirected anymore
		 * so we arrived to the final destination...
		 */
		QString text = QString("QNAMRedirect::replyFinished: Arrived to ")
		                      .append(reply->url().toString());
		this->_textContainer->setText(text);
		/* ...so this can be cleared. */
		_urlRedirectedTo.clear();
	}
	/* Clean up. */
	reply->deleteLater();
}

QUrl QNAMRedirect::redirectUrl(const QUrl& possibleRedirectUrl,
                               const QUrl& oldRedirectUrl) const {
	QUrl redirectUrl;
	/*
	 * Check if the URL is empty and
	 * that we aren't being fooled into a infinite redirect loop.
	 * We could also keep track of how many redirects we have been to
	 * and set a limit to it, but we'll leave that to you.
	 */
	if(!possibleRedirectUrl.isEmpty() &&
	   possibleRedirectUrl != oldRedirectUrl) {
		redirectUrl = possibleRedirectUrl;
	}
	return redirectUrl;
}
