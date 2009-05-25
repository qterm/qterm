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
    Script(Window *parent);
    ~Script();
public slots:
    int caretX();
    int caretY();
    int columns();
    int rows();
    int char_x(int x,int y);
    int char_y(int x,int y);
    int pos(int x,int y);
    void buzz();
    void sendString(const QString & string);
    void showMessage(const QString & message, int type, int duration);
//    void sendParsedString(const QString & message);
    QScriptValue getLine(int line);
private:
    Window * m_window;
};
} // namespace QTerm

#endif //SCRIPT_H
