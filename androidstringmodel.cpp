#include "androidstringmodel.h"

AndroidStringItem::AndroidStringItem(const AndroidString *str, AndroidStringItem *parent)
{
    parentItem = parent;
    mString = str;
}

AndroidStringItem::~AndroidStringItem()
{
    qDeleteAll(childItems);
}

void AndroidStringItem::appendChild(AndroidStringItem *item)
{
    childItems.append(item);
}

AndroidStringItem *AndroidStringItem::child(int row)
{
    return childItems.value(row);
}

int AndroidStringItem::childCount() const
{
    return childItems.count();
}

int AndroidStringItem::columnCount() const
{
    return ColumnCount;
}

QVariant AndroidStringItem::data(int column) const
{
    switch (column) {
        case ColumnPath:
            //Check if we should display the path
            if ((parentItem == NULL) || (parentItem->mString->path() != mString->path()))
                return mString->path();
            else
                return QString("---");
        case ColumnAndroidLabel:
        if ((parentItem == NULL) || (parentItem->mString->androidLabel() != mString->androidLabel()))
            return mString->androidLabel();
        else
            return QString("---");
        case ColumnLanguage:
            return mString->language();
        case ColumnTranslation:
            return mString->formatedTranslation();
    }
    return QVariant();
}

AndroidStringItem *AndroidStringItem::parent()
{
    return parentItem;
}

int AndroidStringItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<AndroidStringItem*>(this));

    return 0;
}

//--------------------------------------------------------------

AndroidStringModel::AndroidStringModel(QList<AndroidString*> &list, QObject *parent)
    : QAbstractItemModel(parent)
{
    AndroidString *root = new AndroidString();
    root->setPath("Path");
    root->setAndroidLabel("Label");
    root->setLanguage("Language");
    root->appendTranslation("Text");
    rootItem = new AndroidStringItem(root);

    setupModelData(list, rootItem);
}

AndroidStringModel::~AndroidStringModel()
{
    delete rootItem;
}

int AndroidStringModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<AndroidStringItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant AndroidStringModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    AndroidStringItem *item = static_cast<AndroidStringItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags AndroidStringModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant AndroidStringModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex AndroidStringModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    AndroidStringItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<AndroidStringItem*>(parent.internalPointer());

    AndroidStringItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex AndroidStringModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    AndroidStringItem *childItem = static_cast<AndroidStringItem*>(index.internalPointer());
    AndroidStringItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int AndroidStringModel::rowCount(const QModelIndex &parent) const
{
    AndroidStringItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<AndroidStringItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void AndroidStringModel::setupModelData(QList<AndroidString*> &list, AndroidStringItem *parent)
{
    AndroidString *previousString = NULL;
    AndroidStringItem *parentPathItem = NULL;
    AndroidStringItem *parentLabelItem = NULL;

    foreach (AndroidString *androidString, list) {
        AndroidStringItem *childItem = NULL;
        if ((previousString == NULL) ||
            (previousString->path() != androidString->path())) {
            //Need to attach to the root
            childItem = new AndroidStringItem(androidString, parent);
            parent->appendChild(childItem);
            //Save the parentItem if we must add one
            parentPathItem = childItem;
        } else {
            //Path are equal
            if (previousString->androidLabel() != androidString->androidLabel()) {
                childItem = new AndroidStringItem(androidString, parentPathItem);
                parentPathItem->appendChild(childItem);
                parentLabelItem = childItem;
            } else {
                childItem = new AndroidStringItem(androidString, parentLabelItem);
                parentLabelItem->appendChild(childItem);
            }
        }
        previousString = androidString;
    }
}
