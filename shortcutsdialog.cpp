#include "shortcutsdialog.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QShortcut>

shortcutsDialog :: shortcutsDialog(QWidget* parent,
        QList<QAction*> actions,
        QList<QShortcut*> shortcuts)
    : QDialog(parent)
{
	setupUi(this);
	editShortcut->installEventFilter(this);
    buttonAssign->setEnabled(false);
    
    foreach(QAction* action, actions)
    {
        QTableWidgetItem *itemAction=new QTableWidgetItem(action->text());
        QString shortcut = action->shortcut().toString();
        QTableWidgetItem *itemShortcut=new QTableWidgetItem(shortcut);
        listKeys << shortcut;
        listActions << action;
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, itemAction);
        tableWidget->setItem(row, 1, itemShortcut);
    }
    foreach(QShortcut* shortcut, shortcuts)
    {
        QTableWidgetItem *itemShortcut=new QTableWidgetItem(shortcut->objectName());
        QString key = shortcut->key().toString();
        QTableWidgetItem *itemKey=new QTableWidgetItem(key);
        listKeys << key;
        listActions << shortcut;
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, itemShortcut);
        tableWidget->setItem(row, 1, itemKey);        
    }
    tableWidget->resizeColumnsToContents();
    tableWidget->setCurrentCell(0,0);
}

void shortcutsDialog :: on_buttonAssign_clicked()
{
    QTableWidgetItem* item;
    
    int row = listKeys.indexOf(editShortcut->text());

    if(row==tableWidget->currentRow())
        return;
    
    // already existed    
    if(row!=-1)
    {   
        QObject* obj=listActions.at(row);
        int aw = QMessageBox::warning(this, tr("Assign Shortcut"),
        QString(tr("shortcut %1 is already assigned to %2\n"
                    "Do you want to clear and reassign?"))
                .arg(editShortcut->text())
                .arg(getName(obj)),
                QMessageBox::Yes,
                QMessageBox::No|QMessageBox::Default);
        if(aw==QMessageBox::No)
            return;
        // clear it
        item = tableWidget->item(row,1);
        item->setText("");
        listKeys.replace(row,"");
        setKeySequence(obj,QKeySequence());
    }
    // set it to the target action
    row = tableWidget->currentRow();
    item = tableWidget->item(row,1);
    item->setText(editShortcut->text());
    listKeys.replace(row, editShortcut->text());
    setKeySequence(listActions.at(row),editShortcut->text());
}

void shortcutsDialog :: on_buttonRemove_clicked()
{
    int row = tableWidget->currentRow();
    // update table
    QTableWidgetItem* item = tableWidget->item(row,1);
    item->setText("");
    // update listKeys
    listKeys.replace(row,"");
    // update action
    setKeySequence(listActions.at(row),QKeySequence());    
}

void shortcutsDialog :: on_editShortcut_textChanged(const QString& shortcut)
{
    buttonAssign->setEnabled(!shortcut.isEmpty());
}

void shortcutsDialog :: on_buttonDone_clicked()
{
}
void shortcutsDialog :: on_tableWidget_currentCellChanged(int row, int col,int,int)
{
    QTableWidgetItem *item = tableWidget->item(row, 1);
    buttonRemove->setEnabled(!item->text().isEmpty());
}
bool shortcutsDialog :: eventFilter(QObject *object, QEvent *event)
{
    if (object!=editShortcut || event->type()!=QEvent::KeyPress)
        return false;

    QKeyEvent* keyevent = (QKeyEvent*)event;
    int key=keyevent->key();
    Qt::KeyboardModifiers mod = keyevent->modifiers();
    
    // dont accept Mdodifiers only
    if(key==Qt::Key_Meta||key==Qt::Key_Alt || key==Qt::Key_AltGr
        || key==Qt::Key_Shift || key==Qt::Key_Control)
        return false;
    // TODO: Shift+1 yields Shift+!, a bit confusing
    // because Shift+1 is translated to Qt::Key, not raw scancode
    // I dono how to do
    QString text="";
    if(mod != Qt::NoModifier)
    {
        QKeySequence seqMod(mod);
        text=seqMod.toString(QKeySequence::NativeText);
    }
    QKeySequence seqKey(key);
    text+=seqKey.toString(QKeySequence::NativeText);
    
    editShortcut->setText(text);

    return true;
}

QString shortcutsDialog :: getName(QObject* obj)
{
    if(obj->inherits("QAction"))
    {
        QAction* action=qobject_cast<QAction*>(obj);
        return action->text();
    }
    else if(obj->inherits("QShortcut"))
        return obj->objectName();
}

void shortcutsDialog :: setKeySequence(QObject* obj, QKeySequence key)
{
    if(obj->inherits("QAction"))
    {
        QAction* action=qobject_cast<QAction*>(obj);
        action->setShortcut(key);
    }
    else if(obj->inherits("QShortcut"))
    {
        QShortcut* shortcut=qobject_cast<QShortcut*>(obj);
        shortcut->setKey(key);
    }
}
