#ifndef ANDROIDSTRINGMODEL_H
#define ANDROIDSTRINGMODEL_H

#include <QAbstractItemModel>

#include "androidstring.h"

class AndroidStringItem
{
    public:
        AndroidStringItem(const AndroidString *str, AndroidStringItem *parent = NULL);
        ~AndroidStringItem();

        void appendChild(AndroidStringItem *child);

        int childCount() const;
        int columnCount() const;
        int row() const;
        QVariant data(int column) const;

        AndroidStringItem *parent();
        AndroidStringItem *child(int row);

        typedef enum _columnDefinition
        {
            ColumnPath = 0,
            ColumnAndroidLabel,
            ColumnLanguage,
            ColumnTranslation,
            ColumnCount, //Last displayed column
            ColumnType,
            ColumnOverided
        } columnDefinition;

    private:
        const AndroidString *mString;
        QList<AndroidStringItem*> childItems;
        AndroidStringItem *parentItem;
};

class AndroidStringModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        AndroidStringModel(QList<AndroidString*> &list, QObject *parent = 0);
        ~AndroidStringModel();

        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

    private:
        void setupModelData(QList<AndroidString*> &list, AndroidStringItem *parent);

        AndroidStringItem *rootItem;
};

#endif // ANDROIDSTRINGMODEL_H
