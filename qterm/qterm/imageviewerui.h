/****************************************************************************
** Form interface generated from reading ui file 'browser.ui'
**
** Created: 一  8月 22 19:32:06 2005
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QTERMIMAGEUI_H
#define QTERMIMAGEUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTermCanvas;
class QPushButton;

class QTermImageUI : public QDialog
{
    Q_OBJECT

public:
    QTermImageUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QTermImageUI();

    QTermCanvas* d_canvas;
    QPushButton* d_previous;
    QPushButton* d_browser;
    QPushButton* d_next;

protected:
    QGridLayout* QTermImageUILayout;
    QHBoxLayout* layout2;
    QSpacerItem* spacer3_3;
    QSpacerItem* spacer3;
    QSpacerItem* spacer3_2;
    QSpacerItem* spacer4;

protected slots:
    virtual void languageChange();

};

#endif // QTERMIMAGEUI_H
