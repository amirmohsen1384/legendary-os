#ifndef PRIORITYTASKMODEL_H
#define PRIORITYTASKMODEL_H

#include <QAbstractItemModel>
#include <QPersistentModelIndex>

class PriorityTaskModel : public QAbstractTableModel
{
    Q_OBJECT
protected:
    virtual bool betterThan(const QModelIndex &one, const QModelIndex &two) const = 0;

protected:
    void upheap(qint64 node);

    qint64 left(qint64 node);

    qint64 right(qint64 node);

    bool hasLeft(qint64 node);

    void downheap(qint64 node);

    bool hasRight(qint64 node);

    qint64 ancestor(qint64 node);

    void swap(qint64 one, qint64 two);

public:
    explicit PriorityTaskModel(qsizetype maximum = 0, QObject *parent = nullptr);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public:
    virtual QModelIndex peekBest() const;

public slots:
    virtual void removeBest();
    virtual bool insertTask(const QModelIndex &index);

private:
    QList<QPersistentModelIndex> container;
};

#endif // PRIORITYTASKMODEL_H
