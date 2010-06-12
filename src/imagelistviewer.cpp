#include "imagelistviewer.h"
#include "qtermglobal.h"

#include <QFile>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <QFileInfo>

namespace QTerm
{

ImageFileModel::ImageFileModel(const QString& path, QObject *parent)
        : QFileSystemModel(parent) {
    setFilter(QDir::Files);
    setNameFilters(QStringList() << "*.jpg" << "*.jpeg" << "*.gif" 
                                 << "*.mng" << "*.png"  << "*.bmp");
    setRootPath(path);
}

int ImageFileModel::columnCount(const QModelIndex &parent) const {
    return 1;
}

QVariant ImageFileModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    switch (role) {
        case Qt::ToolTipRole: return QString(tr("Name: %1\nType: %2\nSize: %3 KB"))
                                        .arg(fileName(index))
                                        .arg(type(index))
                                        .arg(size(index)/1024);
        case Qt::DisplayRole: return filePath(index);
        default: return QVariant();
    }
}

ImageDelegate::ImageDelegate(QObject *parent)
        : QStyledItemDelegate(parent) {
    d_shadow = Global::instance()->pathPic() + "pic/shadow.png";
}

QSize ImageDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const {
    QString fullFilename = qvariant_cast<QString>(index.model()->data(index));
    QFileInfo fi(fullFilename);
    QString thumbFilename = genThumb(fi.absolutePath(), fi.fileName());
    QImage image(thumbFilename);
    return image.size();
}

QString ImageDelegate::genThumb(const QString & path, const QString & filename) const {
    // check thumbview file existence
    const QString file = QString( "shadow_cache_%1_%2.png" ).arg(filename).arg( thumbSize );
    const QString folder = path+"/shadow-cache/";
    QDir dir;
    if (dir.exists(folder+file))
        return folder + file;
    // scale original and shadow images
    QImage original(path+"/"+filename);
    original = original.scaled(thumbSize, thumbSize, Qt::KeepAspectRatio);
    QImage shadow(d_shadow);
    uint shadowSize;
    if (original.width() > original.height())
        shadowSize = static_cast<uint>(original.width()/100.0*6.0);
    else
        shadowSize = static_cast<uint>(original.height()/100.0*6.0);

    shadow = shadow.scaled( original.width() + shadowSize, original.height() + shadowSize);
    // create thumbview file
    QImage target(shadow);
    QPainter painter(&target);
    painter.drawImage(0, 0, original);
    target.save( folder + file, "PNG" );

    return folder + file;
}


void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // get the thumbview image
    QString fullFilename = qvariant_cast<QString>(index.model()->data(index));
    QFileInfo fi(fullFilename);
    QString thumbFilename = genThumb(fi.absolutePath(), fi.fileName());
    QImage image(thumbFilename);
    // prepare
    painter->save();
    QRect displayRect(option.rect);
    displayRect.translate((displayRect.width()-image.width())/2, 0);
    // draw selection highlight
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    // draw image
    painter->drawImage(displayRect.topLeft(), image);
    // finish
    painter->restore();
}

const QString ImageListViewer::thumbDir("shadow-cache/");

ImageListViewer::ImageListViewer(QWidget * parent)
    :QListView(parent)
{
    QString path = Global::instance()->m_pref.strPoolPath;
    d_model = new ImageFileModel(path);
    setModel(d_model);
    setRootIndex(d_model->index(path));

    d_delegate = new ImageDelegate();
    setItemDelegate(d_delegate);

    //connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(viewImage(const QModelIndex&)));
}

ImageListViewer::~ImageListViewer()
{
    delete d_model;
    delete d_delegate;
}

void ImageListViewer::imageChanged(const QString &filename)
{
    setCurrentIndex(d_model->index(filename));
}

void ImageListViewer::viewImage(const QModelIndex &index)
{
    QString filename = qvariant_cast<QString>(index.model()->data(index));
    emit selectedFileChanged(filename);
}

void ImageListViewer::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    viewImage(current);
    QListView::currentChanged(current, previous);
}

} // namespace QTerm

#include <imagelistviewer.moc>
