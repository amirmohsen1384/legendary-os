#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractItemModel>
#include "process.h"

class ProcessModel : public QAbstractItemModel
{
    Q_OBJECT
protected:
    enum class Info {
        Name = 1,
        State = 2,
        Dependency = 4,
        Priority = 3,
        PID = 0
    };

    Process *createProcess(const ProcessInfo &info, Process *parent);

public:
    explicit ProcessModel(QObject *parent = nullptr);

    virtual QModelIndex parent(const QModelIndex &index) const override;
    virtual QModelIndex index(qint64 value, const QModelIndex &parent = QModelIndex());
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    virtual bool insert(const ProcessInfo &info, const QModelIndex &parent = QModelIndex());
    virtual bool remove(const QModelIndex &index);

public slots:
    void clear();

private:
    std::unique_ptr<Process> root;
};

#endif // PROCESSMODEL_H
