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

void QTermEditor :: closeEvent(QCloseEvent*e)
{
    QTermWindowBase::closeEvent(e);
}
