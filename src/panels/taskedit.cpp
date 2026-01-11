#include "taskedit.h"
#include <QMessageBox>
#include "ui_taskedit.h"

TaskEdit::TaskEdit(TaskModel *model, QWidget *parent) : QDialog(parent)
{
    ui = std::make_unique<Ui::TaskEdit>();
    ui->setupUi(this);
    setModel(model);
}

TaskEdit::~TaskEdit() {}

QString TaskEdit::getName() const
{
    return ui->nameEdit->text();
}

void TaskEdit::setName(const QString &value)
{
    ui->nameEdit->setText(value);
}

qint64 TaskEdit::getPriority() const
{
    return ui->priorityEdit->value();
}

void TaskEdit::setPriority(qint64 value)
{
    ui->priorityEdit->setValue(value);
}

qint64 TaskEdit::getBurstTime() const
{
    return ui->burstEdit->value();
}

void TaskEdit::setBurstTime(qint64 value)
{
    ui->burstEdit->setValue(value);
}

QString TaskEdit::getResource() const
{
    return ui->resourceEdit->text();
}

void TaskEdit::setResource(const QString &value)
{
    ui->resourceEdit->setText(value);
}

bool TaskEdit::dependency() const
{
    return ui->resourceGroup->isChecked();
}

void TaskEdit::setDependency(bool value)
{
    ui->resourceGroup->setChecked(value);
}

TaskInfo TaskEdit::getTaskInfo() const
{
    TaskInfo info;
    info.setName(getName());
    info.setPriority(getPriority());
    info.setBurstTime(getBurstTime());
    info.setResource(getResource());
    return info;
}

void TaskEdit::setTaskInfo(const TaskInfo &info)
{
    setName(info.getName());
    setPriority(info.getPriority());
    setResource(info.getResource());
    setBurstTime(info.getBurstTime());
}

QModelIndex TaskEdit::getParent() const
{
    return ui->locationView->currentIndex();
}

void TaskEdit::setParent(const QModelIndex &parent)
{
    ui->locationView->setCurrentIndex(parent);
}

TaskModel *TaskEdit::getModel()
{
    return static_cast<TaskModel*>(ui->locationView->model());
}

void TaskEdit::setModel(TaskModel *model)
{
    ui->locationView->setModel(model);
    auto visible = model->rowCount() > 0;
    ui->emptyLabel->setVisible(!visible);
    ui->locationView->setVisible(visible);
}

void TaskEdit::expandFrom(const QModelIndex &index)
{
    ui->locationView->expand(index);
}

void TaskEdit::accept()
{
    if (getName().isEmpty())
    {
        QMessageBox::warning(
            this, "No name provided",
            "You have entered the name of the process."
        );
        return;
    }
    else if (getPriority() <= 0)
    {
        QMessageBox::warning(
            this, "No priority is set",
            "You have set the priority.\n\n"
            "The prority should be at least 1 and at most 100."
        );
        return;
    }
    else if (getBurstTime() <= 0)
    {
        QMessageBox::warning(
            this, "No burst time is set",
            "You have set the burst time.\n\n"
            "The burst time should be at least 1 and at most 100."
        );
        return;
    }
    else if (dependency() && getResource().isEmpty())
    {
        QMessageBox::warning(
            this, "No file name is set",
            "You have set the file name.\n\n"
            "You need to set a file name as you enable file dependency."
        );
        return;
    }
    QDialog::accept();
}
