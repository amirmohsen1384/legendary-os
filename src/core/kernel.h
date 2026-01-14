#ifndef KERNEL_H
#define KERNEL_H

#include "core/system.h"
#include "models/taskmodel.h"
#include "models/agentmodel.h"
#include "models/loggingmodel.h"
#include "models/readytaskmodel.h"
#include "models/waitingtaskmodel.h"

class CentralKernel : public QObject
{
    Q_OBJECT
protected:
    void dispatch(const QPersistentModelIndex &index);

public:
    explicit CentralKernel(const Config::Info &info, QObject *parent = nullptr);

    bool isLocked() const;
    bool hasPrerequisits() const;

    TaskModel* taskModel();
    AgentModel* agentModel();
    LoggingModel* loggingModel();
    ReadyTaskModel* readyTaskModel();
    WaitingTaskModel* waitingTaskModel();

public slots:
    void lock();
    void unlock();
    void setLocked(bool value);
    void setTaskModel(TaskModel* model);
    void setAgentModel(AgentModel* model);
    void setLoggingModel(LoggingModel* model);
    void setReadyTaskModel(ReadyTaskModel* model);
    void setWaitingTaskModel(WaitingTaskModel *model);

    void addAgent(const AgentInfo &info, const QModelIndex &parent);
    void addTask(const TaskInfo &info, const QModelIndex &parent);
    void removeAgent(const QModelIndex &index);
    void removeTask(const QModelIndex &index);
    void shutdown();
    void execute();

signals:
    void taskModelChanged();
    void agentModelChanged();
    void loggingModelChanged();
    void readyTaskModelChanged();
    void waitingTaskModelChanged();

    void scheduledForShutdown();
    void lockChanged(bool value);
private:
    bool locked;
    qint64 inputLimit;
    Config::Info settings;
    qint64 elapsedQuantum;
    bool scheduleForShutdown;
    TaskModel* tasks = nullptr;
    AgentModel* agents = nullptr;
    LoggingModel* logger = nullptr;
    ReadyTaskModel* readyTasks = nullptr;
    WaitingTaskModel* waitingTasks = nullptr;
};

#endif // KERNEL_H
