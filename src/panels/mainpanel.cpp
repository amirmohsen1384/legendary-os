#include "mainpanel.h"
#include "ui_mainpanel.h"
#include <QDesktopServices>
#include "dialogs/taskedit.h"
#include "dialogs/settingsedit.h"

void MainPanel::confirm(const QString &text, std::function<void(bool)> handler, const QString &information, const QMessageBox::Icon icon)
{
    auto message = new QMessageBox(this);
    message->setIcon(icon);
    if(!information.isEmpty()) {
        message->setInformativeText(information);
    }
    message->setText(text);
    message->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    message->setDefaultButton(QMessageBox::Yes);

    message->setMinimumSize(message->sizeHint());
    message->adjustSize();

    QObject::connect(message, &QMessageBox::finished, [handler, message](int result) {
        handler(result == QMessageBox::Yes);
        message->deleteLater();
    });

    message->open();
}

void MainPanel::setupViews()
{
    auto makeStretch = [this](QTableView *view) {
        auto header = view->horizontalHeader();
        header->setSectionResizeMode(QHeaderView::Interactive);
        header->setSectionResizeMode(header->count() - 1, QHeaderView::Stretch);
    };
    makeStretch(ui->logsView);
    makeStretch(ui->readyTaskView);
    makeStretch(ui->sizeLimitView);
    makeStretch(ui->agentDependencyView);
}

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

void MainPanel::setupConnections()
{
    connect(kernel, &Coordinator::shutdownScheduled, this, [this]() {
        statusBar()->showMessage("Scheduled the system for a shutdown", 3000);
    });
    auto hook = [this](QAbstractItemModel *model, auto updater) {
        connect(model, &QAbstractItemModel::rowsInserted, this,
                [this, updater](const QModelIndex &, int, int) {
                    (this->*updater)();
                });

        connect(model, &QAbstractItemModel::rowsRemoved, this,
                [this, updater](const QModelIndex &, int, int) {
                    (this->*updater)();
                });

        connect(model, &QAbstractItemModel::modelReset, this,
                [this, updater]() {
                    (this->*updater)();
                });
    };
    hook(logs, &MainPanel::updateLogView);
    hook(tasks, &MainPanel::updateTaskView);
    hook(agents, &MainPanel::updateAgentView);
    hook(limitTasks, &MainPanel::updateSizeLimitView);
    hook(readyTasks, &MainPanel::updateReadyTaskView);
    hook(agentTasks, &MainPanel::updateAgentDependencyView);
    connect(ui->tasksView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this](const QItemSelection &current, const QItemSelection &) {
            ui->actionRemoveTask->setDisabled(current.isEmpty());
        }
    );
    connect(ui->agentsView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [this](const QItemSelection &current, const QItemSelection &) {
            ui->actionRemoveAgent->setDisabled(current.isEmpty());
        }
    );
    connect(ui->entitiesGroup, &QTabWidget::currentChanged, [this](int) {
        ui->tasksView->selectionModel()->clear();
        ui->agentsView->selectionModel()->clear();
    });
    connect(ui->queueGroup, &QTabWidget::currentChanged, [this](int) {
        ui->readyTaskView->selectionModel()->clear();
        ui->sizeLimitView->selectionModel()->clear();
        ui->agentDependencyView->selectionModel()->clear();
    });
}


void MainPanel::setupEditionActions()
{
    editionActions = new QActionGroup(this);
    editionActions->addAction(ui->actionShutdown);
    editionActions->addAction(ui->actionInsertTask);
    editionActions->addAction(ui->actionRemoveTask);
    editionActions->addAction(ui->actionInsertAgent);
    editionActions->addAction(ui->actionRemoveAgent);
}

bool MainPanel::isLogsTabVisible() const
{
    return ui->logsGroup->isVisible();
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

void MainPanel::openGitHubPage()
{
    qDebug() << "Showing GitHub Page";
    if(!QDesktopServices::openUrl(QUrl("https://github.com/UI-DS-2025/LegendaryOS.git")))
    {
        qDebug() << "Failed to open the GitHub Page.";
    }

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

void MainPanel::updateViews()
{
    updateTaskView();
    updateAgentView();
    updateReadyTaskView();
    updateSizeLimitView();
    updateAgentDependencyView();
    updateLogView();
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

void MainPanel::updateEmptyFrame()
{
    ui->emptyFrame->setVisible(
        !isLogsTabVisible() &&
        !isAgentsTabVisible() &&
        !isTasksTabVisible() &&
        !isSizeLimitTabVisible() &&
        !isReadyTasksTabVisible() &&
        !isAgentDependencyTabVisible()
    );
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
    updateEmptyFrame();
}

void MainPanel::setTasksTabVisible(bool visible)
{
    ui->entitiesGroup->setTabVisible(0, visible);
    ui->entitiesGroup->setVisible(isTasksTabVisible() || isAgentsTabVisible());
    updateEmptyFrame();
}

void MainPanel::setAgentsTabVisible(bool visible)
{
    ui->entitiesGroup->setTabVisible(1, visible);
    ui->entitiesGroup->setVisible(isTasksTabVisible() || isAgentsTabVisible());
    updateEmptyFrame();
}

void MainPanel::setSizeLimitTabVisible(bool visible)
{
    ui->queueGroup->setTabVisible(1, visible);
    ui->queueGroup->setVisible(isSizeLimitTabVisible() || isReadyTasksTabVisible() || isAgentDependencyTabVisible());
    updateEmptyFrame();
}

void MainPanel::setReadyTasksTabVisible(bool visible)
{
    ui->queueGroup->setTabVisible(0, visible);
    ui->queueGroup->setVisible(isSizeLimitTabVisible() || isReadyTasksTabVisible() || isAgentDependencyTabVisible());
    updateEmptyFrame();
}

void MainPanel::setAgentDependencyTabVisible(bool visible)
{
    ui->queueGroup->setTabVisible(2, visible);
    ui->queueGroup->setVisible(isSizeLimitTabVisible() || isReadyTasksTabVisible() || isAgentDependencyTabVisible());
    updateEmptyFrame();
}

void MainPanel::insertTask()
{
    auto editor = new TaskEdit(this);
    editor->setObjectName("taskeditor");
    editor->setAgentModel(agents);
    editor->setTaskModel(tasks);
    connect(editor, &QDialog::accepted, this, [this, editor]() {
        kernel->insertTask(editor->getTaskInfo(), editor->getParent());
    });
    connect(editor, &QDialog::finished, editor, &QObject::deleteLater);
    editor->open();
}

void MainPanel::insertAgent()
{
    auto editor = new AgentEdit(this);
    editor->setObjectName("agenteditor");
    editor->setModel(agents);
    connect(editor, &QDialog::accepted, this, [this, editor]() {
        const auto info = editor->getAgentInfo();
        if (!info.isValid()) {
            return;
        }
        kernel->insertAgent(info, editor->getParent());
    });
    connect(editor, &QDialog::finished, editor, &QObject::deleteLater);
    editor->open();
}

void MainPanel::removeTask()
{
    auto selection = ui->tasksView->selectionModel();
    if (!selection->hasSelection()) {
        return;
    }

    QPersistentModelIndex index = selection->selectedIndexes().constFirst();
    if (!index.isValid()) {
        return;
    }

    confirm("Are you sure to delete this task?",
        [this, index](bool result) {
            if (!result || !index.isValid()) {
                return;
            }
            kernel->removeTask(index);
        },
        "This will remove the task from all queues even if it still running."
    );
}

void MainPanel::removeAgent()
{
    auto selection = ui->agentsView->selectionModel();
    if (!selection->hasSelection()) {
        return;
    }

    QPersistentModelIndex index = selection->selectedIndexes().constFirst();
    if (!index.isValid()) {
        return;
    }

    confirm("Are you sure to delete this agent?",
        [this, index](bool accepted) {
            if (!accepted || !index.isValid()) {
                return;
            }
            kernel->removeAgent(index);
        },
        "This will revert depending tasks to the agent dependency queue if available."
    );
}


void MainPanel::shutdown()
{
    kernel->scheduleShutdown();
}

MainPanel::MainPanel(const Settings::Info &info, QWidget *parent) : QMainWindow(parent), settings(info), ui(new Ui::MainPanel)
{
    ui->setupUi(this);
    setupKernel();
    setupEditionActions();
    setupLayout();
    setupModels();
    setupConnections();
    updateEmptyFrame();
    setupViews();
    showMaximized();
}

MainPanel::~MainPanel()
{
    delete ui;
}
