/****************************************************************************
** Form interface generated from reading ui file 'fconfigure.ui'
**
** Created: 三  9月 24 09:44:41 2003
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
class QGroupBox;
class QLineEdit;
class QPushButton;
class QButtonGroup;
class QRadioButton;

class fSoundConfUI : public QDialog
{
    Q_OBJECT

public:
    fSoundConfUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~fSoundConfUI();

    QGroupBox* groupBox2;
    QLineEdit* leFile;
    QPushButton* bfSelect;
    QGroupBox* groupBox3;
    QPushButton* bpSelect;
    QLineEdit* leProg;
    QButtonGroup* bgMethod;
    QRadioButton* radioButton16;
    QRadioButton* radioButton14;
    QRadioButton* radioButton15;
    QRadioButton* radioButton13;
    QPushButton* bOK;
    QPushButton* bCancel;

public slots:
    virtual void onSelectFile() = 0;
    virtual void onSelectProg() = 0;
    virtual void onPlayMethod( int id ) = 0;

protected:

protected slots:
    virtual void languageChange();

};

#endif // FSOUNDCONFUI_H
