//
// C++ Implementation: Wallet
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wallet.h"
#include "qterm.h"
#include <QtDBus>
#include <QtDebug>
#include <QtGui/QWidget>
namespace QTerm {
Wallet::Wallet(QObject * parent)
    :m_walletName(),m_handle(0)
{
    m_wallet = NULL;
}

Wallet::~Wallet()
{
    close();
}

bool Wallet::isWalletAvailable()
{
    QDBusConnectionInterface* interface = QDBusConnection::sessionBus().interface();
    return interface && interface->isServiceRegistered("org.kde.kwalletd");
}

void Wallet::open()
{
    if (m_wallet != NULL) {
        if (m_wallet->isOpen(m_handle)) {
            return;
        } else {
            emit error("Failed to open the wallet");
            return;
        }
    }
    m_wallet = new OrgKdeKWalletInterface("org.kde.kwalletd","/modules/kwalletd",QDBusConnection::sessionBus(),this);
    m_walletName = m_wallet->localWallet();
    // FIXME: how to get win id?
    //WId wid = static_cast<QWidget *> (parent())->winId();
    m_handle = m_wallet->open(m_walletName, 0, "QTerm");
    connect(m_wallet,SIGNAL(walletOpened(const QString &)), this, SLOT(walletOpened(const QString &)));
    connect(m_wallet,SIGNAL(walletClosed(int)), this, SLOT(walletClosed(int)));
}

void Wallet::close()
{
    m_wallet->close(m_handle,false,"QTerm");
    delete m_wallet;
    m_wallet = NULL;
    m_handle = 0;
}

void Wallet::walletOpened(const QString & walletName)
{
    if (!m_wallet->isOpen(m_handle)) {
        qDebug() << "Failed to open the wallet";
        emit error("Failed to open the wallet");
        return;
    }
    if (!m_wallet->hasFolder(m_handle, "QTerm", "QTerm")) {
        m_wallet->createFolder(m_handle, "QTerm", "QTerm");
    }
}

void Wallet::walletClosed(int handle)
{
    if (handle == m_handle) {
        delete m_wallet;
        m_wallet = NULL;
        m_handle = 0;
    }
}

QString Wallet::readPassword(const QString & site, const QString & username)
{
    if (!m_wallet->isOpen(m_handle)) {
        return QString();
    }

    QString key = site+"_"+username;
    QString password=m_wallet->readPassword(m_handle, "QTerm", key, "QTerm");
    return password;
}

bool Wallet::writePassword(const QString & site, const QString & username, const QString & password)
{
    if (!m_wallet->isOpen(m_handle)) {
        return false;
    }

    QString key = site+"_"+username;
    if (m_wallet->writePassword(m_handle, "QTerm", key, password, "QTerm")==0) {
        return true;
    }
    return false;
}

bool Wallet::removePassword(const QString & site, const QString & username)
{
    if (!m_wallet->isOpen(m_handle)) {
        return false;
    }

    QString key = site+"_"+username;
    if (m_wallet->removeEntry(m_handle, "QTerm", key, "QTerm")==0) {
        return true;
    }
    return false;
}

}

#include <wallet.moc>
