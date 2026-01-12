#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QMainWindow>
#include "core/system.h"
#include "core/taskqueue.hpp"
#include "core/restqueue.hpp"
#include "models/taskmodel.h"
#include "models/agentmodel.h"

namespace Ui
{
    class MainPanel;
}

class MainPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPanel(const Config::Info &info, QWidget *parent = nullptr);
    ~MainPanel();

    qint64 getElapsedQuantum() const;

private:
    TaskModel tasks;
    AgentModel agents;
    TaskQueue readyQueue;
    Config::Info settings;
    RestQueue waitingQueue;
    qint64 elapsedQuantum = 0;
    std::unique_ptr<Ui::MainPanel> ui;
};

#endif // MAINPANEL_H
