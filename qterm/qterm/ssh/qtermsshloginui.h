/****************************************************************************
** Form interface generated from reading ui file 'fsshloginui.ui'
**
** Created: 三  2月 18 22:04:09 2004
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FSSHLOGINUI_H
#define FSSHLOGINUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QPushButton;
class QLineEdit;

class fSSHLoginUI : public QDialog
{
    Q_OBJECT

public:
    fSSHLoginUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~fSSHLoginUI();

    QLabel* textLabel1;
    QPushButton* bOK;
    QPushButton* bCancel;
    QLabel* textLabel2_2;
    QLineEdit* leUserName;
    QLineEdit* lePassword;
    QLabel* textLabel2;

protected:

protected slots:
    virtual void languageChange();

};

#endif // FSSHLOGINUI_H
