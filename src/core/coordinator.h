#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "models/prioritymodel.h"
#include "models/loggingmodel.h"
#include "models/readymodel.h"
#include "models/agentmodel.h"
#include "models/taskmodel.h"
#include "core/system.h"
#include <QThread>
#include <QMutex>

class Coordinator : public QThread
{
    Q_OBJECT
    Q_PROPERTY(qint64 unusedQuantums READ getUnusedQuantums NOTIFY quantumUnused)
    Q_PROPERTY(qint64 elapsedQuantums READ getElapsedQuantums NOTIFY quantumElapsed)

private:
    void dispatch(const QModelIndex &task);
    bool logTask(const QModelIndex &task, const Task::State &previous, const Task::State &current, const QString &description = QString());

protected:
    bool hasReqiurements() const;
    void setElapsedQuantums(qint64 value);

protected:
    virtual void run() override;

public:
    Coordinator(const Config::Info &info, QObject *parent = nullptr);
    qint64 getElapsedQuantums();
    qint64 getUnusedQuantums();
    qreal getUtilizationRate();

    TaskModel* getTasks();
    AgentModel* getAgents();
    LoggingModel* getLogs();
    ReadyModel* getReadyTasks();
    PriorityModel* getLimitTasks();
    PriorityModel* getAgentTasks();

public slots:
    bool insertAgent(const AgentInfo &info, const QModelIndex &parent);
    bool insertTask(const TaskInfo &info, const QModelIndex &parent);
    bool removeAgent(const QModelIndex &agent);
    bool removeTask(const QModelIndex &task);
    void scheduleShutdown();

    void setTasks(TaskModel *model);
    void setAgents(AgentModel *model);
    void setLogs(LoggingModel *model);
    void setReadyTasks(ReadyModel *model);
    void setLimitTasks(PriorityModel *model);
    void setAgentTasks(PriorityModel *model);

signals:
    void quantumElapsed(qint64 quantum);
    void quantumUnused(qint64 quantum);
    void shutdownScheduled();

private:
    QMutex mutex;
    Config::Info settings;
    qint64 unusedQuantums = 0;
    qint64 elapsedQuantums = 0;
    TaskModel* tasks = nullptr;
    AgentModel* agents = nullptr;
    LoggingModel* logs = nullptr;
    bool shudownSchedule = false;
    ReadyModel* readyTasks = nullptr;
    PriorityModel* limitTasks = nullptr;
    PriorityModel* agentTasks = nullptr;
};

#endif // COORDINATOR_H
