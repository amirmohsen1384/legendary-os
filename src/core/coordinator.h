#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "models/prioritymodel.h"
#include "models/loggingmodel.h"
#include "models/readymodel.h"
#include "models/agentmodel.h"
#include "models/taskmodel.h"
#include "core/system.h"

#include <QWaitCondition>
#include <QThread>
#include <QMutex>
#include <QQueue>

class Coordinator : public QThread
{
    Q_OBJECT
    Q_PROPERTY(qint64 unusedQuantums READ getUnusedQuantums NOTIFY quantumUnused)
    Q_PROPERTY(qint64 elapsedQuantums READ getElapsedQuantums NOTIFY quantumElapsed)

private slots:
    void cancel();

private:
    void releaseLock();
    void recordLock();
    bool canContinue();
    void dispatch(const QModelIndex &task);
    qint64 evaluatePriority(const QModelIndex &task);
    bool logTask(const QModelIndex &task, const Task::State &previous, const Task::State &current, const QString &description = QString());

protected:
    bool hasReqiurements() const;

protected:
    virtual void run() override;

public:
    Coordinator(const Settings::Info &info, QObject *parent = nullptr);
    ~Coordinator();

    qint64 getElapsedQuantums();
    qint64 getUnusedQuantums();
    qreal getUtilizationRate();
    bool isPaused();
    bool isLocked();

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

    void lock();
    void unlock();
    void setLocked(bool state);
    void setPaused(bool state);

    void setTasks(TaskModel *model);
    void setAgents(AgentModel *model);
    void setLogs(LoggingModel *model);
    void setReadyTasks(ReadyModel *model);
    void setLimitTasks(PriorityModel *model);
    void setAgentTasks(PriorityModel *model);

signals:
    void quantumElapsed(qint64 quantum);
    void quantumUnused(qint64 quantum);
    void lockStateChanged(bool state);
    void pausedChanged(bool state);
    void shutdownScheduled();

private:
    QMutex mutex;
    bool pause = false;
    bool abort = false;
    bool locked = false;
    Settings::Info settings;
    QWaitCondition canProcess;
    qint64 unusedQuantums = 0;
    qint64 enteredCommands = 0;
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
