#include <QMessageBox>
#include "agentedit.h"
#include "ui_agentedit.h"

AgentEdit::AgentEdit(QWidget *parent) : QDialog(parent)
{
    ui = std::make_unique<Ui::AgentEdit>();
    ui->setupUi(this);
}

AgentEdit::~AgentEdit() {}

QString AgentEdit::getName() const
{
    return ui->nameEdit->text();
}

void AgentEdit::setName(const QString &value)
{
    ui->nameEdit->setText(value);
}

QString AgentEdit::getDescription() const
{
    return ui->descriptionEdit->text();
}

void AgentEdit::setDescription(const QString &value)
{
    ui->descriptionEdit->setText(value);
}

QModelIndex AgentEdit::getParent() const
{
    auto selection = ui->agentView->selectionModel();
    if (!selection->hasSelection()) {
        return {};
    }
    return selection->selectedIndexes().constFirst();
}

void AgentEdit::setParent(const QModelIndex &parent)
{
    ui->agentView->selectionModel()->select(parent, QItemSelectionModel::Select);
}

AgentInfo AgentEdit::getAgentInfo() const
{
    AgentInfo info;
    info.setName(getName());
    info.setDescription(getDescription());
    return info;
}

void AgentEdit::setAgentInfo(const AgentInfo &info)
{
    setName(info.getName());
    setDescription(info.getDescription());
}

AgentModel *AgentEdit::getModel() const
{
    return static_cast<AgentModel*>(ui->agentView->model());
}

void AgentEdit::setModel(AgentModel *model)
{
    ui->agentView->setModel(model);
    auto visible = model && model->rowCount() > 0;
    ui->emptyLabel->setVisible(!visible);
    ui->agentView->setVisible(visible);
}

void AgentEdit::expandFrom(const QModelIndex &index)
{
    ui->agentView->expand(index);
}

void AgentEdit::accept()
{
    if (getName().isEmpty())
    {
        QMessageBox::warning(
            this, "No name provided",
            "You have set a name for the agent.\n\n"
            "Agents need to be associated with a name in the agent hiearchy.");
        return;
    }
    QDialog::accept();
}
