/****************************************************************************
** Form interface generated from reading ui file 'schemadialog.ui'
**
** Created: Thu Nov 6 15:51:56 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SCHEMADIALOGUI_H
#define SCHEMADIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QSlider;
class QComboBox;

class schemaDialogUI : public QDialog
{
    Q_OBJECT

public:
    schemaDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~schemaDialogUI();

    QLabel* textLabel1_2;
    QLineEdit* titleLineEdit;
    QListBox* nameListBox;
    QPushButton* clr0Button;
    QPushButton* clr1Button;
    QPushButton* clr2Button;
    QPushButton* clr3Button;
    QPushButton* clr4Button;
    QPushButton* clr5Button;
    QPushButton* clr6Button;
    QPushButton* clr7Button;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QPushButton* clr8Button;
    QPushButton* clr9Button;
    QPushButton* clr10Button;
    QPushButton* clr11Button;
    QPushButton* clr12Button;
    QPushButton* clr13Button;
    QPushButton* clr14Button;
    QPushButton* clr15Button;
    QButtonGroup* bgButtonGroup;
    QLabel* TextLabel3;
    QPushButton* chooseButton;
    QLabel* TextLabel2;
    QLineEdit* imageLineEdit;
    QRadioButton* imageRadioButton;
    QRadioButton* noneRadioButton;
    QRadioButton* transpRadioButton;
    QLabel* TextLabel1;
    QPushButton* fadeButton;
    QSlider* alphaSlider;
    QLabel* imagePixmapLabel;
    QComboBox* typeComboBox;
    QPushButton* removeButton;
    QPushButton* saveButton;
    QPushButton* okButton;
    QPushButton* cancelButton;

public slots:
    virtual void Frame4_destroyed( QObject * );

protected:

protected slots:
    virtual void languageChange();

};

#endif // SCHEMADIALOGUI_H
