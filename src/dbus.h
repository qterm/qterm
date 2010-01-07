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
#include <QtGui/QImage>


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
    bool notificationAvailable() const;
    bool sendNotification(const QString & summary, const QString & body, const QImage & image=QImage(), QList<DBus::Action> actions = QList<DBus::Action>() );
signals:
    void showQTerm();
private slots:
    void slotServiceOwnerChanged(const QString & serviceName, const QString & oldOwner, const QString & newOwner);
    void slotNotificationActionInvoked(uint id, const QString action);
    void slotNotificationClosed(uint id, uint reason);
private:
    DBus();
    void createConnection();
    void closeNotification(uint id);
    static DBus * m_instance;
    bool m_notificationAvailable;
    QList<uint> m_idList;
};

}
#endif // DBUS_H
