#ifndef TASK_H
#define TASK_H

#include <QString>

class TaskInfo
{
public:
    enum Role
    {
        NameRole = Qt::UserRole,
        PIDRole = Qt::UserRole + 5,
        AgentRole = Qt::UserRole + 3,
        StateRole = Qt::UserRole + 4,
        QuantumRole = Qt::UserRole + 9,
        PriorityRole = Qt::UserRole + 1,
        BurstTimeRole = Qt::UserRole + 2,
        StartTimeRole = Qt::UserRole + 6,
        IdleTimeRole = Qt::UserRole + 11,
        FinishTimeRole = Qt::UserRole + 7,
        ExecutableRole = Qt::UserRole + 10,
        RemainingTimeRole = Qt::UserRole + 8
    };
    TaskInfo() {}

public:
    bool isValid() const;
    bool depends() const;
    QString getName() const;
    QString getAgent() const;
    qint64 getPriority() const;
    qint64 getBurstTime() const;

public:
    virtual void setAgent(const QString &value);
    virtual void setName(const QString &value);
    virtual void setBurstTime(qint64 value);
    virtual void setPriority(qint64 value);

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
        WaitingForLimit,
        WaitingForAgent,
        Terminate,
        Execute,
        Running,
        Unknown,
        Timeout,
        Error,
        Ready
    };

public:
    Task(Task *parent = nullptr);
    Q_DISABLE_COPY_MOVE(Task)
    ~Task();

    qint64 getRemainingTime() const;
    qint64 getFinishTime() const;
    qint64 getStartTime() const;
    qint64 getIdleTime() const;
    qint64 getQuantum() const;

    qint64 getIdentifier() const;

    qint64 columnCount() const;
    qint64 childCount() const;
    bool finished() const;

    Task* getChild(int row);
    State getState() const;

    qint64 row() const;
    Task* getParent();

    static QString text(const Task::State &state);

public:
    bool removeChild(int row);
    void setState(State value);
    void setParent(Task *value);
    void addChild(std::unique_ptr<Task> item);

public:
    void setIdleTime(qint64 value);
    virtual void setBurstTime(qint64 value) override;

public:
    bool beginToProceed(qint64 timestamp);
    bool endToProceed(qint64 timestamp);
    qint64 proceed(qint64 value);

private:
    qint64 identifier;
    qint64 quantum = 0;
    qint64 idleTime = 0;
    qint64 startTime = -1;
    qint64 finishTime = -1;
    Task *parent = nullptr;
    State state = State::Unknown;
    std::vector<std::unique_ptr<Task>> children;
};

#endif // TASK_H
