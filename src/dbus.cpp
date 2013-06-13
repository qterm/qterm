//
// C++ Implementation: DBus
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "dbus.h"
#include "imageconverter.h"
#include <QtDBus>
#include <QtDebug>

namespace QTerm
{
DBus * DBus::m_instance = 0;
static const QString dbusServiceName = "org.freedesktop.Notifications";
static const QString dbusInterfaceName = "org.freedesktop.Notifications";
static const QString dbusPath = "/org/freedesktop/Notifications";

DBus * DBus::instance()
{
    static QMutex mutex;
    if (!m_instance) {
        mutex.lock();

        if (!m_instance)
            m_instance = new DBus;

        mutex.unlock();
    }

    return m_instance;
}

DBus::DBus()
    :m_notificationAvailable(false),m_idList(),m_serverCapabilities()
{
    m_idList.clear();
    QDBusConnectionInterface* interface = QDBusConnection::sessionBus().interface();
    m_notificationAvailable = interface && interface->isServiceRegistered(dbusServiceName);

    if( m_notificationAvailable ) {
        qDebug() << "using" << dbusServiceName << "for popups";
        createConnection();
    }

    // to catch register/unregister events from service in runtime
    connect(interface, SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)), this, SLOT(slotServiceOwnerChanged(const QString&, const QString&, const QString&)));

}

void DBus::createConnection()
{
    bool connected = QDBusConnection::sessionBus().connect(QString(), // from any service
                dbusPath,
                dbusInterfaceName,
                "ActionInvoked",
                this,
                SLOT(slotNotificationActionInvoked(uint,const QString&)));
    if (!connected) {
            qDebug() << "warning: failed to connect to ActionInvoked dbus signal";
    }
    connected = QDBusConnection::sessionBus().connect(QString(), // from any service
            dbusPath,
            dbusInterfaceName,
            "NotificationClosed",
            this,
            SLOT(slotNotificationClosed(uint,uint)));
    if (!connected) {
        qDebug() << "warning: failed to connect to NotificationClosed dbus signal";
    }
    checkCapabilities();
}

void DBus::checkCapabilities()
{
    QDBusMessage message = QDBusMessage::createMethodCall( dbusServiceName, dbusPath, dbusInterfaceName, "GetCapabilities" );
    QDBusReply<QStringList> replyMsg = QDBusConnection::sessionBus().call(message);
    m_serverCapabilities = replyMsg.value();
}

void DBus::slotServiceOwnerChanged( const QString & serviceName, const QString & oldOwner, const QString & newOwner )
{
    if(serviceName == dbusServiceName)
    {
        if(oldOwner.isEmpty())
        {
            m_idList.clear();
            m_notificationAvailable = true;
            qDebug() << dbusServiceName << " was registered on bus, now using it to show popups";
            // connect to action invocation signals
            createConnection();
        }
        if(newOwner.isEmpty())
        {
            m_idList.clear();
            m_notificationAvailable = false;
            qDebug() << dbusServiceName << " was unregistered from bus, using passive popups from now on";
        }
    }
}

bool DBus::sendNotification(const QString & summary, const QString & body, const QImage & image, QList<DBus::Action> actions)
{
    QDBusMessage message = QDBusMessage::createMethodCall( dbusServiceName, dbusPath, dbusInterfaceName, "Notify" );
    uint id = 0;
    QList<QVariant> args;
    args.append("QTerm");
    args.append(id); // If I send 0 directly, this will be an int32 instead of uin32, resulting an unknown method error.
//    args.append(QString());
    args.append("qterm"); // Icon name
    args.append(summary); // Title
    args.append(body); // Text
    QStringList actionList;
    if (m_serverCapabilities.contains("actions")) {
        foreach (DBus::Action action, actions) {
            if (action==DBus::Show_QTerm) {
                actionList.append(QString::number(DBus::Show_QTerm));
                actionList.append("Show QTerm");
            }
        }
    }
    args.append(actionList);
    QVariantMap map;
    if (!image.isNull())
        map["image_data"] = ImageConverter::variantForImage(image);
    args.append(map);
    args.append(-1);//-1 means: notification server decides
    message.setArguments(args);
    QDBusMessage replyMsg = QDBusConnection::sessionBus().call(message);
    if(replyMsg.type() == QDBusMessage::ReplyMessage) {
        if (!replyMsg.arguments().isEmpty()) {
            uint dbus_id = replyMsg.arguments().at(0).toUInt();
            m_idList.append(dbus_id);
        } else {
            qDebug() << "error: received reply with no arguments";
        }
    } else if (replyMsg.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "error: failed to send dbus message";
    } else {
        qDebug() << "unexpected reply type";
    }
    //TODO: handle errors
    return true;
}

void DBus::slotNotificationActionInvoked(uint id, const QString action)
{
    if (m_idList.contains(id)) {
        if (action.toInt() == DBus::Show_QTerm) {
            closeNotification(id);
            emit showQTerm();
        }
    }
}

void DBus::slotNotificationClosed(uint id, uint reason)
{
    if (m_idList.contains(id))
        m_idList.removeAll(id);
}

void DBus::closeNotification(uint id)
{
    if (m_idList.contains(id)) {
        m_idList.removeAll(id);
        QDBusMessage m = QDBusMessage::createMethodCall( dbusServiceName, dbusPath, dbusInterfaceName, "CloseNotification" );
        QList<QVariant> args;
        args.append(id);
        m.setArguments( args );
        bool queued = QDBusConnection::sessionBus().send(m);
        if (!queued) {
            qDebug() << "warning: failed to queue dbus message";
        }
    }
}

bool DBus::notificationAvailable() const
{
    return m_notificationAvailable;
}

} // namespace QTerm
#include <moc_dbus.cpp>
