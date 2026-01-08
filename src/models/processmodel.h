#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractItemModel>
#include "processinfo.h"

class ProcessModel : public QAbstractItemModel
{
    Q_OBJECT
    enum class Info {
        Name = 0,
        State = 1,
        Dependency = 2,
        Priority = 3,
        PID = 4
    };

public:
    explicit ProcessModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::unique_ptr<ProcessInfo> root;
};

#endif // PROCESSMODEL_H
