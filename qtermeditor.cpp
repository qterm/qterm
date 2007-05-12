#include "qtermeditor.h"
#include "qtermscreen.h"
#include "qtermbuffer.h"
#include "qtermdecode.h"
#include "qtermtextline.h"

#include <QApplication>
#include <QClipboard>
#include <QResizeEvent>
#include <QFile>
#include <QSettings>

#include <QShortcut>

QTermEditor :: QTermEditor(QWidget *parent)
	: QTermWindowBase(parent)
{
    buffer = new QTermBuffer(QSize(80,1),8096, QTerm::EDITOR);
    buffer->setMode(INSERT_MODE);

    screen = new QTermScreen(this, buffer, QTerm::EDITOR);
    connect(screen, SIGNAL(inputMethodText(QString)),
            this, SLOT(inputMethodText(QString)));
    setFocusProxy(screen);

    QSettings setting("qterm.cfg",QSettings::IniFormat);
    setting.beginGroup("Editor");
    QString descrip = setting.value("Font").toString();
    qWarning(descrip.toLatin1().constData());
    QFont fnt;    fnt.fromString("SimHei,12");
    screen->setFont(fnt);
    setting.endGroup();
    screen->setSchema("Softness.schema");
        
    listActions << "actionSave" << "actionSave_As"
                << "actionPrint" << "actionPrint_Preview"
                << "actionCut" << "actionCopy" 
                << "actionPaste" << "actionAuto_Copy"
                << "actionCopy_w_Color"
                << "actionRectangle_Selection"
                << "actionFont" << "actionRuler";

    // for test purpose    
    QFile f("pp.txt");
    f.open(QIODevice::ReadOnly);
    QTermDecode decoder(buffer);
    QByteArray ba=f.readAll();
    decoder.decode(ba, ba.size());
    
    new QShortcut(QKeySequence::Paste,this,SLOT(on_actionPaste_triggered()));
}

QTermEditor :: ~QTermEditor()
{
}

void QTermEditor :: on_actionSave_triggered()
{
    qWarning("Empty Slot");
}
void QTermEditor :: on_actionSave_As_triggered()
{
    qWarning("Empty Slot");
}
void QTermEditor :: on_actionPrint_triggered()
{
    qWarning("Empty Slot");
}
void QTermEditor :: on_actionPrint_Preview_triggered()
{
    qWarning("Empty Slot");
}
    
void QTermEditor :: on_actionCut_triggered()
{
    qWarning("Empty Slot");
}
void QTermEditor :: on_actionCopy_triggered()
{
    qWarning("Empty Slot");
}
void QTermEditor :: on_actionAuto_Copy_triggered()
{
    qWarning("Empty Slot");
}
void QTermEditor :: on_actionCopy_w_Color_triggered()
{
    qWarning("Empty Slot");
}
void QTermEditor :: on_actionPaste_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
	QByteArray str=clipboard->text(QClipboard::Clipboard).toLocal8Bit();
    parseString(str);
}
void QTermEditor :: on_actionRectangle_Selection_triggered()
{
    qWarning("Empty Slot");
}
    
void QTermEditor :: on_actionFont_triggered()
{
    qWarning("Empty Slot");
}
void QTermEditor :: on_actionRuler_triggered()
{
    qWarning("Empty Slot");
}

void QTermEditor :: inputMethodText(QString text)
{
	if(!text.isEmpty())
        buffer->setBuffer(text.toLocal8Bit());

    screen->update(QTerm::NewData);
    screen->ensureCaretVisible();
}

void QTermEditor :: keyPressEvent(QKeyEvent *ke)
{
    qWarning("key pressed %d", ke->key());
    int x=buffer->absCaret().x();
    int y=buffer->absCaret().y();

    switch( ke->key() )
    {
        case Qt::Key_Home:
			buffer->setCaret(0,y);
		break;
		case Qt::Key_End:
			buffer->setCaret(buffer->absAt(y)->length()-1,y);
		break;
		case Qt::Key_PageUp:
            screen->scrollPageUp();
		break;
		case Qt::Key_PageDown:
            screen->scrollPageDn();
		break;
		case Qt::Key_Up:
			buffer->moveCaret(0,-1);
		break;
		case Qt::Key_Down:
			buffer->moveCaret(0,1);
		break;
		case Qt::Key_Left:
			buffer->moveCaret(-1,0);
		break;
		case Qt::Key_Right:
			buffer->moveCaret(1,0);
		break;
		case Qt::Key_Insert:
            // TODO: to have visual effect, change the caret color
            // blue for insert and red for replace
            if(buffer->getMode(INSERT_MODE))
                buffer->resetMode(INSERT_MODE);
            else
                buffer->setMode(INSERT_MODE);
        break;
		case Qt::Key_Delete:
            if(x<buffer->absAt(y)->length())
                buffer->deleteStr(1);
            else if(y<buffer->lines()-1)
            {  
                buffer->setCaret(0,y+1);
                buffer->mergeLine();
            }
		break;
		case Qt::Key_Backspace:
            if(x!=0)
            {
                buffer->moveCaret(-1,0);
                buffer->deleteStr(1);
            }
            else if(y!=0)
                buffer->mergeLine();
        break;
        case Qt::Key_Return:
            buffer->splitLine();
        break;
		default:
            if(!ke->text().isEmpty())
                buffer->setBuffer(ke->text().toLocal8Bit());
		break;
	}

	// scroll screen to ensure caret visible
    screen->update(QTerm::NewData);
    screen->ensureCaretVisible();
}

void QTermEditor :: mousePressEvent(QMouseEvent*e)
{
    // TODO: clear any selection if any

    // left button to set caret
    if(e->button()==Qt::LeftButton)
    {
        QPoint cpt=screen->mapToChar(e->globalPos());
        buffer->setCaret(cpt.x(),cpt.y());
        screen->update(QTerm::NewData);
    }

	// middle button for paste
	if(e->button()==Qt::MidButton)
	{
        QClipboard *clipboard = QApplication::clipboard();
		QByteArray str=clipboard->text(QClipboard::Selection).toLocal8Bit();
        parseString(str);
	}
}

void QTermEditor :: resizeEvent(QResizeEvent*e)
{
    screen->resize(e->size());
}

void QTermEditor :: closeEvent(QCloseEvent*e)
{
    QTermWindowBase::closeEvent(e);
}

void QTermEditor :: parseString(const QByteArray& data)
{
    QTermDecode decoder(buffer);
    QList<QByteArray> listStr = data.split('\n');
    QList<QByteArray>::const_iterator i=listStr.constBegin();

    while(true)
    {
        decoder.decode(i->data(),i->size());
        // FIXME: dont split if only one line
        i++;
        if(i!=listStr.constEnd())
            buffer->splitLine();
        else
            break;
    }
    screen->update(QTerm::NewData);
}
