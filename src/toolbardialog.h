#ifndef TOOLBARDIALOG_H
#define TOOLBARDIALOG_H

#include "ui_toolbardialog.h"

class ToolbarDialog : public QDialog, public Ui_ToolbarDialog
{
    Q_OBJECT
public:
    ToolbarDialog(QWidget*);
    ~ToolbarDialog();

public slots:
    void buttonUpClicked();
    void buttonDownClicked();

    void buttonAddClicked();
    void buttonRemoveClicked();

    void comboToolbarsCurrentIndexChanged(int);
    void comboIconSizeCurrentIndexChanged(const QString&);
    void comboButtonStyleCurrentIndexChanged(int);
    void restoreDefaultToolbars();
private:
    void createDefaultToolBars();
    QMap<QString, QStringList> m_defaultToolBars;
};

#endif //TOOLBARDIALOG_H
