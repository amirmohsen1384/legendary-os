#include <QMessageBox>
#include "agentedit.h"
#include "ui_agentedit.h"

AgentEdit::AgentEdit(AgentModel *model, QWidget *parent) : AgentEdit(parent)
{
    setModel(model);
}

AgentEdit::AgentEdit(QWidget *parent) : QDialog(parent)
{
    ui = std::make_unique<Ui::AgentEdit>();
    ui->setupUi(this);
}

AgentEdit::~AgentEdit()
{
    delete ui;
}

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
    return ui->agentView->currentIndex();
}

void AgentEdit::setParent(const QModelIndex &index)
{
    ui->agentView->setCurrentIndex(index);
}

AgentModel *AgentEdit::getModel() const
{
    return ui->agentView->model();
}

void AgentEdit::setModel(AgentModel *model)
{
    ui->agentView->setModel(model);
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
