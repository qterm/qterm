/****************************************************************************
** Form interface generated from reading ui file 'prefdialog.ui'
**
** Created: 五  8月 26 16:10:14 2005
**      by: The User Interface Compiler ()
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
class QLabel;
class QFrame;
class QLineEdit;
class QButtonGroup;
class QRadioButton;
class QPushButton;

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
    QLabel* TextLabel1;
    QFrame* Line1;
    QLabel* TextLabel3;
    QLineEdit* httpLineEdit;
    QLineEdit* imageLineEdit;
    QLabel* textLabel1_2;
    QLabel* textLabel1;
    QLabel* TextLabel1_2;
    QLineEdit* wordLineEdit3;
    QLabel* TextLabel2_2;
    QLineEdit* zmodemLineEdit;
    QLineEdit* poolLineEdit;
    QCheckBox* clearCheckBox;
    QLabel* textLabel1_3;
    QButtonGroup* ButtonGroup1;
    QRadioButton* noneRadioButton;
    QLineEdit* wavefileLineEdit;
    QRadioButton* fileRadioButton;
    QRadioButton* beepRadioButton;
    QPushButton* selectsoundPushButton;
    QPushButton* cancelPushButton;
    QPushButton* okPushButton;
    QPushButton* browsePushButton;
    QPushButton* poolPushButton;
    QPushButton* imagePushButton;
    QPushButton* choosehttpPushButton;

protected:

protected slots:
    virtual void languageChange();

};

#endif // PREFDIALOGUI_H
