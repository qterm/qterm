/****************************************************************************
** Form interface generated from reading ui file 'aboutdialog.ui'
**
** Created: Sat Nov 13 22:38:36 2004
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
class QSpacerItem;
class QLabel;
class QTabWidget;
class QWidget;
class QTextBrowser;
class QPushButton;

class aboutDialogUI : public QDialog
{
    Q_OBJECT

public:
    aboutDialogUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~aboutDialogUI();

    QLabel* pixmapLabel;
    QLabel* TextLabel;
    QTabWidget* mainTabWidget;
    QWidget* tab;
    QLabel* TextLabel3;
    QWidget* tab_2;
    QLabel* TextLabel1;
    QWidget* tab_3;
    QTextBrowser* TextBrowser2;
    QPushButton* closeButton;

protected:

protected slots:
    virtual void languageChange();

};

#endif // ABOUTDIALOGUI_H
