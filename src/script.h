#ifndef SCRIPT_H
#define SCRIPT_H
#include <QtCore/QObject>

namespace QTerm
{
class Window;
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
    void sendString(const QString & string);
    void showMessage(const QString & message, int type, int duration);
//    void sendParsedString(const QString & message);
    QString getText(int line);
    QString getAttrText(int line);
    QByteArray getColor(int line);
    QByteArray getAttr(int line);
private:
    Window * m_window;
};
} // namespace QTerm

#endif //SCRIPT_H
