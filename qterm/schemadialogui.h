/****************************************************************************
** Form interface generated from reading ui file 'schemadialog.ui'
**
** Created: Wed Mar 26 18:19:52 2003
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
class QButtonGroup;
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QPushButton;
class QRadioButton;
class QSlider;

class schemaDialogUI : public QDialog
{
    Q_OBJECT

public:
    schemaDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~schemaDialogUI();

    QLabel* textLabel2;
    QLabel* textLabel1;
    QFrame* fg0Frame;
    QFrame* fg1Frame;
    QFrame* fg2Frame;
    QFrame* fg3Frame;
    QFrame* fg4Frame;
    QFrame* fg5Frame;
    QFrame* fg6Frame;
    QFrame* fg7Frame;
    QFrame* bg0Frame;
    QFrame* bg1Frame;
    QFrame* bg2Frame;
    QFrame* bg3Frame;
    QFrame* bg4Frame;
    QFrame* bg5Frame;
    QFrame* bg6Frame;
    QFrame* bg7Frame;
    QPushButton* saveButton;
    QPushButton* removeButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QButtonGroup* bgButtonGroup;
    QLabel* TextLabel3;
    QRadioButton* noneRadioButton;
    QRadioButton* imageRadioButton;
    QComboBox* typeComboBox;
    QLineEdit* imageLineEdit;
    QPushButton* chooseButton;
    QRadioButton* transpRadioButton;
    QLabel* TextLabel1;
    QFrame* shadeFrame;
    QLabel* TextLabel2;
    QSlider* alphaSlider;
    QLabel* imagePixmapLabel;
    QListBox* nameListBox;

public slots:
    virtual void Frame4_destroyed( QObject * );

protected:

protected slots:
    virtual void languageChange();
};

#endif // SCHEMADIALOGUI_H
