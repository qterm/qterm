/****************************************************************************
** Form interface generated from reading ui file 'msgdialog.ui'
**
** Created: Wed Mar 19 18:39:10 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MSGDIALOGUI_H
#define MSGDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTextBrowser;

class msgDialogUI : public QDialog
{
    Q_OBJECT

public:
    msgDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~msgDialogUI();

    QTextBrowser* msgBrowser;
    QPushButton* okButton;

protected:
    QGridLayout* msgDialogUILayout;

protected slots:
    virtual void languageChange();
};

#endif // MSGDIALOGUI_H
