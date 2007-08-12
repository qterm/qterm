#ifndef SCRIPT_H
#define SCRIPT_H
#include <QObject>

namespace QTerm
{

class Window;

class Script : public QObject
{
    Q_OBJECT
public:
    Script(QObject * parent);
    ~Script();

    Q_INVOKABLE int caretX();
    Q_INVOKABLE int caretY();
    Q_INVOKABLE int columns();
    Q_INVOKABLE int rows();
    Q_INVOKABLE void sendString(const QString & text);
    Q_INVOKABLE void sendParsedString(const QString & text);
    Q_INVOKABLE const QString getText(int line);
    Q_INVOKABLE const QString getAttrText(int line);
    Q_INVOKABLE bool isConnected();
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void reconnect();
    Q_INVOKABLE const QString getAddress();
    Q_INVOKABLE ushort getPort();
    Q_INVOKABLE int getProtocol();
    Q_INVOKABLE const QString getReplyKey();
    Q_INVOKABLE const QString getUrl();
    Q_INVOKABLE void previewImage(const QString & url);
private:
    Window * m_window;
};

} // namespace QTerm
#endif // SCRIPT_H

