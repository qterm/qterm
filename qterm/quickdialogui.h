/****************************************************************************
** Form interface generated from reading ui file 'quickdialog.ui'
**
** Created: Mon Apr 7 18:53:48 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QUICKDIALOGUI_H
#define QUICKDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

class quickDialogUI : public QDialog
{
    Q_OBJECT

public:
    quickDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~quickDialogUI();

    QPushButton* deletePushButton;
    QPushButton* advPushButton;
    QPushButton* connectPushButton;
    QPushButton* closePushButton;
    QComboBox* historyComboBox;
    QLabel* TextLabel6;
    QLineEdit* portLineEdit;
    QLabel* TextLabel4;
    QLabel* TextLabel5;
    QLineEdit* addrLineEdit;
    QPushButton* addPushButton;

protected:

protected slots:
    virtual void languageChange();
};

#endif // QUICKDIALOGUI_H
