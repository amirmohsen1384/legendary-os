#include "mainpanel.h"
#include "ui_mainpanel.h"
#include <QMessageBox>

void MainPanel::showAboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}

MainPanel::MainPanel(const Config::Info &info, QWidget *parent) : QMainWindow(parent), settings(info)
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

    ui->setupUi(this);
}

MainPanel::~MainPanel() {}
