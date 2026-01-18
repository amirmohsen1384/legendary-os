#ifndef SETTINGSEDIT_H
#define SETTINGSEDIT_H

#include <QDialog>
#include "core/system.h"

namespace Ui
{
    class SettingsEdit;
}

class SettingsEdit : public QDialog
{
    Q_OBJECT
private slots:
    void resetQuantumLength();
    void resetExecutionCycle();
    void resetReadyQueueLimit();
    void resetInputCommandLimit();
    void resetPauseDelayDuration();

public:
    explicit SettingsEdit(QWidget *parent = nullptr);
    ~SettingsEdit();

    Settings::Info getSettings() const;

public slots:
    void setSettings(const Settings::Info &info);

signals:
    void settingsChanged(Settings::Info info);

private:
    std::unique_ptr<Ui::SettingsEdit> ui;
};

#endif // SETTINGSEDIT_H
