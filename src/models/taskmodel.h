#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractItemModel>
#include "task.h"

class TaskModel : public QAbstractItemModel
{
    Q_OBJECT
protected:
    enum class Header {Name = 0, State = 1, Agent = 3, Priority = 2};
    Task *createTask(const TaskInfo &info, Task *parent);
    void clear();

public:
    explicit TaskModel(QObject *parent = nullptr);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &index) const override;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    
    virtual QModelIndex insert(const TaskInfo &info, const QModelIndex &parent = QModelIndex());
    virtual bool remove(const QModelIndex &index);

public slots:
    void beginToProceed(const QModelIndex &index, qint64 timestamp);
    void endToProceed(const QModelIndex &index, qint64 timestamp);
    qint64 proceed(const QModelIndex &index, qint64 quantum);

private:
    std::unique_ptr<Task> root;
};

#endif // TASKMODEL_H
