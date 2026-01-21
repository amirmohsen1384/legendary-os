#include <QMessageBox>
#include "mainpanel.h"
#include <QActionGroup>
#include "ui_mainpanel.h"
#include "dialogs/taskedit.h"
#include "dialogs/settingsedit.h"

void MainPanel::setupLayout()
{
    ui->actionLogs->setChecked(false);
    ui->actionReadyTasks->setChecked(true);
    ui->actionSizeLimitTasks->setChecked(true);
    ui->actionAvailableTasks->setChecked(true);
    ui->actionAvailableAgents->setChecked(true);
    ui->actionAgentDependencyTasks->setChecked(true);
}

void MainPanel::setupKernel()
{
    kernel = new Coordinator(settings, this);

    tasks = new TaskModel(this);
    kernel->setTasks(tasks);

    agents = new AgentModel(this);
    kernel->setAgents(agents);

    logs = new LoggingModel(this);
    kernel->setLogs(logs);

    limitTasks = new PriorityModel(this);
    kernel->setLimitTasks(limitTasks);

    agentTasks = new PriorityModel(this);
    kernel->setAgentTasks(agentTasks);

    readyTasks = new ReadyModel(settings.readyQueueLimit, this);
    kernel->setReadyTasks(readyTasks);
}

void MainPanel::setupModels()
{
    ui->tasksView->setModel(tasks);
    updateTaskView();

    ui->agentsView->setModel(agents);
    updateAgentView();

    ui->readyTaskView->setModel(readyTasks);
    updateReadyTaskView();

    ui->sizeLimitView->setModel(limitTasks);
    updateSizeLimitView();

    ui->agentDependencyView->setModel(agentTasks);
    updateAgentDependencyView();

    ui->logsView->setModel(logs);
    updateLogView();
}

void MainPanel::setupEditionActions()
{
    editionActions = new QActionGroup(this);
    editionActions->addAction(ui->actionShutdown);
    editionActions->addAction(ui->actionInsertTask);
    editionActions->addAction(ui->actionRemoveTask);
    editionActions->addAction(ui->actionInsertAgent);
    editionActions->addAction(ui->actionImportAgent);
    editionActions->addAction(ui->actionRemoveAgent);
}

bool MainPanel::isTasksTabVisible() const
{
    return ui->entitiesGroup->isTabVisible(0);
}

bool MainPanel::isAgentsTabVisible() const
{
    return ui->entitiesGroup->isTabVisible(1);
}

bool MainPanel::isSizeLimitTabVisible() const
{
    return ui->queueGroup->isTabVisible(1);
}

bool MainPanel::isReadyTasksTabVisible() const
{
    return ui->queueGroup->isTabVisible(0);
}

bool MainPanel::isAgentDependencyTabVisible() const
{
    return ui->queueGroup->isTabVisible(2);
}

void MainPanel::showAboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainPanel::showSettingsDialog()
{
    SettingsEdit editor;
    editor.setSettings(settings);
    if(editor.exec() == QDialog::Accepted)
    {
        Settings::save(editor.getSettings());
        QMessageBox::information(this, "Restart Required", "You need to restart the application to apply the new settings.");
        QApplication::quit();
    }
}

void MainPanel::updateLogView()
{
    auto condition = logs->rowCount() > 0;
    ui->logsView->setVisible(condition);
    ui->logsEmptyLabel->setVisible(!condition);
}

void MainPanel::updateTaskView()
{
    auto condition = tasks->rowCount() > 0;
    ui->tasksEmptyLabel->setVisible(!condition);
    ui->tasksView->setVisible(condition);
}

void MainPanel::updateAgentView()
{
    auto condition = agents->rowCount() > 0;
    ui->agentsEmptyLabel->setVisible(!condition);
    ui->agentsView->setVisible(condition);
}

void MainPanel::updateReadyTaskView()
{
    auto condition = readyTasks->rowCount() > 0;
    ui->readyTaskEmptyLabel->setVisible(!condition);
    ui->readyTaskView->setVisible(condition);
}

void MainPanel::updateSizeLimitView()
{
    auto condition = limitTasks->rowCount() > 0;
    ui->sizeLimitView->setVisible(condition);
    ui->sizeLimitEmptyLabel->setVisible(!condition);
}

void MainPanel::updateAgentDependencyView()
{
    auto condition = agentTasks->rowCount() > 0;
    ui->agentDependencyView->setVisible(condition);
    ui->agentDependencyEmptyLabel->setVisible(!condition);
}

void MainPanel::setLogsTabVisible(bool visible)
{
    ui->logsGroup->setVisible(visible);
}

void MainPanel::setTasksTabVisible(bool visible)
{
    ui->entitiesGroup->setTabVisible(0, visible);
    ui->entitiesGroup->setVisible(isTasksTabVisible() || isAgentsTabVisible());
}

void MainPanel::setAgentsTabVisible(bool visible)
{
    ui->entitiesGroup->setTabVisible(1, visible);
    ui->entitiesGroup->setVisible(isTasksTabVisible() || isAgentsTabVisible());
}

void MainPanel::setSizeLimitTabVisible(bool visible)
{
    ui->queueGroup->setTabVisible(1, visible);
    ui->queueGroup->setVisible(isSizeLimitTabVisible() || isReadyTasksTabVisible() || isAgentDependencyTabVisible());
}

void MainPanel::setReadyTasksTabVisible(bool visible)
{
    ui->queueGroup->setTabVisible(0, visible);
    ui->queueGroup->setVisible(isSizeLimitTabVisible() || isReadyTasksTabVisible() || isAgentDependencyTabVisible());
}

void MainPanel::setAgentDependencyTabVisible(bool visible)
{
    ui->queueGroup->setTabVisible(2, visible);
    ui->queueGroup->setVisible(isSizeLimitTabVisible() || isReadyTasksTabVisible() || isAgentDependencyTabVisible());
}

void MainPanel::insertTask()
{
    TaskEdit editor(tasks);
    if (editor.exec() == QDialog::Accepted)
    {
        kernel->insertTask(editor.getTaskInfo(), editor.getParent());
        updateTaskView();
    }
}

MainPanel::MainPanel(const Settings::Info &info, QWidget *parent) : QMainWindow(parent), settings(info), ui(new Ui::MainPanel)
{
    setupKernel();
    ui->setupUi(this);
    setupEditionActions();
    setupLayout();
    setupModels();
}

MainPanel::~MainPanel()
{
    delete ui;
}
