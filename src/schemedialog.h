#ifndef SCHEMADIALOG_H
#define SCHEMADIALOG_H

#include "ui_schemedialog.h"
namespace QTerm
{
class schemeDialog : public QDialog
{
    Q_OBJECT

public:
    schemeDialog(QWidget* parent = 0, Qt::WFlags fl = 0);
    ~schemeDialog();

    void setScheme(const QString&);
    QString getScheme();
    static QStringList loadSchemeList();

protected:
    //QColor clr0, clr1, clr2, clr3, clr4, clr5, clr6, clr7, clr8, clr9, clr10, clr11, clr12, clr13, clr14, clr15;
    QColor schemeColor[16];
    QColor fade;
    float alpha;
    QString pxmBg;
    int type;
    QString title;

    QStringList fileList;

    QString strCurrentScheme;

    bool bModified;
    int nLastItem;
private:
    Ui::schemeDialog ui;
    QButtonGroup bgBackground;

protected:
    void connectSlots();

    void updateList();
    void loadScheme(const QString& strSchemeFile);
    void saveNumScheme(int n = -1);

    void updateView();
    void updateBgPreview();
    void setBackgroundColor(QWidget * widget, const QColor & color);

    QImage& fadeColor(QImage&, float, const QColor&);

protected slots:
//  void clr0Clicked();
//  void clr1Clicked();
//  void clr2Clicked();
//  void clr3Clicked();
//  void clr4Clicked();
//  void clr5Clicked();
//  void clr6Clicked();
//  void clr7Clicked();
//  void clr8Clicked();
//  void clr9Clicked();
//  void clr10Clicked();
//  void clr11Clicked();
//  void clr12Clicked();
//  void clr13Clicked();
//  void clr14Clicked();
//  void clr15Clicked();
    void buttonClicked();
    void nameChanged(int);
    void bgType(int);
    void imageType(int);
    void chooseImage();
    void fadeClicked();
    void alphaChanged(int);
    void saveScheme();
    void removeScheme();
    void onOK();
    void onCancel();
    void textChanged(const QString&);
};

} // namespace QTerm

#endif // SCHEMADIALOG_H
