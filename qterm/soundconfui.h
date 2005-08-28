/****************************************************************************
** Form interface generated from reading ui file 'soundconf.ui'
**
** Created: 日  8月 28 16:27:08 2005
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FSOUNDCONFUI_H
#define FSOUNDCONFUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QButtonGroup;
class QRadioButton;

class fSoundConfUI : public QDialog
{
    Q_OBJECT

public:
    fSoundConfUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~fSoundConfUI();

    QGroupBox* groupBox2;
    QPushButton* bfSelect;
    QLineEdit* leFile;
    QGroupBox* groupBox3;
    QPushButton* bpSelect;
    QLineEdit* leProg;
    QPushButton* bOK;
    QButtonGroup* bgMethod;
    QRadioButton* radioButton13;
    QRadioButton* radioButton14;
    QRadioButton* radioButton15;
    QRadioButton* radioButton16;
    QPushButton* bpTest;
    QPushButton* bCancel;

public slots:
    virtual void onSelectFile() = 0;
    virtual void onPlayMethod( int id ) = 0;
    virtual void onSelectProg() = 0;
    virtual void onTestPlay() = 0;

protected:

protected slots:
    virtual void languageChange();

};

#endif // FSOUNDCONFUI_H
