#ifndef SHORTCUTSDIALOG_H
#define SHORTCUTSDIALOG_H

#include "ui_shortcutsdialog.h"
#include <QSet>
class shortcutsDialog : public QDialog, public Ui_shortcutsDialog
{
	Q_OBJECT
public:
	shortcutsDialog(QWidget*,QList<QAction*>,QList<QShortcut*>);
	~shortcutsDialog(){}
	
protected slots:
    void on_buttonRemove_clicked();
    void on_buttonDone_clicked();    
    void on_buttonAssign_clicked();
    void on_editShortcut_textChanged(const QString&);
    void on_tableWidget_currentCellChanged(int,int,int,int);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    QString getName(QObject*);
    void setKeySequence(QObject*, QKeySequence);
private:
    QList<QObject*> listActions;
    QList<QString> listKeys;
};


#endif //SHORTCUTSDIALOG_H
