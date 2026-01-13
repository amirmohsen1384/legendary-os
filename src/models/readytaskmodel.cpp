#include "readytaskmodel.h"

ReadyTaskModel::ReadyTaskModel(QObject *parent)
    : QAbstractItemModel(parent)
{}

QVariant ReadyTaskModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex ReadyTaskModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex ReadyTaskModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int ReadyTaskModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int ReadyTaskModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant ReadyTaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
