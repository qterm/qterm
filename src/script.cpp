#include "qterm.h"
#include "script.h"
#include "qtermwindow.h"
#include "qtermbuffer.h"
#include "qtermscreen.h"
#include "qtermtextline.h"

namespace QTerm
{
Script::Script(Window * parent)
    :QObject(parent)
{
    m_window = parent;
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

int Script::columns()
{
    return m_window->m_pBuffer->columns();
}

int Script::rows()
{
    return m_window->m_pBuffer->line();
}

void Script::sendString(const QString & string)
{
    m_window->inputHandle(string);
}

void Script::showMessage(const QString & message, int type, int duration)
{
    m_window->showMessage(message, type, duration);
}

QString Script::getText(int line)
{
    return m_window->m_pBuffer->screen(line)->getText();
}

QString Script::getAttrText(int line)
{
    return m_window->m_pBuffer->screen(line)->getAttrText();
}

QByteArray Script::getColor(int line)
{
    return m_window->m_pBuffer->screen(line)->getColor();
}

QByteArray Script::getAttr(int line)
{
    return m_window->m_pBuffer->screen(line)->getAttr();
}

} // namespace QTerm
#include <script.moc>
