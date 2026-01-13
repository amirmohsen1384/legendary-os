#ifndef WAITINGTASKMODEL_H
#define WAITINGTASKMODEL_H

#include <QAbstractTableModel>
#include <QPersistentModelIndex>

class WaitingTaskModel : public QAbstractTableModel
{
    Q_OBJECT
    enum class Header {PID, Name, Reason};

public:
    explicit WaitingTaskModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void insert(const QPersistentModelIndex &index);
    void remove(const QPersistentModelIndex &index);

private:
    QList<QPersistentModelIndex> container;
};

#endif // WAITINGTASKMODEL_H
