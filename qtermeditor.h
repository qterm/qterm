#ifndef QTERMEDITOR_H
#define QTERMEDITOR_H

#include "qtermwindowbase.h"
class QTextEdit;

class QTermEditor : public QTermWindowBase
{
	Q_OBJECT
public:	
        	
	QTermEditor(QWidget *parent=0);
	~QTermEditor();

    
public slots:
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionPrint_triggered();
    void on_actionPrint_Preview_triggered();     
    
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionAuto_Copy_triggered();
    void on_actionCopy_w_Color_triggered();
    void on_actionPaste_w_Color_triggered();
    void on_actionRectangle_Selection_triggered();
    
    void on_actionFont_triggered();
    void on_actionRuler_triggered();
    
protected:
    void closeEvent(QCloseEvent*e);

protected:
	QTextEdit *te;
};

#endif //QTERMEDITOR_H