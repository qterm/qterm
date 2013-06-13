/****************************************************************************
** Form implementation generated from reading ui file 'articledialog.ui'
**
** Created: Sun Jan 5 19:46:42 2003
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "articledialog.h"
#include <QFileDialog>
#include <QMessageBox>
namespace QTerm
{
/*
 *  Constructs a articleDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
articleDialog::articleDialog(QWidget* parent, Qt::WindowFlags fl)
        : QDialog(parent, fl)

{
    ui.setupUi(this);
    connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(onSave()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
articleDialog::~articleDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void articleDialog::onSave()
{
    QFileDialog fileDlg;
    QString filename  = fileDlg.getSaveFileName();

    if (!filename.isNull()) {
        QFile f(filename);
        if ((f.open(QIODevice::WriteOnly))) {
            f.write(strArticle.toLocal8Bit());
            f.close();
        } else {
            QMessageBox mb("Access file error:",
                           filename,
                           QMessageBox::Warning,
                           0, 0,
                           0, this, 0);
            mb.exec();
        }
    }
}

} // namespace QTerm

#include <moc_articledialog.cpp>
