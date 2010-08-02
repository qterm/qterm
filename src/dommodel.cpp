#include "dommodel.h"

#include <QtCore/QStringList>
#include <QtCore/QMimeData>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QUuid>
#include <QtGui/QMessageBox>
#include <QtGui/QIcon>

namespace QTerm
{

DomItem::DomItem(QDomElement element, DomItem *parent)
{
    domElement = element;
	parentItem = parent;
	// Populate child items
	QDomNodeList nodeList = domElement.childNodes();
	for (int i=0; i<nodeList.count(); i++) {
        QDomElement childElement = domElement.childNodes().item(i).toElement();
        DomItem *childItem = new DomItem(childElement, this);
        childItems << childItem;
    }
}

DomItem::~DomItem()
{
	foreach(DomItem *item, childItems)
		delete item;
}

QString DomItem::name()
{
    QString name;
    if (isFolder()) {
        name = domElement.attribute("name");
    } else if (isSiteReference()) {
        // Compare uuid with all sites and get site name
        QString uuid = domElement.attribute("uuid");
        QDomNodeList siteList = domElement.ownerDocument().elementsByTagName("site");
        for (int i=0; i<siteList.count(); i++) {
            QDomElement element = siteList.item(i).toElement();
            if (element.attribute("uuid") == uuid)
                name = element.attribute("name");
        }
    }
    return name;
}

bool DomItem::setName(const QString& name)
{
    if (!domElement.hasAttribute("name"))
        return false;
    domElement.setAttribute("name", name);
    return true;
}

QString DomItem::uuid()
{
	return domElement.attribute("uuid");
}

bool DomItem::setUuid(const QString & uuid)
{
    if (!domElement.hasAttribute("uuid"))
        return false;
	domElement.setAttribute("uuid", uuid);
	return true;
}

bool DomItem::isSite()
{
    return domElement.nodeName() == "site";
}

bool DomItem::isSiteReference()
{
    return domElement.nodeName() == "addsite";
}

bool DomItem::isFolder()
{
    return domElement.nodeName() == "folder";
}

QDomElement DomItem::element() const
{
    return domElement;
}

DomItem *DomItem::parent()
{
    return parentItem;
}

void DomItem::reparent(DomItem *parent)
{
	parentItem = parent;
}

DomItem *DomItem::child(int i)
{
    if (i>=0 && i<childItems.count())
        return childItems[i];
    return 0;
}

void DomItem::insertChild(int i, DomItem* item)
{
    if (i<0 || i>childItems.count())
        return;

	QDomNode node = domElement.childNodes().item(i);
	if (node.isNull())
		domElement.appendChild(item->element());
	else
		domElement.insertBefore(item->element(),node);
	item->reparent(this);
	childItems.insert(i, item);
}

void DomItem::removeChild(int i)
{
	if (i<0 || i>=childItems.count())
		return;

	QDomNode node = domElement.childNodes().item(i);
	domElement.removeChild(node);

	delete childItems.takeAt(i);
}

int DomItem::row()
{
	if (parentItem)
		return parentItem->childItems.indexOf(this);

	return 0;
}

DomModel::DomModel(QDomDocument document, QObject *parent)
    : QAbstractItemModel(parent), domDocument(document)
{
    rootItem = new DomItem(document.documentElement());
}

DomModel::~DomModel()
{
    delete rootItem;
}

int DomModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QVariant DomModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    DomItem *item = static_cast<DomItem*>(index.internalPointer());

    switch (role) {
		case Qt::UserRole:
			if (item->isFolder()) {
				return item->name();
			} else {
				return item->uuid();
			}
		break;
        case Qt::DisplayRole:
            return item->name();
        break;
        case Qt::DecorationRole:
			switch (type(index)) {
			case Folder:
                    return QVariant(QIcon(":/pic/folder.png"));
			case Favorite:
                    return QVariant(QIcon(":/pic/folder_favorite.png"));
			case Site:
					return QVariant(QIcon(":/pic/tabpad.png"));
            }
        default:
            return QVariant();
    }
}
Qt::ItemFlags DomModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
	ItemType itemType = type(index);

	defaultFlags |= Qt::ItemIsDragEnabled;
    if (itemType == Folder)
		return Qt::ItemIsDropEnabled | Qt::ItemIsEditable | defaultFlags;
    else if (itemType == Site)
        return defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QVariant DomModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return tr("Name");

    return QVariant();
}

QModelIndex DomModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    DomItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex DomModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

DomModel::ItemType DomModel::type(const QModelIndex &index) const
{
	if (!index.isValid())
		return Unknown;

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
	if (item->isFolder())
		return Folder;
	else if (item->isSiteReference()) {
		QString uuid = data(index, Qt::UserRole).toString();
		// get favor attributes
		QDomNodeList siteList = domDocument.elementsByTagName("site");
		for (int i=0; i<siteList.count(); i++) {
			QDomElement element = siteList.item(i).toElement();
			if (element.attribute("uuid") == uuid)
				if (element.attribute("favor") == "1")
					return Favorite;
				else
					return Site;
		}
	}
	return Unknown;
}

int DomModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DomItem *parentItem;
    int sitesCount = 0; // This is the number of site elements of root item
    if (!parent.isValid()) {
        parentItem = rootItem;
        sitesCount = domDocument.elementsByTagName("site").count();
    }
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    return parentItem->element().childNodes().count() - sitesCount;
}

bool DomModel::removeRows(int row, int count, const QModelIndex &parent)
{
	DomItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    if (row < 0 || row >= rowCount(parent))
        return false;
	
	beginRemoveRows(parent, row, row+count-1);
	for (int n=0; n<count; n++)
		parentItem->removeChild(row);
	endRemoveRows();

	return true;
}

bool DomModel::insertRow(int row, const QModelIndex &parent, DomItem *item)
{
    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    if (row < 0 || row > rowCount(parent))
        row = rowCount(parent);

    beginInsertRows(parent, row, row);
    parentItem->insertChild(row, item);
    endInsertRows();

    return true;
}

bool DomModel::setData(const QModelIndex &index,
                        const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    DomItem *item = static_cast<DomItem*>(index.internalPointer());

    switch (role) {
        case Qt::EditRole:
            item->setName(value.toString());
            break;
		case Qt::UserRole:
			if (item->isFolder())
				item->setName(value.toString());
			else if (item->isSiteReference())
				item->setUuid(value.toString());
			break;
        default:
            return false;
    }

    emit dataChanged(index, index);
    return true;
}

bool DomModel::dropMimeData(const QMimeData *data,
    Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("text/plain"))
        return false;

    // Find out after which row to insert
    int beginRow;
    if (row != -1) // given
        beginRow = row;
    else if (parent.isValid()) // 1st of parent item
        beginRow = 0;
    else // last of the root item
        beginRow = rowCount(QModelIndex());

	DomItem *parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    // Extract dropped items
    QByteArray encodedData = data->data("text/plain");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<DomItem*> newItems;
    while (!stream.atEnd()) {
        quintptr p;
        stream >> p;
        newItems << static_cast<DomItem*>((void *)p);
    }
	int count = newItems.count();

    switch(action) {
    case Qt::MoveAction:
		for (int i=0; i<count; i++) {    
			DomItem *item = newItems.at(i);
			QDomElement node = item->element().cloneNode().toElement();
			DomItem *newItem = new DomItem(node);
			insertRow(beginRow, parent, newItem);
		}
        break;
    default:
        return false;
    }
    return true;
}

QMimeData *DomModel::mimeData(const QModelIndexList &indexes) const
{
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (QModelIndex index, indexes) {
        if (index.isValid())
            stream << quintptr(index.internalPointer());
    }
    QMimeData *mimeData = new QMimeData();
    mimeData->setData("text/plain", encodedData);
    return mimeData;
}

QStringList DomModel::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    return types;
}

Qt::DropActions DomModel::supportedDropActions() const 
{
    return Qt::MoveAction;
}

void DomModel::addFolder(const QModelIndex &index)
{
	// New folder is either subfolder or sibyling
	QModelIndex parentIndex;
	int row;
	if (type(index) == Folder) {
		parentIndex = index;
		row = -1;
	} else {
		parentIndex = index.parent();
		row = index.row();
	}
	// Create folder element
	QDomElement folder = domDocument.createElement("folder");
	folder.setAttribute("name", "New Folder");
	// Create and insert
	DomItem *item = new DomItem(folder);
	insertRow(row, parentIndex, item);
}

void DomModel::toggleFavorite(const QModelIndex &index)
{
	QString uuid = data(index, Qt::UserRole).toString();
	if (QUuid(uuid).isNull())
		return;
	// toggle favor attributes
	QDomNodeList siteList = domDocument.elementsByTagName("site");
	for (int i=0; i<siteList.count(); i++) {
		QDomElement element = siteList.item(i).toElement();
		if (element.attribute("uuid") == uuid)
			if (element.attribute("favor") == "1")
				element.setAttribute("favor", 0);
			else
				element.setAttribute("favor", 1);
	}
}

void DomModel::addSite(const QModelIndex &index)
{
	QString newUuid = QUuid::createUuid().toString();
	// Clone current site or default site
	QString uuid;
        if (type(index) == Site || type(index) == Favorite)
		uuid = data(index, Qt::UserRole).toString();
	else
		uuid = QUuid().toString();
	QDomNodeList siteList = domDocument.elementsByTagName("site");
    for (int i=0; i<siteList.count(); i++) {
		QDomElement site = siteList.item(i).toElement();
        if (site.attribute("uuid") == uuid) {
			QDomElement newSite = site.cloneNode().toElement();
			newSite.setAttribute("uuid", newUuid);
			QDomElement root = domDocument.documentElement();
			root.appendChild(newSite);
			break;
		}
	}
	// Create site reference
	QDomElement newSiteRef = domDocument.createElement("addsite");
	newSiteRef.setAttribute("uuid", newUuid);
	// New site reference is either child or sibyling
	QModelIndex parentIndex;
	int row;
	if (type(index) == Folder) {
		parentIndex = index;
		row = -1;
	} else {
		parentIndex = index.parent();
		row = index.row();
	}
	DomItem *item = new DomItem(newSiteRef);
	insertRow(row, parentIndex, item);
}

void DomModel::removeItem(const QModelIndex &index)
{
	if (type(index) == Folder) {
		// recursively remove all children
		for (int n=0; n<rowCount(index); n++)
			removeItem(index.child(n,0));
		// remove folder itself, should be empty now
		removeRows(index.row(), 1, index.parent());
	}
	else {
		QString uuid = data(index, Qt::UserRole).toString();
		QDomNodeList nodeList;
		// remove the actual site
                nodeList = domDocument.documentElement().elementsByTagName("site");
		for (int i=0; i<nodeList.count(); i++) {
			QDomElement node = nodeList.at(i).toElement();
			if (uuid == node.attribute("uuid")) {
                                domDocument.documentElement().removeChild(node);
			}
		}
		// and its reference in folder
		removeRows(index.row(), 1, index.parent());
	}
}

} //namespace QTerm

#include <dommodel.moc>
