#include "imageviewer.h"
#include "qtermcanvas.h"

#include <QFile>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <QFileInfo>

namespace QTerm
{
Image::Image(const QString & path, QWidget * parent)
    :QWidget(parent), d_index(0)
{
    ui.setupUi(this);

    d_extensionShown = true;

    /*
    ui.d_browser->setText("Hide Browser");
    connect(ui.d_next, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui.d_previous, SIGNAL(clicked()), this, SLOT(previous()));
    connect(ui.d_browser, SIGNAL(clicked()), this, SLOT(browser()));
   */
    connect(ui.d_list, SIGNAL(selectedFileChanged(const QString&)), this, SLOT(onChange(const QString&)));
    connect(this, SIGNAL(changeImage(const QString&)), ui.d_list, SLOT(imageChanged(const QString&)));
}

Image::~Image()
{
}

void Image::next()
{
    /*
    if (d_list->isEmpty())
        return;
    if (d_index < d_list->count() - 1) {
        ++d_index;
        emit changeImage(d_list->at(d_index));
    }
    */
}

void Image::previous()
{
    /*
    if (d_list->isEmpty())
        return;
    if (d_index <= 0)
        return;
    --d_index;
    emit changeImage(d_list->at(d_index));
    */
}

void Image::browser()
{
    /*
    if (d_list->isEmpty())
        return;
    */
    d_extensionShown = !d_extensionShown;
    ui.d_list->setVisible(d_extensionShown);
    /*
    if (d_extensionShown)
        ui.d_browser->setText(tr("Hide &Browser"));
    else
        ui.d_browser->setText(tr("Show &Browser"));
    */
}

void Image::onChange(const QString & filename)
{
    /*
    int position = d_list->indexOf(filename);
    if (position != -1)
        d_index = position;

    ui.d_previous->setEnabled(true);
    ui.d_next->setEnabled(true);

    if (d_index == 0)
        ui.d_previous->setEnabled(false);
    if (d_index == d_list->count() - 1)
        ui.d_next->setEnabled(false);
    
    ui.d_canvas->loadImage(d_path+filename);
    */
    ui.d_canvas->loadImage(filename);
}

} // namespace QTerm

#include <moc_imageviewer.cpp>
