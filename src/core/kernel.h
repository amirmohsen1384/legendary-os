#ifndef KERNEL_H
#define KERNEL_H

#include <QObject>
#include "core/system.h"
#include "core/taskqueue.hpp"
#include "core/restqueue.hpp"
#include "models/taskmodel.h"
#include "models/agentmodel.h"
#include "models/loggingmodel.h"

class CentralKernel : public QObject
{
    Q_OBJECT
public:
    explicit CentralKernel(const Config::Info &info, QObject *parent = nullptr);

    void addTask(const TaskInfo &info, const QModelIndex &parent);
    void removeTask(const QModelIndex &index);

    void addAgent(const AgentInfo &info, const QModelIndex &parent);
    void removeAgent(const QModelIndex &index);

    void shutdown();
    void execute();

    bool isLocked() const;

    TaskModel* showcastTasks();
    AgentModel* showcastAgents();
    LoggingModel* showcastLogs();
    TaskQueue* showcastReadyTasks();
    RestQueue* showcastWaitingTasks();

public slots:
    void lock();
    void unlock();
    void setLocked(bool value);

signals:
    void scheduledForShutdown();
    void lockChanged(bool value);

private:
    bool locked;
    qint64 inputLimit;
    Config::Info settings;
    qint64 elapsedQuantum;
    bool scheduleForShutdown;

    TaskModel tasks;
    AgentModel agents;
    LoggingModel logger;
    TaskQueue readyQueue;
    RestQueue waitingQueue;
};

#endif // KERNEL_H
