/****************************************************************************
** Form interface generated from reading ui file 'prefdialog.ui'
**
** Created: Wed Aug 25 22:02:28 2004
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
class QSpacerItem;
class QCheckBox;
class QComboBox;
class QButtonGroup;
class QRadioButton;
class QLineEdit;
class QPushButton;
class QLabel;
class QFrame;

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
    QComboBox* ximComboBox;
    QCheckBox* aacheckBox;
    QCheckBox* trayCheckBox;
    QCheckBox* smartCheckBox;
    QCheckBox* blinkCheckBox;
    QButtonGroup* ButtonGroup1;
    QRadioButton* noneRadioButton;
    QRadioButton* fileRadioButton;
    QLineEdit* wavefileLineEdit;
    QPushButton* selectsoundPushButton;
    QRadioButton* beepRadioButton;
    QLabel* TextLabel1;
    QPushButton* okPushButton;
    QFrame* Line1;
    QPushButton* cancelPushButton;
    QLabel* TextLabel3;
    QLineEdit* httpLineEdit;
    QLineEdit* imageLineEdit;
    QLabel* textLabel1_2;
    QPushButton* imagePushButton;
    QLabel* textLabel1;
    QLabel* TextLabel1_2;
    QLineEdit* wordLineEdit3;
    QLabel* TextLabel2_2;
    QLineEdit* zmodemLineEdit;
    QLineEdit* poolLineEdit;
    QCheckBox* clearCheckBox;
    QPushButton* browsePushButton;
    QPushButton* poolPushButton;
    QPushButton* choosehttpPushButton;
    QLabel* textLabel1_3;

protected:

protected slots:
    virtual void languageChange();

};

#endif // PREFDIALOGUI_H
