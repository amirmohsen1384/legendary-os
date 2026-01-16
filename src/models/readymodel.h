#ifndef READYMODEL_H
#define READYMODEL_H

#include <QAbstractItemModel>
#include <QPersistentModelIndex>

class ReadyModel : public QAbstractTableModel
{
    Q_OBJECT
    enum class Header {PID, Name, Priority, Progress};

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
    explicit ReadyModel(qsizetype maximum = 0, QObject *parent = nullptr);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public:
    bool hasCapacity() const;
    qsizetype getMaximumSize() const;
    QModelIndex getMostCritical() const;

public slots:
    bool insertTask(const QModelIndex &index);
    void removeTask(const QModelIndex &index);
    void setMaximumSize(qsizetype size);
    void removeMostCritical();

private:
    qsizetype maximum;
    QList<QPersistentModelIndex> container;
};

#endif // READYMODEL_H
