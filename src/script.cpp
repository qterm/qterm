#include "qterm.h"
#include "script.h"
#include "qtermwindow.h"
#include "qtermbuffer.h"
#include "qtermscreen.h"
#include "qtermframe.h"
#include "qtermbbs.h"
#include "qtermtextline.h"
#include <QtScript>

namespace QTerm
{
Script::Script(Window * parent, QScriptEngine * engine)
    :QObject(parent)
{
    m_window = parent;
    m_engine = engine;
}

Script::~Script()
{
}

int Script::caretX()
{
    return m_window->m_pBuffer->caret().x();
}

int Script::caretY()
{
    return m_window->m_pBuffer->caret().y();
}

int Script::char_x(int x, int y)
{
    QPoint pt = m_window->getScreen()->mapToChar(QPoint(x,y));
    return pt.x();
}

int Script::char_y(int x, int y)
{
    QPoint pt = m_window->getScreen()->mapToChar(QPoint(x,y));
    return pt.y() - m_window->m_pBBS->getScreenStart();
}

int Script::pos(int x, int y)
{
    return m_window->m_pBuffer->screen(y)->pos(x);
}

int Script::columns()
{
    return m_window->m_pBuffer->columns();
}

int Script::rows()
{
    return m_window->m_pBuffer->line();
}

void Script::buzz()
{
    m_window->m_pFrame->buzz();
}

void Script::sendString(const QString & string)
{
    m_window->inputHandle(string);
}

void Script::showMessage(const QString & message, int type, int duration)
{
    m_window->showMessage(message, type, duration);
}

QScriptValue Script::getLine(int line)
{
    TextLine * obj = m_window->m_pBuffer->screen(line);
    return m_engine->newQObject(obj);
}

{
}

} // namespace QTerm
#include <script.moc>
