/****************************************************************************
** Form interface generated from reading ui file 'prefdialog.ui'
**
** Created: Fri Jun 20 16:28:33 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PREFDIALOGUI_H
#define PREFDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;

class prefDialogUI : public QDialog
{
    Q_OBJECT

public:
    prefDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~prefDialogUI();

    QLabel* TextLabel2_2;
    QCheckBox* wheelCheckBox;
    QCheckBox* saveCheckBox;
    QCheckBox* warnCheckBox;
    QCheckBox* urlCheckBox;
    QLineEdit* wordLineEdit3;
    QCheckBox* smartCheckBox;
    QLabel* TextLabel1_2;
    QLabel* TextLabel3;
    QPushButton* okPushButton;
    QLineEdit* httpLineEdit;
    QFrame* Line1;
    QPushButton* cancelPushButton;
    QPushButton* choosehttpPushButton;
    QComboBox* ximComboBox;
    QButtonGroup* ButtonGroup1;
    QRadioButton* noneRadioButton;
    QRadioButton* beepRadioButton;
    QLineEdit* wavefileLineEdit;
    QRadioButton* fileRadioButton;
    QPushButton* selectsoundPushButton;
    QLabel* TextLabel1;
    QCheckBox* aacheckBox;
    QCheckBox* blinkCheckBox;
    QCheckBox* trayCheckBox;

protected:

protected slots:
    virtual void languageChange();
};

#endif // PREFDIALOGUI_H
