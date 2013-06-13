#ifndef SHORTCUTSDIALOG_H
#define SHORTCUTSDIALOG_H

#include "ui_shortcutsdialog.h"
#include <QtGui/QShortcut>

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
    void restoreDefaultShortcut();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    QString getName(QObject*);
    void setKeySequence(QObject*, QKeySequence);
private:
    void createDefaultShortcut();
    void updateTableItem(QObject * action, QString key);
    QList<QObject*> listActions;
    QList<QString> listKeys;
    QMap<QString, QString> m_defaultShortcut;
};

}

#endif //SHORTCUTSDIALOG_H
