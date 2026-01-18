#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QMainWindow>
#include "core/system.h"
#include "core/coordinator.h"
#include "models/taskmodel.h"
#include "models/readymodel.h"
#include "models/agentmodel.h"
#include "models/loggingmodel.h"
#include "models/prioritymodel.h"

namespace Ui
{
    class MainPanel;
}

class MainPanel : public QMainWindow
{
    Q_OBJECT

private slots:
    void showAboutQt();
    void showSettingsDialog();

public:
    explicit MainPanel(const Settings::Info &info, QWidget *parent = nullptr);
    ~MainPanel();

private:
    Settings::Info settings {};
    std::unique_ptr<TaskModel> tasks {};
    std::unique_ptr<Ui::MainPanel> ui {};
    std::unique_ptr<AgentModel> agents {};
    std::unique_ptr<LoggingModel> logs {};
    std::unique_ptr<Coordinator> kernel {};
    std::unique_ptr<ReadyModel> readyTasks {};
    std::unique_ptr<PriorityModel> limitTasks {};
    std::unique_ptr<PriorityModel> agentTasks {};
};

#endif // MAINPANEL_H
