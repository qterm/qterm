#ifndef IMAGEVIERWER_H
#define IMAGEVIERWER_H

#include "ui_imageviewer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileSystemModel>
#include <QStringList>


class QString;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

namespace QTerm
{
class Canvas;
class ImageListViewer;

class Image : public QWidget
{
    Q_OBJECT
public:
    Image(const QString &, const QString &, QWidget * parent=0);
    ~Image();
    
public slots:
    void next();
    void previous();
    void browser();
    void onChange(const QString &);
signals:
    void changeImage(const QString &);
private:
    Ui::QTermImage ui;
    int d_index;
    bool d_extensionShown;
};

} // namespace QTerm

#endif
