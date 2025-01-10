//
// C++ Interface: CloseDialog
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLOSEDIALOG_H
#define CLOSEDIALOG_H

#include "ui_closedialog.h"
class QTimer;
namespace QTerm
{
class CloseDialog : public QDialog
{
    Q_OBJECT

public:
    CloseDialog(QWidget* parent = 0, Qt::WindowFlags fl = Qt::Widget);
    ~CloseDialog();
    void setSiteList(const QStringList & list);
    Ui::closeDialog ui;

private slots:
    void changeTitle();
private:
    QTimer * m_timer;
    int m_count;

};

} // namespace QTerm

#endif // CLOSEDIALOG_H
