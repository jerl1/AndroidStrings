#include "androidstringmodel.h"

#include <QColor>

AndroidStringItem::AndroidStringItem(const AndroidString *str, AndroidStringItem *parent)
{
    mParentItem = parent;
    mString = str;
}

AndroidStringItem::~AndroidStringItem()
{
    qDeleteAll(mChildItems);
}

void AndroidStringItem::appendChild(AndroidStringItem *item)
{
    mChildItems.append(item);
}

AndroidStringItem *AndroidStringItem::child(int row)
{
    return mChildItems.value(row);
}

int AndroidStringItem::childCount() const
{
    return mChildItems.count();
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
            if ((mParentItem == NULL) || (mParentItem->mString->path() != mString->path()))
                return mString->path();
            else
                return QString("---");
        case ColumnAndroidLabel:
            if ((mParentItem == NULL) ||
                (mParentItem->mString->androidLabel() != mString->androidLabel()))
                return mString->androidLabel();
            else
                return QString("---");
        case ColumnLanguage:
            return mString->language();
        case ColumnTranslation:
            return mString->formatedTranslation();
        case ColumnStatus:
            return mString->status();
    }
    return QVariant();
}

AndroidStringItem *AndroidStringItem::parent()
{
    return mParentItem;
}

int AndroidStringItem::row() const
{
    if (mParentItem)
        return mParentItem->mChildItems.indexOf(const_cast<AndroidStringItem*>(this));

    return 0;
}

//--------------------------------------------------------------

AndroidStringModel::AndroidStringModel(QList<AndroidString*> &list, QObject *parent)
    : QAbstractItemModel(parent)
{
    mRootString = new AndroidString();
    mRootString->setPath("Path");
    mRootString->setAndroidLabel("Label");
    mRootString->setLanguage("Language");
    mRootString->appendTranslation("Text");
    mRootItem = new AndroidStringItem(mRootString);

    setupModelData(list, mRootItem);
}

AndroidStringModel::~AndroidStringModel()
{
    delete mRootString;
    delete mRootItem;
}

int AndroidStringModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<AndroidStringItem*>(parent.internalPointer())->columnCount();
    else
        return mRootItem->columnCount();
}

QVariant AndroidStringModel::data(const QModelIndex &index, int role) const
{
    AndroidStringItem *item = static_cast<AndroidStringItem*>(index.internalPointer());
    if (!index.isValid())
        return QVariant();

    if (role == Qt::ForegroundRole) {
        switch (item->data(AndroidStringItem::ColumnStatus).toInt())
        {
            case AndroidString::TypeOverided:
                return QVariant(QColor(Qt::blue));
            case AndroidString::TypeNew:
                return QVariant(QColor(Qt::black));
            case AndroidString::TypeOverlayNew:
                return QVariant(QColor(Qt::red));
        }
    } else if (role == Qt::DisplayRole) {
        return item->data(index.column());
    }

    return QVariant();
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
        return mRootItem->data(section);

    return QVariant();
}

QModelIndex AndroidStringModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    AndroidStringItem *parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
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

    if (parentItem == mRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int AndroidStringModel::rowCount(const QModelIndex &parent) const
{
    AndroidStringItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = mRootItem;
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
            parentLabelItem = parentPathItem;
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
