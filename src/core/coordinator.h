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

// Forward declaration
class CoordinatorWorker;

class Coordinator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 unusedQuantums READ getUnusedQuantums NOTIFY quantumUnused)
    Q_PROPERTY(qint64 elapsedQuantums READ getElapsedQuantums NOTIFY quantumElapsed)

    friend class CoordinatorWorker;

private slots:
    void cancel();
    bool updateIdleTimeExceptFor(const QModelIndex &parent, const QModelIndex &target, qint64 forwarded);

private:
    void releaseLock();
    void dispatch(const QModelIndex &task);
    qint64 evaluatePriority(const QModelIndex &task);
    bool updateIdleTime(const QModelIndex &task, qint64 forwarded);
    bool logTask(const QModelIndex &task, const Task::State &previous, const Task::State &current, const QString &description = QString());

protected:
    bool hasReqiurements() const;

public:
    Coordinator(const Settings::Info &info, QObject *parent = nullptr);
    ~Coordinator();

    qint64 getElapsedQuantums();
    qint64 getUnusedQuantums();
    qreal getUtilizationRate();
    bool isPaused();
    bool isLocked();
    bool isRunning() const;

    TaskModel* getTasks();
    AgentModel* getAgents();
    LoggingModel* getLogs();
    ReadyModel* getReadyTasks();
    PriorityModel* getLimitTasks();
    PriorityModel* getAgentTasks();

public slots:
    void start();
    bool insertAgent(const AgentInfo &info, const QModelIndex &parent);
    bool insertTask(const TaskInfo &info, const QModelIndex &parent);
    bool removeAgent(const QModelIndex &agent);
    bool removeTask(const QModelIndex &task);
    void scheduleShutdown();

    void lock();
    void unlock();
    void recordLock();
    void setLocked(bool state);
    void setPaused(bool state);

    void setTasks(TaskModel *model);
    void setAgents(AgentModel *model);
    void setLogs(LoggingModel *model);
    void setReadyTasks(ReadyModel *model);
    void setLimitTasks(PriorityModel *model);
    void setAgentTasks(PriorityModel *model);

signals:
    void utilizationRateChanged(qreal value);
    void quantumElapsed(qint64 quantum);
    void quantumUnused(qint64 quantum);
    void lockStateChanged(bool state);
    void pausedChanged(bool state);
    void shutdownScheduled();
    void lockRecorded();
    void finished();

private:
    QMutex mutex;
    bool pause = false;
    bool abort = false;
    bool locked = false;
    bool running = false;
    Settings::Info settings;
    QWaitCondition canProcess;
    qint64 unusedQuantums = 0;
    qint64 enteredCommands = 0;
    qint64 elapsedQuantums = 0;
    TaskModel* tasks = nullptr;
    AgentModel* agents = nullptr;
    LoggingModel* logs = nullptr;
    bool shutdownSchedule = false;
    ReadyModel* readyTasks = nullptr;
    PriorityModel* limitTasks = nullptr;
    PriorityModel* agentTasks = nullptr;
    QThread workerThread;
    CoordinatorWorker* worker = nullptr;
};

// Worker class that runs on a separate thread
class CoordinatorWorker : public QObject
{
    Q_OBJECT

public:
    explicit CoordinatorWorker(Coordinator* coord);

public slots:
    void process();

signals:
    void finished();

private:
    bool canContinue();
    Coordinator* coord;
};

#endif // COORDINATOR_H
