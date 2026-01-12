#ifndef AGENTMODEL_H
#define AGENTMODEL_H

#include <QAbstractItemModel>
#include "agent.h"

class AgentModel : public QAbstractItemModel
{
    Q_OBJECT
    void clear();
    Agent* createAgent(const AgentInfo &info, Agent *parent);
    bool loadFromJSON(const QJsonObject &data, const QModelIndex &parent = QModelIndex());

public:
    enum class Header {Name = 0, Description = 1};

    explicit AgentModel(QObject *parent = nullptr);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex index(const QString &path) const;

    virtual QModelIndex parent(const QModelIndex &index) const override;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    virtual QModelIndex insert(const AgentInfo &info, const QModelIndex &parent);
    virtual bool remove(const QModelIndex &index);

public:
    QString toString(const QModelIndex &index);
    bool loadFromJSON(const QByteArray &data, const QModelIndex &parent = QModelIndex());

private:
    std::unique_ptr<Agent> root;
};

#endif // AGENTMODEL_H
