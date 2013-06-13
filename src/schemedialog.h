#ifndef SCHEMADIALOG_H
#define SCHEMADIALOG_H

#include "ui_schemedialog.h"
namespace QTerm
{
class schemeDialog : public QDialog
{
    Q_OBJECT

public:
    schemeDialog(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    ~schemeDialog();

    void setScheme(const QString&);
    QString getScheme();
    static QStringList loadSchemeList();

protected:
    QColor schemeColor[16];
    QString title;

    QStringList fileList;

    QString strCurrentScheme;

    bool bModified;
    int nLastItem;
private:
    Ui::schemeDialog ui;

protected:
    void connectSlots();

    void updateList();
    void loadScheme(const QString& strSchemeFile);
    void saveNumScheme(int n = -1);

    void updateView();
    void setBackgroundColor(QPushButton * widget, const QColor & color);

protected slots:
    void buttonClicked();
    void nameChanged(int);
    void saveScheme();
    void resetScheme();
    void removeScheme();
    void onOK();
    void onCancel();
    void textChanged(const QString&);
};

} // namespace QTerm

#endif // SCHEMADIALOG_H
