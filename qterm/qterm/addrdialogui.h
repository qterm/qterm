/****************************************************************************
** Form interface generated from reading ui file 'addrdialog.ui'
**
** Created: Wed Aug 18 21:58:17 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ADDRDIALOGUI_H
#define ADDRDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QListBox;
class QListBoxItem;
class QFrame;
class QTabWidget;
class QWidget;
class QLineEdit;
class QComboBox;
class QLabel;
class QCheckBox;
class QGroupBox;
class QButtonGroup;
class QRadioButton;

class addrDialogUI : public QDialog
{
    Q_OBJECT

public:
    addrDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~addrDialogUI();

    QPushButton* applyPushButton;
    QPushButton* connectPushButton;
    QPushButton* closePushButton;
    QListBox* nameListBox;
    QPushButton* deletePushButton;
    QPushButton* addPushButton;
    QFrame* Line2;
    QTabWidget* tabWidget;
    QWidget* Widget2;
    QLineEdit* userLineEdit;
    QLineEdit* postloginLineEdit;
    QLineEdit* preloginLineEdit;
    QLineEdit* passwdLineEdit;
    QComboBox* hostTypeComboBox;
    QLineEdit* addrLineEdit;
    QLineEdit* portLineEdit;
    QLineEdit* nameLineEdit;
    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QLabel* TextLabel3;
    QLabel* TextLabel3_6;
    QCheckBox* autoLoginCheckBox;
    QLabel* TextLabel1_2;
    QLabel* TextLabel4;
    QLabel* TextLabel5;
    QLabel* TextLabel11;
    QWidget* tab;
    QCheckBox* ansicolorCheckBox;
    QCheckBox* highlightCheckBox;
    QPushButton* bgcolorPushButton;
    QPushButton* fgcolorPushButton;
    QPushButton* schemaPushButton;
    QLabel* displayTextLabel;
    QCheckBox* autofontCheckBox;
    QLabel* TextLabel6;
    QComboBox* bbscodeComboBox;
    QLabel* TextLabel7;
    QComboBox* displaycodeComboBox;
    QPushButton* fontPushButton;
    QWidget* tab_2;
    QLineEdit* termtypeLineEdit;
    QLineEdit* escapeLineEdit;
    QLineEdit* columnLineEdit;
    QLabel* TextLabel3_4;
    QLabel* TextLabel1_5;
    QLabel* TextLabel6_3;
    QLabel* TextLabel4_3;
    QLineEdit* rowLineEdit;
    QComboBox* cursorTypeComboBox;
    QLabel* TextLabel5_3;
    QLabel* TextLabel1_4;
    QLabel* TextLabel2_5;
    QComboBox* keytypeComboBox;
    QLineEdit* scrollLineEdit;
    QWidget* tab_3;
    QGroupBox* GroupBox2;
    QComboBox* protocolComboBox;
    QLineEdit* sshuserLineEdit;
    QLineEdit* sshpasswdLineEdit;
    QLabel* TextLabel3_3;
    QLabel* TextLabel2_3;
    QLabel* TextLabel1_3;
    QGroupBox* GroupBox1;
    QLineEdit* proxyaddrLineEdit;
    QLineEdit* proxyuserLineEdit;
    QLineEdit* proxypasswdLineEdit;
    QLabel* TextLabel5_2;
    QLabel* TextLabel6_2;
    QLineEdit* proxyportLineEdit;
    QLabel* TextLabel4_2;
    QLabel* TextLabel3_2;
    QLabel* TextLabel2_2;
    QComboBox* proxytypeComboBox;
    QCheckBox* authCheckBox;
    QWidget* Widget3;
    QLabel* TextLabel9;
    QLabel* textLabel5;
    QLineEdit* idletimeLineEdit;
    QLabel* textLabel4;
    QLabel* textLabel2;
    QLabel* textLabel1;
    QCheckBox* reconnectCheckBox;
    QLineEdit* reconnectLineEdit;
    QLineEdit* retryLineEdit;
    QPushButton* scriptPushButton;
    QLineEdit* antiLineEdit;
    QCheckBox* scriptCheckBox;
    QCheckBox* replyCheckBox;
    QLabel* textLabel1_2;
    QLabel* TextLabel8;
    QLabel* TextLabel7_2;
    QLineEdit* scriptLineEdit;
    QLineEdit* replyLineEdit;
    QLineEdit* replykeyLineEdit;
    QWidget* tab_4;
    QButtonGroup* menuGroup;
    QRadioButton* radioButton1;
    QRadioButton* radioButton2;
    QRadioButton* radioButton3;
    QPushButton* menucolorButton;
    QLabel* menuLabel;

protected:

protected slots:
    virtual void languageChange();

};

#endif // ADDRDIALOGUI_H
