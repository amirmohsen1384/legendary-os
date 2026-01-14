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

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public:
    void insertTask(const QPersistentModelIndex &index);
    void removeTask(const QPersistentModelIndex &index);

private:
    QList<QPersistentModelIndex> container;
};

#endif // WAITINGTASKMODEL_H
