/****************************************************************************
** Form interface generated from reading ui file 'aboutdialog.ui'
**
** Created: Wed Apr 2 14:52:53 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ABOUTDIALOGUI_H
#define ABOUTDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;
class QTabWidget;
class QTextBrowser;
class QWidget;

class aboutDialogUI : public QDialog
{
    Q_OBJECT

public:
    aboutDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~aboutDialogUI();

    QLabel* TextLabel;
    QPushButton* closeButton;
    QTabWidget* mainTabWidget;
    QWidget* tab;
    QLabel* TextLabel3;
    QWidget* tab_2;
    QLabel* TextLabel1;
    QWidget* tab_3;
    QTextBrowser* TextBrowser2;
    QLabel* pixmapLabel;

protected:

protected slots:
    virtual void languageChange();
};

#endif // ABOUTDIALOGUI_H
