#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QMessageBox>
#include <QMainWindow>
#include <QActionGroup>
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
    void confirm(
        const QString &text,
        std::function<void(bool)> handler,
        const QString &information = QString(),
        const QMessageBox::Icon icon = QMessageBox::Warning
    );

private:
    void setupViews();
    void setupLayout();
    void setupKernel();
    void setupModels();
    void setupStatusBar();
    void setupConnections();
    void setupEditionActions();

private:
    bool isTasksTabVisible() const;
    bool isAgentsTabVisible() const;
    bool isSizeLimitTabVisible() const;
    bool isReadyTasksTabVisible() const;
    bool isAgentDependencyTabVisible() const;

private slots:
    void insertTask();
    void insertAgent();

    void removeTask();
    void removeAgent();

    void shutdown();

    void showAboutQt();
    void openGitHubPage();
    void showSettingsDialog();

    void startKernel();
    void pauseKernel();

private slots:
    void updateViews();
    void updateLogView();
    void updateTaskView();
    void updateAgentView();
    void updateEmptyFrame();
    void updateReadyTaskView();
    void updateSizeLimitView();
    void updateAgentDependencyView();

private slots:
    void showLogsDock();
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
