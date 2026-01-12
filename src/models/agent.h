#ifndef AGENT_H
#define AGENT_H

#include <QString>
#include <QPixmap>
#include <QDate>

class AgentInfo
{
public:
    enum Role
    {
        NameRole = Qt::UserRole,
        DescriptionRole = Qt::UserRole + 1
    };

public:
    QString getDescription() const;
    QString getName() const;

public:
    void setDescription(const QString &value);
    void setName(const QString &value);

private:
    QString description;
    QString name;
};

class Agent : public AgentInfo
{
public:
    Q_DISABLE_COPY_MOVE(Agent)
    Agent(Agent *parent = nullptr);

    Agent* getChild(int row);
    qint64 childCount() const;
    qint64 columnCount() const;

    Agent* find(const QString &name);
    qint64 getRow() const;
    Agent* getParent();

public:
    void addChild(std::unique_ptr<Agent> item);
    void setParent(Agent* value);
    void removeChild(int row);

private:
    std::vector<std::unique_ptr<Agent>> children;
    Agent* parent = nullptr;
};

#endif // AGENT_H
