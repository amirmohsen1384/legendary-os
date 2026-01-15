#ifndef KERNEL_H
#define KERNEL_H

#include "models/loggingmodel.h"
#include "models/agentmodel.h"
#include "models/readymodel.h"
#include "models/restmodel.h"
#include "models/taskmodel.h"

class Kernel : public QObject
{
    Q_OBJECT
    bool dispatch(const QModelIndex &index);

public:
    explicit Kernel(QObject *parent = nullptr);
    bool hasRequirements();

public slots:
    bool insertAgent(const AgentInfo &info, const QModelIndex &parent);
    bool insertTask(const TaskInfo &info, const QModelIndex &parent);
    bool removeAgent(const QModelIndex &index);
    bool removeTask(const QModelIndex &index);
    bool scheduleShutdown();

signals:
    void shutdownScheduled();

private:
    ReadyModel* readyTasks {};
    LoggingModel* logger {};
    RestModel* restTasks {};
    AgentModel* agents {};
    TaskModel* tasks {};
    bool shutdown = false;
};

#endif // KERNEL_H
