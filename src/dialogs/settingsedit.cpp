#include "ui_settingsedit.h"
#include "settingsedit.h"
#include <QMessageBox>

void SettingsEdit::resetPauseDelayDuration()
{
    Settings::Info load = Settings::load();
    ui->pauseDelayDurationEdit->setValue(load.pause);
}

void SettingsEdit::resetQuantumLength()
{
    Settings::Info load = Settings::load();
    ui->quantumLengthEdit->setValue(load.quantumSize);
}

void SettingsEdit::resetReadyQueueLimit()
{
    Settings::Info load = Settings::load();
    ui->readyQueueLimitEdit->setValue(load.readyQueueLimit);
}

void SettingsEdit::resetInputCommandLimit()
{
    Settings::Info load = Settings::load();
    ui->inputCommandLimitEdit->setValue(load.inputCommandLimit);
}

void SettingsEdit::resetExecutionCycle()
{
    Settings::Info load = Settings::load();
    ui->executionCycleEdit->setValue(load.executionCycle);
}

SettingsEdit::SettingsEdit(QWidget *parent) : QDialog(parent)
{
    ui = std::make_unique<Ui::SettingsEdit>();
    ui->setupUi(this);
}

SettingsEdit::~SettingsEdit() {}

Settings::Info SettingsEdit::getSettings() const
{
    Settings::Info result;
    result.inputCommandLimit = ui->inputCommandLimitEdit->value();
    result.readyQueueLimit = ui->readyQueueLimitEdit->value();
    result.executionCycle = ui->executionCycleEdit->value();
    result.quantumSize = ui->quantumLengthEdit->value();
    result.pause = ui->pauseDelayDurationEdit->value();
    return result;
}

void SettingsEdit::setSettings(const Settings::Info &info)
{
    ui->inputCommandLimitEdit->setValue(info.inputCommandLimit);
    ui->readyQueueLimitEdit->setValue(info.readyQueueLimit);
    ui->executionCycleEdit->setValue(info.executionCycle);
    ui->quantumLengthEdit->setValue(info.quantumSize);
    ui->pauseDelayDurationEdit->setValue(info.pause);
    emit settingsChanged(info);
}
