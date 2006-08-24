#ifndef TOOLBARDIALOG_H
#define TOOLBARDIALOG_H

#include "ui_toolbardialog.h"

class toolbarDialog : public QDialog, public Ui_toolbarDialog
{
    Q_OBJECT
public:
    toolbarDialog(QWidget*);
    ~toolbarDialog();

public slots:
    void on_buttonUp_clicked();
    void on_buttonDown_clicked();

    void on_buttonAdd_clicked();
    void on_buttonRemove_clicked();
    
    void on_comboToolbars_currentIndexChanged(int);
    void on_comboIconSize_currentIndexChanged(const QString&);
    void on_comboButtonStyle_currentIndexChanged(int);
};

#endif //TOOLBARDIALOG_H
