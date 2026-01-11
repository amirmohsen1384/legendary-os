#ifndef TASK_H
#define TASK_H

#include <QString>

class TaskInfo
{
public:
    enum Role
    {
        Name = Qt::UserRole,
        Agent = Qt::UserRole + 3,
        State = Qt::UserRole + 4,
        Priority = Qt::UserRole + 1,
        BurstTime = Qt::UserRole + 2,
    };
    TaskInfo() {}

    QString getName() const;

    void setName(const QString &value);

    qint64 getPriority() const;

    void setPriority(qint64 value);

    qint64 getBurstTime() const;

    void setBurstTime(qint64 value);

    bool depends() const;

    QString getAgent() const;

    void setAgent(const QString &value);

private:
    QString name;
    QString agent;
    qint64 priority;
    qint64 burstTime;
};

class Task : public TaskInfo
{
public:
    enum class State
    {
        Unknown,
        Running,
        Ready,
        WaitingForLimit,
        WaitingForAgent
    };

public:
    Q_DISABLE_COPY_MOVE(Task)
    Task(Task *parent = nullptr);
    ~Task();

    State getState() const;

    void setState(State value);

    Task* getParent();

    void setParent(Task *value);

    void addChild(std::unique_ptr<Task> item);

    void removeChild(int row);

    qint64 childCount() const;

    Task* getChild(int row);

    qint64 row() const;

    qint64 columnCount() const;

    qint64 getStartTime() const;

    void setStartTime(qint64 value);

    qint64 getFinishTime() const;

    void setFinishTime(qint64 value);

    qint64 getIdentifier() const;

    qint64 getRemainingTime() const;

    void setRemainingTime(qint64 value);

    Task* find(qint64 value);

    static qint64 getMinimumID();

    static qint64 getMaximumID();

private:
    qint64 identifier;
    qint64 startTime = 0;
    qint64 finishTime = 0;
    qint64 remainingTime = 0;
    Task *parent = nullptr;
    State state = State::Unknown;
    std::vector<std::unique_ptr<Task>> children;
};

#endif // TASK_H
