#ifndef IMAGELISTVIEWER_H
#define IMAGELISTVIEWER_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileSystemModel>
#include <QStringList>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QListView>


class QString;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

namespace QTerm
{
class ImageFileModel  : public QFileSystemModel
{
    Q_OBJECT
public:
    ImageFileModel(const QString& path, QObject *parent=0);
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

class ImageDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ImageDelegate(QObject *parent=0);
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QString genThumb(const QString & path, const QString & filename) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    static const int thumbSize = 100;
    QString d_shadow;
};

class ImageListViewer : public QListView
{
    Q_OBJECT
public:
    ImageListViewer(QWidget * parent=0);
    ~ImageListViewer();
    
    QSize sizeHint() const {
      return QSize(150,150);
    }
    static void genThumb(const QString &, const QString &, const QString &);

public slots:
    void viewImage(const QModelIndex&);
    void imageChanged(const QString &);
protected slots:
    virtual void currentChanged(const QModelIndex & current, const QModelIndex & previous);

signals:
    void selectedFileChanged(const QString &);
private:
    static const int thumbSize = 100;
    static const QString thumbDir;
    ImageFileModel *d_model;
    ImageDelegate  *d_delegate;
};

} // namespace QTerm

#endif
