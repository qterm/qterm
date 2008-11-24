#ifndef SHORTCUTSDIALOG_H
#define SHORTCUTSDIALOG_H

#include "ui_shortcutsdialog.h"
#include <QShortcut>

namespace QTerm{

class ShortcutsDialog : public QDialog, public Ui_ShortcutsDialog
{
    Q_OBJECT
public:
    ShortcutsDialog(QWidget*, QList<QAction*>, QList<QShortcut*>);
    ~ShortcutsDialog() {}

protected slots:
    void buttonRemoveClicked();
    void buttonDoneClicked();
    void buttonAssignClicked();
    void editShortcutTextChanged(const QString&);
    void tableWidgetCurrentCellChanged(int, int, int, int);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    QString getName(QObject*);
    void setKeySequence(QObject*, QKeySequence);
private:
    QList<QObject*> listActions;
    QList<QString> listKeys;
};

}

#endif //SHORTCUTSDIALOG_H
