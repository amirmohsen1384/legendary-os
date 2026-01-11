#ifndef TASKEDIT_H
#define TASKEDIT_H

#include <QDialog>
#include "models/taskmodel.h"

namespace Ui
{
    class TaskEdit;
}

class TaskEdit : public QDialog
{
    Q_OBJECT
public:
    explicit TaskEdit(TaskModel* model, QWidget *parent = nullptr);
    ~TaskEdit();

    QString getName() const;
    void setName(const QString &value);

    qint64 getPriority() const;
    void setPriority(qint64 value);

    qint64 getBurstTime() const;
    void setBurstTime(qint64 value);

    QString getResource() const;
    void setResource(const QString &value);

    bool dependency() const;
    void setDependency(bool value);

    TaskInfo getTaskInfo() const;
    void setTaskInfo(const TaskInfo &info);

    QModelIndex getParent() const;
    void setParent(const QModelIndex &parent);

    TaskModel* getModel();
    void setModel(TaskModel *model);

    void expandFrom(const QModelIndex &index);

public:
    virtual void accept();

private:
    std::unique_ptr<Ui::TaskEdit> ui;
};

#endif // TASKEDIT_H
