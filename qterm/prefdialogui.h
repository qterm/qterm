/****************************************************************************
** Form interface generated from reading ui file 'prefdialog.ui'
**
** Created: Mon Aug 23 12:16:37 2004
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
class QLineEdit;
class QComboBox;
class QLabel;
class QButtonGroup;
class QRadioButton;
class QPushButton;
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
    QLineEdit* wordLineEdit3;
    QComboBox* ximComboBox;
    QCheckBox* aacheckBox;
    QCheckBox* trayCheckBox;
    QLabel* TextLabel2_2;
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
    QLabel* textLabel1;
    QLineEdit* httpLineEdit;
    QLineEdit* zmodemLineEdit;
    QPushButton* okPushButton;
    QPushButton* cancelPushButton;
    QFrame* Line1;
    QLabel* textLabel1_2;
    QLineEdit* imageLineEdit;
    QPushButton* browsePushButton;
    QPushButton* choosehttpPushButton;
    QPushButton* imagePushButton;

protected:

protected slots:
    virtual void languageChange();

};

#endif // PREFDIALOGUI_H
