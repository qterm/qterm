/****************************************************************************
** Form interface generated from reading ui file 'articledialog.ui'
**
** Created: Mon Jan 6 21:18:57 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ARTICLEDIALOGUI_H
#define ARTICLEDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTextBrowser;

class articleDialogUI : public QDialog
{
    Q_OBJECT

public:
    articleDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~articleDialogUI();

    QTextBrowser* textBrowser2;
    QPushButton* saveButton;
    QPushButton* closeButton;

protected:
    QGridLayout* articleDialogUILayout;

protected slots:
    virtual void languageChange();
};

#endif // ARTICLEDIALOGUI_H
