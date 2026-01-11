#include "processpanel.h"
#include "ui_processpanel.h"
#include <QMessageBox>

ProcessPanel::ProcessPanel(ProcessModel *model, QWidget *parent) : QDialog(parent)
{
    ui = std::make_unique<Ui::ProcessPanel>();
    ui->setupUi(this);
    setModel(model);
}

ProcessPanel::~ProcessPanel() {}

QString ProcessPanel::getName() const
{
    return ui->nameEdit->text();
}

void ProcessPanel::setName(const QString &value)
{
    ui->nameEdit->setText(value);
}

qint64 ProcessPanel::getPriority() const
{
    return ui->priorityEdit->value();
}

void ProcessPanel::setPriority(qint64 value)
{
    ui->priorityEdit->setValue(value);
}

qint64 ProcessPanel::getBurstTime() const
{
    return ui->burstEdit->value();
}

void ProcessPanel::setBurstTime(qint64 value)
{
    ui->burstEdit->setValue(value);
}

QString ProcessPanel::getFileName() const
{
    return ui->fileEdit->text();
}

void ProcessPanel::setFileName(const QString &value)
{
    ui->fileEdit->setText(value);
}

bool ProcessPanel::dependsOnFile() const
{
    return ui->fileGroup->isChecked();
}

void ProcessPanel::setDependsOnFile(bool value)
{
    ui->fileGroup->setChecked(value);
}

ProcessInfo ProcessPanel::getProcessInfo() const
{
    ProcessInfo info;
    info.setName(getName());
    info.setFileName(getFileName());
    info.setPriority(getPriority());
    info.setBurstTime(getBurstTime());
    return info;
}

void ProcessPanel::setProcessInfo(const ProcessInfo &info)
{
    setName(info.getName());
    setPriority(info.getPriority());
    setFileName(info.getFileName());
    setBurstTime(info.getBurstTime());
}

QModelIndex ProcessPanel::getParent() const
{
    return ui->locationView->currentIndex();
}

void ProcessPanel::setParent(const QModelIndex &parent)
{
    ui->locationView->setCurrentIndex(parent);
}

ProcessModel *ProcessPanel::getModel()
{
    return static_cast<ProcessModel*>(ui->locationView->model());
}

void ProcessPanel::setModel(ProcessModel *model)
{
    ui->locationView->setModel(model);
    auto visible = model->rowCount() > 0;
    ui->emptyLabel->setVisible(!visible);
    ui->locationView->setVisible(visible);
}

void ProcessPanel::expandFrom(const QModelIndex &index)
{
    ui->locationView->expand(index);
}

void ProcessPanel::accept()
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
    else if (dependsOnFile() && getFileName().isEmpty())
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
