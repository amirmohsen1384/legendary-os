#ifndef READYMODEL_H
#define READYMODEL_H

#include "models/prioritymodel.h"

class ReadyModel : public PriorityModel
{
    Q_OBJECT
    enum class Header {PID, Name, Priority, Progress};

public:
    explicit ReadyModel(qsizetype maximum = 0, QObject *parent = nullptr);
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public:
    bool hasCapacity() const;
    qsizetype maximumSize() const;

public slots:
    virtual bool insertTask(const QPersistentModelIndex &index) override;
    void setMaximumSize(qsizetype size);

private:
    qsizetype maximum;
};

#endif // READYMODEL_H
