/****************************************************************************
** Form interface generated from reading ui file 'keydialog.ui'
**
** Created: Mon Jan 6 21:19:31 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KEYDIALOGUI_H
#define KEYDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QLabel;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QPushButton;
class QRadioButton;

class keyDialogUI : public QDialog
{
    Q_OBJECT

public:
    keyDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~keyDialogUI();

    QButtonGroup* buttonGroup2;
    QRadioButton* radioButton1;
    QLineEdit* keyEdit;
    QPushButton* downButton;
    QPushButton* enterButton;
    QPushButton* rightButton;
    QPushButton* leftButton;
    QPushButton* upButton;
    QLineEdit* scriptEdit;
    QRadioButton* radioButton3;
    QRadioButton* radioButton2;
    QLineEdit* programEdit;
    QLabel* textLabel3;
    QPushButton* scriptButton;
    QPushButton* programButton;
    QLineEdit* nameEdit;
    QLabel* textLabel1;
    QListBox* nameListBox;
    QPushButton* addButton;
    QPushButton* deleteButton;
    QPushButton* updateButton;
    QPushButton* closeButton;

protected:

protected slots:
    virtual void languageChange();
};

#endif // KEYDIALOGUI_H
