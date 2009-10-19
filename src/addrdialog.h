/****************************************************************************
** Form interface generated from reading ui file 'addrdialog.ui'
**
** Created: Sun Dec 15 20:55:11 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ADDRDIALOG_H
#define ADDRDIALOG_H

#include "qtermparam.h"
#include "ui_addrdialog.h"
#include <QButtonGroup>
namespace QTerm
{
class Config;

class addrDialog : public QDialog
{
    Q_OBJECT

public:
    addrDialog(QWidget* parent = 0, bool partial = false, Qt::WFlags fl = 0);
    ~addrDialog();

    Param param;
    void updateData(bool save);

protected slots:
    void onNamechange(int);
    void onAdd();
    void onDelete();
    void onApply();
    void onConnect();
    void onClose();
    void onReset();
    void onConfigScheme();
    void onScheme(int);
    void onKeyboardProfile(int);
    void onProtocol(int);
    void onChooseScript();
    void onMenuColor();
    void onASCIIFont(const QFont & font);
    void onGeneralFont(const QFont & font);
    void onFontSize(int size);

protected:
    void connectSlots();
    bool isChanged();
    void setLabelPixmap();
    void setMenuPixmap();
    void updateSchemeList();
    void updateKeyboardProfiles();

    bool bPartial;
    QString strASCIIFontName;
    QString strGeneralFontName;
    int nFontSize;
    QString strSchemeFile;
    QString strKeyboardProfile;
    QColor clrMenu;
    QButtonGroup bgMenu;
    QStringList schemeFileList;
    QStringList profileList;

    int nLastItem;

public:
    Ui::addrDialog ui;
};

} // namespace QTerm

#endif // ADDRDIALOG_H
