#include "shortcutsdialog.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QShortcut>

namespace QTerm {

ShortcutsDialog::ShortcutsDialog(QWidget* parent, QList<QAction*> actions, QList<QShortcut*> shortcuts)
        :QDialog(parent),m_defaultShortcut()
{
    setupUi(this);
    editShortcut->installEventFilter(this);
    buttonAssign->setEnabled(false);
    connect(buttonAssign, SIGNAL(clicked()), this, SLOT(buttonAssignClicked()));
    connect(buttonRemove, SIGNAL(clicked()), this, SLOT(buttonRemoveClicked()));
    connect(buttonDone, SIGNAL(clicked()), this, SLOT(buttonDoneClicked()));
    connect(buttonDefault, SIGNAL(clicked()), this, SLOT(restoreDefaultShortcut()));
    connect(editShortcut, SIGNAL(textChanged(const QString &)), this, SLOT(editShortcutTextChanged(const QString &)));
    connect(tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(tableWidgetCurrentCellChanged(int,int,int,int)));

    createDefaultShortcut();
    foreach(QAction* action, actions) {
        //toolTip() gives text without '&'
        if (action->actionGroup() != 0)
            continue;
        QTableWidgetItem *itemAction = new QTableWidgetItem(action->toolTip());
        QString shortcut = action->shortcut().toString();
        QTableWidgetItem *itemShortcut = new QTableWidgetItem(shortcut);
        listKeys << shortcut;
        listActions << action;
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, itemAction);
        tableWidget->setItem(row, 1, itemShortcut);
    }
    foreach(QShortcut* shortcut, shortcuts) {
        QTableWidgetItem *itemShortcut = new QTableWidgetItem(shortcut->objectName());
        QString key = shortcut->key().toString();
        QTableWidgetItem *itemKey = new QTableWidgetItem(key);
        listKeys << key;
        listActions << shortcut;
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, itemShortcut);
        tableWidget->setItem(row, 1, itemKey);
    }
    tableWidget->resizeColumnsToContents();
    tableWidget->setCurrentCell(0, 0);
}

void ShortcutsDialog::createDefaultShortcut()
{
    m_defaultShortcut.insert("actionAbout","F1");
    m_defaultShortcut.insert("actionAddress","F2");
    m_defaultShortcut.insert("actionQuickConnect","F3");
    m_defaultShortcut.insert("actionRefresh","F5");
    m_defaultShortcut.insert("actionFull","F6");
    m_defaultShortcut.insert("actionScriptRun","F7");
    m_defaultShortcut.insert("actionScriptStop","F8");
    m_defaultShortcut.insert("actionCopyArticle","F9");
    m_defaultShortcut.insert("actionViewMessage","F10");
    m_defaultShortcut.insert("actionBoss","F12");
    m_defaultShortcut.insert("actionCopy","Ctrl+Ins");
    m_defaultShortcut.insert("actionPaste","Shift+Ins");
}

void ShortcutsDialog::buttonAssignClicked()
{
    int row = listKeys.indexOf(editShortcut->text());

    if (row == tableWidget->currentRow())
        return;

    // already existed
    if (row != -1) {
        QObject* obj = listActions.at(row);
        int aw = QMessageBox::warning(this, tr("Assign Shortcut"),
                                      QString(tr("shortcut %1 is already assigned to %2\n"
                                                 "Do you want to clear and reassign?"))
                                      .arg(editShortcut->text())
                                      .arg(getName(obj)),
                                      QMessageBox::Yes,
                                      QMessageBox::No | QMessageBox::Default);
        if (aw == QMessageBox::No)
            return;
        // clear it
        setKeySequence(obj, QKeySequence());
    }
    // set it to the target action
    row = tableWidget->currentRow();
    setKeySequence(listActions.at(row), editShortcut->text());
}

void ShortcutsDialog::buttonRemoveClicked()
{
    int row = tableWidget->currentRow();
    setKeySequence(listActions.at(row), QKeySequence());
}

void ShortcutsDialog::editShortcutTextChanged(const QString& shortcut)
{
    buttonAssign->setEnabled(!shortcut.isEmpty());
}

void ShortcutsDialog::buttonDoneClicked()
{
}

void ShortcutsDialog::restoreDefaultShortcut()
{
    foreach(QObject * action, listActions) {
        if (m_defaultShortcut.contains(action->objectName())) {
            setKeySequence(action, QKeySequence(m_defaultShortcut.value(action->objectName())));
        } else {
            setKeySequence(action, QKeySequence());
        }
    }
    editShortcut->setText(listKeys.at(tableWidget->currentRow()));
}

void ShortcutsDialog::updateTableItem(QObject * action, QString key)
{
    QTableWidgetItem* item;
    int row = listActions.indexOf(action);
    item = tableWidget->item(row, 1);
    item->setText(key);
    listKeys.replace(row, key);
}

void ShortcutsDialog::tableWidgetCurrentCellChanged(int row, int col, int, int)
{
    QTableWidgetItem *item = tableWidget->item(row, 1);
    buttonRemove->setEnabled(!item->text().isEmpty());
    editShortcut->setText(item->text());
}
bool ShortcutsDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object != editShortcut || event->type() != QEvent::KeyPress)
        return false;

    QKeyEvent* keyevent = (QKeyEvent*)event;
    int key = keyevent->key();
    Qt::KeyboardModifiers mod = keyevent->modifiers();

    // dont accept Mdodifiers only
    if (key == Qt::Key_Meta || key == Qt::Key_Alt || key == Qt::Key_AltGr
            || key == Qt::Key_Shift || key == Qt::Key_Control)
        return false;
    // TODO: Shift+1 yields Shift+!, a bit confusing
    // because Shift+1 is translated to Qt::Key, not raw scancode
    // I dono how to do
    QString text = "";
    if (mod != Qt::NoModifier) {
        QKeySequence seqMod(mod);
        text = seqMod.toString(QKeySequence::NativeText);
    }
    QKeySequence seqKey(key);
    text += seqKey.toString(QKeySequence::NativeText);

    editShortcut->setText(text);

    return true;
}

QString ShortcutsDialog::getName(QObject* obj)
{
    if (obj->inherits("QAction")) {
        QAction* action = qobject_cast<QAction*>(obj);
        return action->text();
    } else if (obj->inherits("QShortcut"))
        return obj->objectName();
    return QString();
}

void ShortcutsDialog::setKeySequence(QObject* obj, QKeySequence key)
{
    if (obj->inherits("QAction")) {
        QAction* action = qobject_cast<QAction*>(obj);
        action->setShortcut(key);
    } else if (obj->inherits("QShortcut")) {
        QShortcut* shortcut = qobject_cast<QShortcut*>(obj);
        shortcut->setKey(key);
    }
    updateTableItem(obj, key.toString());
}

}

#include <shortcutsdialog.moc>
