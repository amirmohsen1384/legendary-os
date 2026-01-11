#include "processpanel.h"
#include "ui_processpanel.h"

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
