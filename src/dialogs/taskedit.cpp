#include "taskedit.h"
#include <QMessageBox>
#include "ui_taskedit.h"

void TaskEdit::browseAgent()
{
    if(!agents) {
        return;
    }
    AgentEdit editor;
    editor.setModel(agents);
    if (editor.exec() == QDialog::Accepted)
    {
        auto index = editor.getParent();
        auto info = editor.getAgentInfo();
        ui->agentEdit->setText(agents->toString(index).append("/%1").arg(info.getName()));
    }
}

TaskEdit::TaskEdit(QWidget *parent) : QDialog(parent), ui(new Ui::TaskEdit)
{
    ui->setupUi(this);
}

TaskEdit::~TaskEdit()
{
    delete ui;
}

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

QString TaskEdit::getAgent() const
{
    return ui->agentEdit->text();
}

void TaskEdit::setAgent(const QString &value)
{
    ui->agentEdit->setText(value);
}

bool TaskEdit::dependency() const
{
    return ui->agentGroup->isChecked();
}

void TaskEdit::setDependency(bool value)
{
    ui->agentGroup->setChecked(value);
}

TaskInfo TaskEdit::getTaskInfo() const
{
    TaskInfo info;
    info.setName(getName());
    info.setPriority(getPriority());
    info.setBurstTime(getBurstTime());
    info.setAgent(getAgent());
    return info;
}

void TaskEdit::setTaskInfo(const TaskInfo &info)
{
    setName(info.getName());
    setAgent(info.getAgent());
    setPriority(info.getPriority());
    setBurstTime(info.getBurstTime());
}

QModelIndex TaskEdit::getParent() const
{
    auto selection = ui->locationView->selectionModel();
    if (!selection->hasSelection()) {
        return {};
    }
    return selection->selectedIndexes().constFirst();
}

void TaskEdit::setParent(const QModelIndex &parent)
{
    ui->locationView->selectionModel()->select(parent, QItemSelectionModel::Select);
}

TaskModel *TaskEdit::getTaskModel()
{
    return static_cast<TaskModel*>(ui->locationView->model());
}

void TaskEdit::setTaskModel(TaskModel *model)
{
    ui->locationView->setModel(model);
    auto visible = model && model->rowCount() > 0;
    ui->emptyLabel->setVisible(!visible);
    ui->locationView->setVisible(visible);
}

AgentModel *TaskEdit::getAgentModel()
{
    return agents;
}

void TaskEdit::setAgentModel(AgentModel *model)
{
    agents = model;
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
    else if (dependency() && getAgent().isEmpty())
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
