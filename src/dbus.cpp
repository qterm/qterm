#include "dbus.h"
#include <QtDBus>
#include <QtDebug>

namespace QTerm
{
DBus * DBus::m_instance = 0;
static const QString dbusServiceName = "org.kde.VisualNotifications";
static const QString dbusInterfaceName = "org.kde.VisualNotifications";
static const QString dbusPath = "/VisualNotifications";

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
    :m_dbusAvailable(false),m_idList()
{
    m_idList.clear();
    QDBusConnectionInterface* interface = QDBusConnection::sessionBus().interface();
    m_dbusAvailable = interface && interface->isServiceRegistered(dbusServiceName);

    if( m_dbusAvailable) {
        qDebug() << "using" << dbusServiceName << "for popups";
        createDBusConnection();
    }

    // to catch register/unregister events from service in runtime
    connect(interface, SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)), this, SLOT(slotServiceOwnerChanged(const QString&, const QString&, const QString&)));

}

void DBus::createDBusConnection()
{
    bool connected = QDBusConnection::sessionBus().connect(QString(), // from any service
                dbusPath,
                dbusInterfaceName,
                "ActionInvoked",
                this,
                SLOT(slotDBusNotificationActionInvoked(uint,const QString&)));
    if (!connected) {
            qDebug() << "warning: failed to connect to NotificationClosed dbus signal";
    }
    connected = QDBusConnection::sessionBus().connect(QString(), // from any service
            dbusPath,
            dbusInterfaceName,
            "NotificationClosed",
            this,
            SLOT(slotDBusNotificationClosed(uint,uint)));
    if (!connected) {
        qDebug() << "warning: failed to connect to NotificationClosed dbus signal";
    }
}

void DBus::slotServiceOwnerChanged( const QString & serviceName, const QString & oldOwner, const QString & newOwner )
{
    if(serviceName == dbusServiceName)
    {
        if(oldOwner.isEmpty())
        {
            m_idList.clear();
            m_dbusAvailable = true;
            qDebug() << dbusServiceName << " was registered on bus, now using it to show popups";
            // connect to action invocation signals
            createDBusConnection();
        }
        if(newOwner.isEmpty())
        {
            m_idList.clear();
            m_dbusAvailable = false;
            qDebug() << dbusServiceName << " was unregistered from bus, using passive popups from now on";
        }
    }
}

bool DBus::sendDBusNotification(const QString & summary, const QString & body, QList<DBus::Action> actions)
{
    QDBusMessage message = QDBusMessage::createMethodCall( dbusServiceName, dbusPath, dbusInterfaceName, "Notify" );
    uint id = 0;
    QList<QVariant> args;
    args.append("QTerm");
    args.append(id); // If I send 0 directly, this will be an int32 instead of uin32, resulting an unknown method error.
    args.append(QString());
    args.append(QString());
    args.append(summary);
    args.append(body);
    QStringList actionList;
    foreach (DBus::Action action, actions) {
        if (action==DBus::Show_QTerm) {
            actionList.append(QString::number(DBus::Show_QTerm));
            actionList.append("Show QTerm");
        }
    }
    args.append(actionList);
    args.append(QVariantMap());
    args.append(6*1000);
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

void DBus::slotDBusNotificationActionInvoked(uint id, const QString action)
{
    if (m_idList.contains(id)) {
        if (action.toInt() == DBus::Show_QTerm) {
            closeNotification(id);
            emit showQTerm();
        }
    }
}

void DBus::slotDBusNotificationClosed(uint id, uint reason)
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

bool DBus::dbusExist() const
{
    return m_dbusAvailable;
}

} // namespace QTerm
#include <dbus.moc>
