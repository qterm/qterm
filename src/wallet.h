//
// C++ Interface: Wallet
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2009
//
/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef WALLET_H
#define WALLET_H

#include <kwallet_interface.h>

namespace QTerm{

class Wallet : public QObject
{
    Q_OBJECT
public:
    Wallet(QObject * parent = 0);
    ~Wallet();
    static bool isWalletAvailable();
    QString readPassword(const QString & site, const QString & username);
    bool writePassword(const QString & site, const QString & username, const QString & password);
    bool removePassword(const QString & site, const QString & username);
    void open();
    void close();
signals:
    void error(const QString & message);
private slots:
    void walletOpened(const QString & wallet);
    void walletClosed(int handle);
private:
    OrgKdeKWalletInterface * m_wallet;
    QString m_walletName;
    int m_handle;
};

}
#endif // WALLET_H
