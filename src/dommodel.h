#ifndef DOMMODEL_H
#define DOMMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QHash>

namespace QTerm
{
class DomItem
{
public:
    DomItem(QDomElement element, DomItem *parent = 0);
    ~DomItem();
    DomItem *child(int i);
	void insertChild(int i, DomItem *item);
	void removeChild(int i);
	void reparent(DomItem *parent);
    DomItem *parent();
    QDomElement element() const;
    int row();

    QString name();
    bool setName(const QString& name);

	QString uuid();
	bool setUuid(const QString& uuid);

    bool isFolder();
    bool isSite();
    bool isSiteReference();

private:
    QDomElement domElement;
    QList<DomItem*> childItems;
    DomItem *parentItem;
};

class DomModel : public QAbstractItemModel
{
    Q_OBJECT
public:
	enum ItemType {Unknown, Folder, Site, Favorite};

    DomModel(QDomDocument document, QObject *parent = 0);
    ~DomModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;    
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

	bool removeRows(int row, int count, const QModelIndex &parent);
    bool insertRow(int row, const QModelIndex &parent, DomItem *item);
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;

	ItemType type(const QModelIndex & index) const;

	void addSite(const QModelIndex &position);
	void addFolder(const QModelIndex &position);
	void toggleFavorite(const QModelIndex &position);
	void removeItem(const QModelIndex &index);

	QDomDocument document() { return domDocument; }

private:
    QDomDocument domDocument;
    DomItem *rootItem;
};
} // namespace QTerm
#endif //DOMMODEL_H
