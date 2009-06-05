#ifndef SCRIPT_H
#define SCRIPT_H
#include <QtCore/QObject>
#include <QtCore/QStringList>
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
    void showMessage(const QString & message, int type = 1, int duration = 0);
    void cancelZmodem();
    void setZmodemFileList(const QStringList & fileList);
    QScriptValue getLine(int line);
    QScriptValue window();
    bool addPopupMenu(QString id, QString menuTitle, QString icon = "QTerm");
    bool addUrlMenu(QString id, QString menuTitle, QString icon = "QTerm");
    void addPopupSeparator();
    void addUrlSeparator();
    void import(const QString & filename);
    void addImportedScript(const QString & filename);
    bool isScriptLoaded(const QString & filename);
    QString globalPath();
    QString localPath();
    QString getSelectedText(bool rect = false, bool color = false, const QString & escape = "");
    void openUrl(const QString & url);
    void loadExtension(const QString & extension);
    QString version();
signals:
    void scriptEvent(const QString & type);
private:
    Window * m_window;
    QScriptEngine * m_scriptEngine;
    bool m_accepted;
    QStringList m_scriptList;
    QStringList m_popupActionList;
    QStringList m_urlActionList;
};
} // namespace QTerm

#endif //SCRIPT_H
