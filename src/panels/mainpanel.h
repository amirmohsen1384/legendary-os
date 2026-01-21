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
private:
    void setupLayout();
    void setupKernel();
    void setupModels();
    void setupEditionActions();

private:
    bool isTasksTabVisible() const;
    bool isAgentsTabVisible() const;
    bool isSizeLimitTabVisible() const;
    bool isReadyTasksTabVisible() const;
    bool isAgentDependencyTabVisible() const;

private slots:
    void insertTask();
    void showAboutQt();
    void openGitHubPage();
    void showSettingsDialog();

private slots:
    void updateLogView();
    void updateTaskView();
    void updateAgentView();
    void updateReadyTaskView();
    void updateSizeLimitView();
    void updateAgentDependencyView();

private slots:
    void setLogsTabVisible(bool visible);
    void setTasksTabVisible(bool visible);
    void setAgentsTabVisible(bool visible);
    void setSizeLimitTabVisible(bool visible);
    void setReadyTasksTabVisible(bool visible);
    void setAgentDependencyTabVisible(bool visible);

public:
    explicit MainPanel(const Settings::Info &info, QWidget *parent = nullptr);
    ~MainPanel();

private:
    TaskModel* tasks {};
    Ui::MainPanel* ui {};
    AgentModel* agents {};
    LoggingModel* logs {};
    Coordinator* kernel {};
    ReadyModel* readyTasks {};
    Settings::Info settings {};
    PriorityModel* limitTasks {};
    PriorityModel* agentTasks {};
    QActionGroup* editionActions {};
};

#endif // MAINPANEL_H
