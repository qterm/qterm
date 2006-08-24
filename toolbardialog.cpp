#include "toolbardialog.h"

#include <QToolBar>
#include <QRegExp>
#include <QSettings>
#include <QMainWindow>

toolbarDialog :: toolbarDialog(QWidget* parent)
        : QDialog(parent)
{
    setupUi(this);

    // populate all available actions
    QList<QAction*> actions = parent->findChildren<QAction*>(QRegExp("action*"));
    QAction* action;
    foreach(action, actions)
    {
        QListWidgetItem* item = new QListWidgetItem(action->text());
        item->setIcon(action->icon());
        item->setData(Qt::UserRole, QVariant::fromValue((QObject*)action));
        listAllActions->addItem(item);
    }
    // Important to add special Separator
    listAllActions->addItem("Separator");
    
    QList<QToolBar*> toolbars = parent->findChildren<QToolBar*>();
    QToolBar* toolbar;
    foreach (toolbar, toolbars)
        if(toolbar->objectName()!="PagerBar")
            comboToolbars->addItem(toolbar->windowTitle(),
                    QVariant::fromValue((QObject*)toolbar));

    QMainWindow *mwParent = qobject_cast<QMainWindow*>(parent);

    comboButtonStyle->setCurrentIndex(int(mwParent->toolButtonStyle()));
    QVariant size(mwParent->iconSize());
    for(int index=0; index<comboIconSize->count(); index++)
        if(size.toString()==comboIconSize->itemText(index))
            comboIconSize->setCurrentIndex(index);

}

toolbarDialog::~toolbarDialog()
{
}

void toolbarDialog :: on_buttonAdd_clicked()
{
    // get the target toolar
    QVariant v;
    v = comboToolbars->itemData(comboToolbars->currentIndex());
    QToolBar *toolbar = qobject_cast<QToolBar*>(v.value<QObject*>());

    // get the action to be added
    QListWidgetItem* itemSrc=listAllActions->currentItem();
    if(itemSrc==NULL)
        return;
    v = itemSrc->data(Qt::UserRole);
    QAction *actionSrc =  qobject_cast<QAction*>(v.value<QObject*>());
    
    if(toolbar->actions().contains(actionSrc))
        return;

    // get the position to be inserted
    QListWidgetItem* itemPos = listUsedActions->currentItem();
    QAction *actionCurrent=0;
    QListWidgetItem* itemNew = new QListWidgetItem();

    if(itemPos!=0) //get the currentItem data if have one
    {
        v = itemPos->data(Qt::UserRole);
        actionCurrent =  qobject_cast<QAction*>(v.value<QObject*>());
    }
    if(itemSrc->text()=="Separator")
    {
        actionSrc = toolbar->insertSeparator(actionCurrent);
        itemNew->setText("Separator");
    }
    else
    {
        toolbar->insertAction(actionCurrent, actionSrc);
        itemNew->setText(actionSrc->text());
        itemNew->setIcon(actionSrc->icon());
    }
    itemNew->setData(Qt::UserRole, 
                QVariant::fromValue((QObject*)actionSrc));
    if(listUsedActions->currentRow()==-1) //append if nothing selected, consistent with QObject::insertAction();
        listUsedActions->addItem(itemNew);
    else
        listUsedActions->insertItem(listUsedActions->currentRow(), itemNew);
    listUsedActions->setCurrentItem(itemNew);
}

void toolbarDialog :: on_buttonRemove_clicked()
{
    // get the target toolar
    QVariant v;
    v = comboToolbars->itemData(comboToolbars->currentIndex());
    QToolBar *toolbar = qobject_cast<QToolBar*>(v.value<QObject*>());

    // get the action to be removed
    QListWidgetItem* itemPos = listUsedActions->currentItem();
    if(itemPos!=0) //get the currentItem data if have one
    {
        v = itemPos->data(Qt::UserRole);
        QAction *actionCurrent =  qobject_cast<QAction*>(v.value<QObject*>());
        toolbar->removeAction(actionCurrent);
        delete itemPos;
    }
    
}

void toolbarDialog :: on_buttonUp_clicked()
{
    // get the target toolar
    QVariant v;
    v = comboToolbars->itemData(comboToolbars->currentIndex());
    QToolBar *toolbar = qobject_cast<QToolBar*>(v.value<QObject*>());

    int index = listUsedActions->currentRow();
    if(index<=0) // either the first or nothing selected
        return;

    // take out current action/item
    QListWidgetItem* itemCurrent= listUsedActions->takeItem(index);    
    v = itemCurrent->data(Qt::UserRole);
    QAction *actionCurrent =  qobject_cast<QAction*>(v.value<QObject*>());
    toolbar->removeAction(actionCurrent);
    // insert it back
    QListWidgetItem* itemBefore= listUsedActions->item(index-1);    
    v = itemBefore->data(Qt::UserRole);
    QAction *actionBefore =  qobject_cast<QAction*>(v.value<QObject*>());
    toolbar->insertAction(actionBefore, actionCurrent);
    listUsedActions->insertItem(index-1,itemCurrent);
    listUsedActions->setCurrentItem(itemCurrent);
}
void toolbarDialog :: on_buttonDown_clicked()
{
    // get the target toolar
    QVariant v;
    v = comboToolbars->itemData(comboToolbars->currentIndex());
    QToolBar *toolbar = qobject_cast<QToolBar*>(v.value<QObject*>());

    int index = listUsedActions->currentRow();
    if(index<0||index==listUsedActions->count()-1) // either the last or nothing selected
        return;

    // take out action/item behind
    QListWidgetItem* itemBehind= listUsedActions->takeItem(index+1);
    v = itemBehind->data(Qt::UserRole);
    QAction *actionBehind =  qobject_cast<QAction*>(v.value<QObject*>());
    toolbar->removeAction(actionBehind);
    
    // insert it back
    QListWidgetItem* itemCurrent= listUsedActions->item(index);    
    v = itemCurrent->data(Qt::UserRole);
    QAction *actionCurrent =  qobject_cast<QAction*>(v.value<QObject*>());
    toolbar->insertAction(actionCurrent, actionBehind);
    listUsedActions->insertItem(index,itemBehind);
}

void toolbarDialog :: on_comboButtonStyle_currentIndexChanged(int index)
{
    QMainWindow *parent = qobject_cast<QMainWindow*>(parentWidget());
    parent->setToolButtonStyle(Qt::ToolButtonStyle(index));
}

void toolbarDialog :: on_comboIconSize_currentIndexChanged(const QString& item)
{
    QVariant v;
    v.setValue(item);
    QMainWindow *parent = qobject_cast<QMainWindow*>(parentWidget());
    parent->setIconSize(v.toSize());
}

void toolbarDialog :: on_comboToolbars_currentIndexChanged(int index)
{
    listUsedActions->clear();
    QToolBar *toolbar = qobject_cast<QToolBar*>(comboToolbars->itemData(index).value<QObject*>());
    foreach(QAction *action, toolbar->actions())
    {
        QListWidgetItem* item = new QListWidgetItem();
        if(action->isSeparator())
            item->setText("Separator");
        else
        {
            item->setText(action->text());
            item->setIcon(action->icon());
        }
        item->setData(Qt::UserRole, 
                QVariant::fromValue((QObject*)action));
        listUsedActions->addItem(item);
    }
}
