/****************************************************************************
** Form interface generated from reading ui file 'prefdialog.ui'
**
** Created: Wed Jul 21 16:07:37 2004
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
class QCheckBox;
class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;
class QFrame;
class QButtonGroup;
class QRadioButton;

class prefDialogUI : public QDialog
{
    Q_OBJECT

public:
    prefDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~prefDialogUI();

    QCheckBox* wheelCheckBox;
    QCheckBox* saveCheckBox;
    QCheckBox* warnCheckBox;
    QCheckBox* urlCheckBox;
    QLineEdit* wordLineEdit3;
    QComboBox* ximComboBox;
    QCheckBox* aacheckBox;
    QCheckBox* trayCheckBox;
    QLabel* TextLabel2_2;
    QPushButton* okPushButton;
    QFrame* Line1;
    QCheckBox* smartCheckBox;
    QLabel* TextLabel1_2;
    QCheckBox* blinkCheckBox;
    QButtonGroup* ButtonGroup1;
    QRadioButton* noneRadioButton;
    QRadioButton* fileRadioButton;
    QLineEdit* wavefileLineEdit;
    QPushButton* selectsoundPushButton;
    QRadioButton* beepRadioButton;
    QLabel* TextLabel1;
    QLabel* TextLabel3;
    QPushButton* choosehttpPushButton;
    QLabel* textLabel1;
    QPushButton* cancelPushButton;
    QPushButton* browsePushButton;
    QLineEdit* httpLineEdit;
    QLineEdit* zmodemLineEdit;

protected:

protected slots:
    virtual void languageChange();

};

#endif // PREFDIALOGUI_H
