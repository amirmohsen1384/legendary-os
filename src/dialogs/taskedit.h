#ifndef TASKEDIT_H
#define TASKEDIT_H

#include <QDialog>
#include "agentedit.h"
#include "models/taskmodel.h"

namespace Ui
{
    class TaskEdit;
}

class TaskEdit : public QDialog
{
    Q_OBJECT
private slots:
    void browseAgent();

public:
    explicit TaskEdit(QWidget *parent = nullptr);
    ~TaskEdit();

    QString getName() const;
    void setName(const QString &value);

    qint64 getPriority() const;
    void setPriority(qint64 value);

    qint64 getBurstTime() const;
    void setBurstTime(qint64 value);

    QString getAgent() const;
    void setAgent(const QString &value);

    bool dependency() const;
    void setDependency(bool value);

    TaskInfo getTaskInfo() const;
    void setTaskInfo(const TaskInfo &info);

    QModelIndex getParent() const;
    void setParent(const QModelIndex &parent);

    TaskModel* getTaskModel();
    void setTaskModel(TaskModel *model);

    AgentModel* getAgentModel();
    void setAgentModel(AgentModel *model);

    void expandFrom(const QModelIndex &index);

public:
    virtual void accept();

private:
    Ui::TaskEdit* ui {};
    AgentModel* agents {};
};

#endif // TASKEDIT_H
