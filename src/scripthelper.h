#ifndef SCRIPT_H
#define SCRIPT_H
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QQmlEngine>

namespace QTerm
{
class Window;
class TextLine;
class ScriptHelper : public QObject
{
    Q_OBJECT
public:
    ScriptHelper(Window *parent, QQmlEngine *engine);
    ~ScriptHelper();
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted)
    Q_PROPERTY(bool qtbindingsAvailable READ qtbindingsAvailable)
    void loadScriptFile(const QString&);
public slots:
    bool accepted() const;
    bool qtbindingsAvailable() const;
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
    void osdMessage(const QString & message, int type = 1, int duration = 0);
    void showMessage(const QString & title, const QString & message, int duration = -1);
    void cancelZmodem();
    void setZmodemFileList(const QStringList & fileList);
    QJSValue getLine(int line);
    QJSValue window();
    bool addPopupMenu(QString id, QString menuTitle, QString icon = "QTerm");
    bool addUrlMenu(QString id, QString menuTitle, QString icon = "QTerm");
    void addPopupSeparator();
    void addUrlSeparator();
    void loadScript(const QString & filename);
    QString globalPath();
    QString localPath();
    QString getSelectedText(bool rect = false, bool color = false, const QString & escape = "");
    void openUrl(const QString & url);
    QString version();
    QString findFile(const QString & filename);
signals:
    void scriptEvent(const QString & type);
    void eventFinished();
private:
    bool isScriptLoaded(const QString & filename);
    void addImportedScript(const QString & filename);
    Window * m_window;
    QQmlEngine * m_scriptEngine;
    bool m_accepted;
    bool m_qtbindingsAvailable;
    QStringList m_scriptList;
    QStringList m_popupActionList;
    QStringList m_urlActionList;
};
} // namespace QTerm

#endif //SCRIPT_H
