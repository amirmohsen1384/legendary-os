#include <QMessageBox>
#include "mainpanel.h"
#include "ui_mainpanel.h"

#include "dialogs/taskedit.h"
#include "dialogs/settingsedit.h"

void MainPanel::showAboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainPanel::updateTaskView()
{
    auto condition = tasks->rowCount() > 0;
    ui->emptyLabel->setVisible(!condition);
    ui->tasksView->setVisible(condition);
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

void MainPanel::insertTask()
{
    TaskEdit editor(tasks.get());
    if (editor.exec() == QDialog::Accepted)
    {
        kernel->insertTask(editor.getTaskInfo(), editor.getParent());
        updateTaskView();
    }
}

MainPanel::MainPanel(const Settings::Info &info, QWidget *parent) : QMainWindow(parent), settings(info)
{
    tasks = std::make_unique<TaskModel>();
    ui = std::make_unique<Ui::MainPanel>();
    agents = std::make_unique<AgentModel>();
    logs = std::make_unique<LoggingModel>();
    limitTasks = std::make_unique<PriorityModel>();
    agentTasks = std::make_unique<PriorityModel>();
    kernel = std::make_unique<Coordinator>(settings);
    readyTasks = std::make_unique<ReadyModel>(settings.readyQueueLimit);

    kernel->setLogs(logs.get());
    kernel->setTasks(tasks.get());
    kernel->setAgents(agents.get());
    kernel->setReadyTasks(readyTasks.get());
    kernel->setLimitTasks(limitTasks.get());
    kernel->setAgentTasks(agentTasks.get());

    ui->setupUi(this);
    ui->tasksView->setModel(tasks.get());

    updateTaskView();
}

MainPanel::~MainPanel() {}
