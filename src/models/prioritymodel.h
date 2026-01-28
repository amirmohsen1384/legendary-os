#ifndef PRIORITYMODEL_H
#define PRIORITYMODEL_H

#include <QAbstractItemModel>
#include <QPersistentModelIndex>

class PriorityModel : public QAbstractTableModel
{
    Q_OBJECT
    enum Header {PID, Name, Agent, Priority};

protected:
    virtual bool betterThan(const QPersistentModelIndex &one, const QPersistentModelIndex &two) const;

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
    explicit PriorityModel(QObject *parent = nullptr);
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public:
    virtual QPersistentModelIndex peekBest() const;

public slots:
    virtual void clear();
    virtual void removeBest();
    virtual bool insertTask(const QPersistentModelIndex &index);

public:
    QPersistentModelIndex toTask(const QPersistentModelIndex &index) const;

protected:
    QList<QPersistentModelIndex> container;
};

#endif // PRIORITYMODEL_H
