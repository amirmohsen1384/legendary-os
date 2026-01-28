#include "dependencytest.h"
#include "dialogs/taskedit.h"
#include "dialogs/agentedit.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QTreeView>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QRandomGenerator>

DependencyTest::DependencyTest(MainPanel *window, QObject *parent)
    : panel(window) {}

void DependencyTest::run()
{
    QVERIFY(panel);

    panel->show();
    QVERIFY(QTest::qWaitForWindowActive(panel));

    auto insertAgent = panel->findChild<QAction*>("actionInsertAgent");
    auto insertTask = panel->findChild<QAction*>("actionInsertTask");
    QVERIFY(insertAgent);
    QVERIFY(insertTask);

    // Lambda to create an agent via dialog
    auto createAgentViaDialog = [&](const QString &name, const QString &description, const QModelIndex &parentIndex = QModelIndex())
    {
        const int existingCount = panel->findChildren<AgentEdit*>("agenteditor").size();

        insertAgent->trigger();

        QPointer<AgentEdit> agentEdit;

        QVERIFY(QTest::qWaitFor([&] {
            auto dialogs = panel->findChildren<AgentEdit*>("agenteditor");
            if (dialogs.size() <= existingCount)
                return false;

            agentEdit = dialogs.last();
            return agentEdit && agentEdit->isVisible();
        }, 1000));

        QVERIFY(agentEdit);

        auto nameEdit = agentEdit->findChild<QLineEdit*>("nameEdit");
        auto descEdit = agentEdit->findChild<QLineEdit*>("descriptionEdit");
        auto locationView = agentEdit->findChild<QTreeView*>("agentView");
        auto buttons = agentEdit->findChild<QDialogButtonBox*>("control");

        QVERIFY(nameEdit);
        QVERIFY(descEdit);
        QVERIFY(locationView);
        QVERIFY(buttons);

        nameEdit->setText(name);
        descEdit->setText(description);

        if (parentIndex.isValid()) {
            auto *sel = locationView->selectionModel();
            sel->clearSelection();
            sel->select(parentIndex, QItemSelectionModel::ClearAndSelect);
        }

        QCoreApplication::processEvents();

        auto okButton = buttons->button(QDialogButtonBox::Ok);
        QVERIFY(okButton);
        QTest::mouseClick(okButton, Qt::LeftButton);

        QVERIFY(QTest::qWaitFor([&] {
            return agentEdit.isNull();
        }, 1000));
    };

    // Lambda to create a task via dialog with optional agent dependency
    auto createTaskViaDialog = [&](const QString &name, int priority, int burst, const QModelIndex &parentIndex = QModelIndex(), const QString &agentPath = QString())
    {
        const int existingCount = panel->findChildren<TaskEdit*>("taskeditor").size();

        insertTask->trigger();

        QPointer<TaskEdit> taskEdit;

        QVERIFY(QTest::qWaitFor([&] {
            auto dialogs = panel->findChildren<TaskEdit*>("taskeditor");
            if (dialogs.size() <= existingCount)
                return false;

            taskEdit = dialogs.last();
            return taskEdit && taskEdit->isVisible();
        }, 1000));

        QVERIFY(taskEdit);

        auto nameEdit = taskEdit->findChild<QLineEdit*>("nameEdit");
        auto burstEdit = taskEdit->findChild<QSpinBox*>("burstEdit");
        auto priorityEdit = taskEdit->findChild<QSpinBox*>("priorityEdit");
        auto locationView = taskEdit->findChild<QTreeView*>("locationView");
        auto agentEdit = taskEdit->findChild<QLineEdit*>("agentEdit");
        auto buttons = taskEdit->findChild<QDialogButtonBox*>("controlButton");

        QVERIFY(nameEdit);
        QVERIFY(priorityEdit);
        QVERIFY(burstEdit);
        QVERIFY(locationView);
        QVERIFY(buttons);

        nameEdit->setText(name);
        auto clamp = [](int v) {
            return std::clamp(v, 1, 100);
        };
        priorityEdit->setValue(clamp(priority));
        burstEdit->setValue(burst);

        if (parentIndex.isValid()) {
            auto *sel = locationView->selectionModel();
            sel->clearSelection();
            sel->select(parentIndex, QItemSelectionModel::ClearAndSelect);
        }

        // Set agent dependency if specified
        if (!agentPath.isEmpty() && agentEdit) {
            agentEdit->setText(agentPath);
        }

        QCoreApplication::processEvents();

        auto okButton = buttons->button(QDialogButtonBox::Ok);
        QVERIFY(okButton);
        QTest::mouseClick(okButton, Qt::LeftButton);

        QVERIFY(QTest::qWaitFor([&] {
            return taskEdit.isNull();
        }, 1000));
    };

    // Create agent tree structure
    auto agentsView = panel->findChild<QTreeView*>("agentsView");
    QVERIFY(agentsView);
    auto agentModel = agentsView->model();
    QVERIFY(agentModel);

    // Create root-level agents
    QVector<QModelIndex> rootAgentIndexes;

    createAgentViaDialog("System", "System resources");
    QVERIFY(QTest::qWaitFor([&] {
        return agentModel->rowCount() >= 1;
    }, 1000));
    rootAgentIndexes.append(agentModel->index(0, 0));

    createAgentViaDialog("Network", "Network resources");
    QVERIFY(QTest::qWaitFor([&] {
        return agentModel->rowCount() >= 2;
    }, 1000));
    rootAgentIndexes.append(agentModel->index(1, 0));

    createAgentViaDialog("Storage", "Storage resources");
    QVERIFY(QTest::qWaitFor([&] {
        return agentModel->rowCount() >= 3;
    }, 1000));
    rootAgentIndexes.append(agentModel->index(2, 0));

    // Create child agents under System
    createAgentViaDialog("CPU", "CPU resources", rootAgentIndexes[0]);
    createAgentViaDialog("Memory", "Memory resources", rootAgentIndexes[0]);

    // Create child agents under Network
    createAgentViaDialog("Ethernet", "Ethernet interface", rootAgentIndexes[1]);
    createAgentViaDialog("WiFi", "WiFi interface", rootAgentIndexes[1]);

    // Create child agents under Storage
    createAgentViaDialog("Disk", "Hard disk storage", rootAgentIndexes[2]);
    createAgentViaDialog("Cache", "Cache storage", rootAgentIndexes[2]);

    // Now create tasks with agent dependencies
    auto tasksView = panel->findChild<QTreeView*>("tasksView");
    QVERIFY(tasksView);
    auto taskModel = tasksView->model();
    QVERIFY(taskModel);

    QVector<QModelIndex> topLevelTaskIndexes;
    auto generator = QRandomGenerator::global();

    // Create tasks with different agent dependencies
    createTaskViaDialog("Compute Task", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/System/CPU");
    QVERIFY(QTest::qWaitFor([&] {
        return taskModel->rowCount() >= 1;
    }, 1000));
    topLevelTaskIndexes.append(taskModel->index(0, 0));

    createTaskViaDialog("Memory Task", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/System/Memory");
    QVERIFY(QTest::qWaitFor([&] {
        return taskModel->rowCount() >= 2;
    }, 1000));
    topLevelTaskIndexes.append(taskModel->index(1, 0));

    createTaskViaDialog("Network Task", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/Network/Ethernet");
    QVERIFY(QTest::qWaitFor([&] {
        return taskModel->rowCount() >= 3;
    }, 1000));
    topLevelTaskIndexes.append(taskModel->index(2, 0));

    createTaskViaDialog("Storage Task", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/Storage/Disk");
    QVERIFY(QTest::qWaitFor([&] {
        return taskModel->rowCount() >= 4;
    }, 1000));
    topLevelTaskIndexes.append(taskModel->index(3, 0));

    // Create child tasks with agent dependencies
    createTaskViaDialog("CPU SubTask 1", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[0], "/System/CPU");
    createTaskViaDialog("CPU SubTask 2", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[0], "/System/CPU");

    createTaskViaDialog("Memory SubTask 1", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[1], "/System/Memory");
    createTaskViaDialog("Memory SubTask 2", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[1], "/System/Memory");

    createTaskViaDialog("Network SubTask 1", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[2], "/Network/WiFi");
    createTaskViaDialog("Network SubTask 2", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[2], "/Network/Ethernet");

    createTaskViaDialog("Storage SubTask 1", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[3], "/Storage/Cache");
    createTaskViaDialog("Storage SubTask 2", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[3], "/Storage/Disk");
}
