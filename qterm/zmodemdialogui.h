/****************************************************************************
** Form interface generated from reading ui file 'zmodemdialogui.ui'
**
** Created: Wed Aug 18 19:57:45 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ZMODEMDIALOGUI_H
#define ZMODEMDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QProgressBar;
class QLabel;
class QPushButton;
class QTextBrowser;

class zmodemDialogUI : public QDialog
{
    Q_OBJECT

public:
    zmodemDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~zmodemDialogUI();

    QProgressBar* pbProgress;
    QLabel* labelStatus;
    QPushButton* buttonCancel;
    QTextBrowser* browserError;
    QLabel* labelFileName;

protected:
    QGridLayout* zmodemDialogUILayout;
    QSpacerItem* spacer2;
    QSpacerItem* spacer1;

protected slots:
    virtual void languageChange();

};

#endif // ZMODEMDIALOGUI_H
