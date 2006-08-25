#include "qtermeditor.h"
#include <QTextEdit>
QTermEditor :: QTermEditor(QWidget *parent)
	: QTermWindowBase(parent)
{
    listActions << "actionSave" << "actionSave_As"
                << "actionPrint" << "actionPrint_Preview"
                << "actionCut" << "actionCopy" 
                << "actionPaste" << "actionAuto_Copy"
                << "actionPaste_w_Color"
                << "actionCopy_w_Colot"
                << "actionRectangle_Selection"
                << "actionFont" << "Ruler";
                
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
void QTermEditor :: on_actionPaste_w_Color_triggered()
{
    qWarning("Empty Slot");
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


void QTermEditor :: closeEvent(QCloseEvent*e)
{
    QTermWindowBase::closeEvent(e);
}
