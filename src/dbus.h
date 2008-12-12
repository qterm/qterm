//
// C++ Interface: DBus
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DBUS_H
#define DBUS_H

#include "qtermparam.h"

#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QObject>

namespace QTerm
{
class DBus: public QObject
{
    Q_OBJECT
public:
    enum Action {
        Show_QTerm
    };
    static DBus * instance();
    bool dbusExist() const;
    bool sendDBusNotification(const QString & summary, const QString & body, QList<DBus::Action> actions);
signals:
    void showQTerm();
private slots:
    void slotServiceOwnerChanged(const QString & serviceName, const QString & oldOwner, const QString & newOwner);
    void slotDBusNotificationActionInvoked(uint id, const QString action);
    void slotDBusNotificationClosed(uint id, uint reason);
private:
    DBus();
    void createDBusConnection();
    void closeNotification(uint id);
    static DBus * m_instance;
    bool m_dbusAvailable;
    QList<uint> m_idList;
};

}
#endif // DBUS_H
