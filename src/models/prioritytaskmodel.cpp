#include "prioritytaskmodel.h"

qint64 PriorityTaskModel::left(qint64 node)
{
    return node * 2 + 1;
}

qint64 PriorityTaskModel::right(qint64 node)
{
    return node * 2 + 2;
}

bool PriorityTaskModel::hasLeft(qint64 node)
{
    return left(node) < container.size();
}

qint64 PriorityTaskModel::ancestor(qint64 node)
{
    return (node - 1) / 2;
}

bool PriorityTaskModel::hasRight(qint64 node)
{
    return right(node) < container.size();
}

void PriorityTaskModel::swap(qint64 one, qint64 two)
{
    if (one < 0 || one >= container.size())
    {
        return;
    }
    else if (two < 0 || two >= container.size())
    {
        return;
    }
    else
    {
        std::swap(container[one], container[two]);
        emit dataChanged(index(one, 0), index(one, columnCount() - 1), roleNames().keys());
        emit dataChanged(index(two, 0), index(two, columnCount() - 1), roleNames().keys());
    }
}

void PriorityTaskModel::upheap(qint64 node)
{
    qint64 top = ancestor(node);
    while (top >= 0)
    {
        if (betterThan(container[node], container[top]))
        {
            swap(top, node);
            node = top;
            top = ancestor(node);
        }
        else
        {
            break;
        }
    }
}

void PriorityTaskModel::downheap(qint64 node)
{
    while (hasLeft(node))
    {
        qint64 larger = left(node);
        if (hasRight(node))
        {
            auto rightNode = right(node);
            if (betterThan(container[rightNode], container[larger]))
            {
                larger = rightNode;
            }
        }
        qint64 parent = node, child = larger;
        if (betterThan(container[child], container[parent]))
        {
            swap(child, parent);
            node = larger;
        }
        else
        {
            break;
        }
    }
}

bool PriorityTaskModel::insertTask(const QModelIndex &index)
{
    auto size = container.size();
    beginInsertRows(QModelIndex(), size, size);
    container.append(QPersistentModelIndex(index));
    endInsertRows();
    upheap(size);
    return true;
}

void PriorityTaskModel::removeBest()
{
    removeRow(0);
}

QModelIndex PriorityTaskModel::peekBest() const
{
    return !container.isEmpty() ? container.front() : QModelIndex();
}

int PriorityTaskModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return container.size();
}

bool PriorityTaskModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || row < 0 || row + count > container.size())
    {
        return false;
    }
    auto size = container.size();
    for (auto i = row; i < row + count; ++i)
    {
        container[i] = container[size-- - 1];
    }
    size = container.size();
    beginRemoveRows(parent, size - count, size - 1);
    container.remove(size - count, count);
    endRemoveRows();
    if (!container.isEmpty())
    {
        for (auto i = row; i < row + count && i < container.size(); ++i)
        {
            downheap(i);
        }
    }
    return true;
}
