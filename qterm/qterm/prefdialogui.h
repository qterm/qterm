/****************************************************************************
** Form interface generated from reading ui file 'prefdialog.ui'
**
** Created: Mon Jan 20 17:29:08 2003
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
    QLineEdit* wordLineEdit3;
    QLabel* TextLabel1_2;
    QLabel* TextLabel1;
    QComboBox* ximComboBox;
    QCheckBox* wheelCheckBox;
    QCheckBox* urlCheckBox;
    QCheckBox* saveCheckBox;
    QCheckBox* blinkCheckBox;
    QButtonGroup* ButtonGroup1;
    QRadioButton* noneRadioButton;
    QRadioButton* beepRadioButton;
    QRadioButton* fileRadioButton;
    QPushButton* selectsoundPushButton;
    QLineEdit* wavefileLineEdit;
    QCheckBox* smartCheckBox;
    QPushButton* okPushButton;
    QPushButton* cancelPushButton;
    QFrame* Line1;
    QPushButton* choosehttpPushButton;
    QLineEdit* httpLineEdit;
    QLabel* TextLabel3;
    QCheckBox* aacheckBox;
    QCheckBox* warnCheckBox;

protected:

protected slots:
    virtual void languageChange();
};

#endif // PREFDIALOGUI_H
