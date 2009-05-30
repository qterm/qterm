#ifndef SCRIPT_H
#define SCRIPT_H
#include <QtCore/QObject>
#include <QtScript>

namespace QTerm
{
class Window;
class TextLine;
class Script : public QObject
{
    Q_OBJECT
public:
    Script(Window *parent, QScriptEngine *engine);
    ~Script();
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted);
public slots:
    bool accepted() const;
    void setAccepted(bool);
    int caretX();
    int caretY();
    int columns();
    int rows();
    int char_x(int x,int y);
    int char_y(int x,int y);
    QString getUrl();
    void buzz();
    void sendString(const QString & string);
    void sendParsedString(const QString & string);
    void showMessage(const QString & message, int type, int duration);
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
