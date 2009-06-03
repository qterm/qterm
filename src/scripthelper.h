#ifndef SCRIPT_H
#define SCRIPT_H
#include <QtCore/QObject>
#include <QtScript>

namespace QTerm
{
class Window;
class TextLine;
class ScriptHelper : public QObject
{
    Q_OBJECT
public:
    ScriptHelper(Window *parent, QScriptEngine *engine);
    ~ScriptHelper();
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted);
public slots:
    bool accepted() const;
    void setAccepted(bool);
    int caretX();
    int caretY();
    int columns();
    int rows();
    int charX(int x,int y);
    int charY(int x,int y);
    int posX();
    int posY();
    QString getUrl();
    QString getIP();
    bool isConnected();
    void reconnect();
    void disconnect();
    void buzz();
    void sendString(const QString & string);
    void sendParsedString(const QString & string);
    void showMessage(const QString & message, int type, int duration);
    void cancelZmodem();
    void setZmodemFileList(const QStringList & fileList);
    QScriptValue getLine(int line);
signals:
    void downloadFinished();
private:
    Window * m_window;
    QScriptEngine * m_engine;
    bool m_accepted;
};
} // namespace QTerm

#endif //SCRIPT_H
