/****************************************************************************
** Form interface generated from reading ui file 'quicklogin.ui'
**
** Created: Sat Dec 14 14:25:50 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QUICKDIALOG_H
#define QUICKDIALOG_H

#include "qtermparam.h"
#include "ui_quickdialog.h"
//Added by qt3to4:
//#include <QCloseEvent>

class QCloseEvent;

namespace QTerm
{
class Config;

class quickDialog : public QDialog
{
    Q_OBJECT

public:
    quickDialog(QWidget* parent = 0, Qt::WFlags fl = 0);
    ~quickDialog();

    Param param;

protected slots:
    void listChanged(int);
    void addAddr();
    void deleteAddr();
    void advOption();
    void connectIt();
    void close();

protected:
    void closeEvent(QCloseEvent *);
    void connectSlots();
    void loadHistory();

    Config * pConf;

private:
    Ui::quickDialog ui;
};

} // namespace QTerm

#endif // QUICKDIALOG_H
