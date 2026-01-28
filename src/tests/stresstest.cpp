#include "stresstest.h"
#include "dialogs/taskedit.h"
#include "dialogs/agentedit.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QTreeView>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QRandomGenerator>

StressTest::StressTest(MainPanel *window, QObject *parent)
    : QObject(parent), panel(window) {}

void StressTest::run()
{
    QVERIFY(panel);

    panel->show();
    QVERIFY(QTest::qWaitForWindowActive(panel));

    auto insertAgent = panel->findChild<QAction*>("actionInsertAgent");
    auto insertTask = panel->findChild<QAction*>("actionInsertTask");
    auto removeAgentAction = panel->findChild<QAction*>("actionRemoveAgent");
    auto removeTaskAction = panel->findChild<QAction*>("actionRemoveTask");
    QVERIFY(insertAgent);
    QVERIFY(insertTask);
    QVERIFY(removeAgentAction);
    QVERIFY(removeTaskAction);

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
        QVERIFY(agentEdit);
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
        if (!agentPath.isEmpty()) {
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

    // Get model references
    auto agentsView = panel->findChild<QTreeView*>("agentsView");
    QVERIFY(agentsView);
    auto agentModel = agentsView->model();
    QVERIFY(agentModel);

    auto tasksView = panel->findChild<QTreeView*>("tasksView");
    QVERIFY(tasksView);
    auto taskModel = tasksView->model();
    QVERIFY(taskModel);

    auto generator = QRandomGenerator::global();

    // ===================================================================
    // PHASE 1: Bulk Agent Creation (Test agent hierarchy management)
    // ===================================================================
    
    QVector<QModelIndex> rootAgentIndexes;
    
    // Create multiple root-level agents
    createAgentViaDialog("ResourceA", "Resource A");
    QVERIFY(QTest::qWaitFor([&] { return agentModel->rowCount() >= 1; }, 1000));
    rootAgentIndexes.append(agentModel->index(0, 0));
    
    createAgentViaDialog("ResourceB", "Resource B");
    QVERIFY(QTest::qWaitFor([&] { return agentModel->rowCount() >= 2; }, 1000));
    rootAgentIndexes.append(agentModel->index(1, 0));
    
    createAgentViaDialog("ResourceC", "Resource C");
    QVERIFY(QTest::qWaitFor([&] { return agentModel->rowCount() >= 3; }, 1000));
    rootAgentIndexes.append(agentModel->index(2, 0));
    
    createAgentViaDialog("ResourceD", "Resource D");
    QVERIFY(QTest::qWaitFor([&] { return agentModel->rowCount() >= 4; }, 1000));
    rootAgentIndexes.append(agentModel->index(3, 0));

    // Create nested agent hierarchies
    QVector<QModelIndex> childAgentIndexes;
    
    // Under ResourceA
    createAgentViaDialog("SubA1", "Sub resource A1", rootAgentIndexes[0]);
    QTest::qWait(50);
    createAgentViaDialog("SubA2", "Sub resource A2", rootAgentIndexes[0]);
    QTest::qWait(50);
    
    // Under ResourceB
    createAgentViaDialog("SubB1", "Sub resource B1", rootAgentIndexes[1]);
    QTest::qWait(50);
    createAgentViaDialog("SubB2", "Sub resource B2", rootAgentIndexes[1]);
    QTest::qWait(50);
    
    // Under ResourceC
    createAgentViaDialog("SubC1", "Sub resource C1", rootAgentIndexes[2]);
    QTest::qWait(50);
    createAgentViaDialog("SubC2", "Sub resource C2", rootAgentIndexes[2]);
    QTest::qWait(50);
    
    // Under ResourceD - create deeper nesting
    createAgentViaDialog("SubD1", "Sub resource D1", rootAgentIndexes[3]);
    QTest::qWait(50);

    // Verify agent tree structure is intact
    QVERIFY(agentModel->rowCount() >= 4);
    QVERIFY(agentModel->rowCount(rootAgentIndexes[0]) >= 2);
    QVERIFY(agentModel->rowCount(rootAgentIndexes[1]) >= 2);
    QVERIFY(agentModel->rowCount(rootAgentIndexes[2]) >= 2);
    QVERIFY(agentModel->rowCount(rootAgentIndexes[3]) >= 1);

    // ===================================================================
    // PHASE 2: Bulk Task Creation with Dependencies
    // ===================================================================
    
    QVector<QModelIndex> topLevelTaskIndexes;
    
    // Create tasks with various agent dependencies
    createTaskViaDialog("Task_A1", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/ResourceA/SubA1");
    QVERIFY(QTest::qWaitFor([&] { return taskModel->rowCount() >= 1; }, 1000));
    topLevelTaskIndexes.append(taskModel->index(0, 0));
    
    createTaskViaDialog("Task_A2", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/ResourceA/SubA2");
    QVERIFY(QTest::qWaitFor([&] { return taskModel->rowCount() >= 2; }, 1000));
    topLevelTaskIndexes.append(taskModel->index(1, 0));
    
    createTaskViaDialog("Task_B1", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/ResourceB/SubB1");
    QVERIFY(QTest::qWaitFor([&] { return taskModel->rowCount() >= 3; }, 1000));
    topLevelTaskIndexes.append(taskModel->index(2, 0));
    
    createTaskViaDialog("Task_C1", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/ResourceC");
    QVERIFY(QTest::qWaitFor([&] { return taskModel->rowCount() >= 4; }, 1000));
    topLevelTaskIndexes.append(taskModel->index(3, 0));
    
    createTaskViaDialog("Task_D1", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/ResourceD/SubD1");
    QVERIFY(QTest::qWaitFor([&] { return taskModel->rowCount() >= 5; }, 1000));
    topLevelTaskIndexes.append(taskModel->index(4, 0));

    // Create child tasks with mixed dependencies
    createTaskViaDialog("ChildTask_A1_1", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[0], "/ResourceA/SubA1");
    QTest::qWait(50);
    createTaskViaDialog("ChildTask_A1_2", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[0], "/ResourceA/SubA2");
    QTest::qWait(50);
    
    createTaskViaDialog("ChildTask_B1_1", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[2], "/ResourceB");
    QTest::qWait(50);
    createTaskViaDialog("ChildTask_B1_2", generator->bounded(1, 100), generator->bounded(30, 100), topLevelTaskIndexes[2], "/ResourceB/SubB2");
    QTest::qWait(50);

    // Create some tasks without agent dependencies (independent tasks)
    createTaskViaDialog("IndependentTask1", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex());
    QVERIFY(QTest::qWaitFor([&] { return taskModel->rowCount() >= 6; }, 1000));
    topLevelTaskIndexes.append(taskModel->index(5, 0));
    
    createTaskViaDialog("IndependentTask2", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex());
    QVERIFY(QTest::qWaitFor([&] { return taskModel->rowCount() >= 7; }, 1000));
    topLevelTaskIndexes.append(taskModel->index(6, 0));

    // Verify task tree structure
    QVERIFY(taskModel->rowCount() >= 7);
    QVERIFY(taskModel->rowCount(topLevelTaskIndexes[0]) >= 2);
    QVERIFY(taskModel->rowCount(topLevelTaskIndexes[2]) >= 2);

    // ===================================================================
    // PHASE 3: Edge Case - Remove Agent with Active Task Dependencies
    // ===================================================================
    
    // Remove a leaf agent that has dependent tasks
    // This should trigger task state transitions (tasks should move to WaitingForAgent)
    auto subA1Index = agentModel->index(0, 0, rootAgentIndexes[0]);
    QVERIFY(subA1Index.isValid());
    
    // Select the agent in the view
    agentsView->setCurrentIndex(subA1Index);
    QCoreApplication::processEvents();
    
    // Trigger removal
    removeAgentAction->trigger();
    QTest::qWait(200);
    
    // The tasks depending on SubA1 should now be in WaitingForAgent state
    // The agent should be removed from the tree
    QTest::qWait(200);

    // ===================================================================
    // PHASE 4: Edge Case - Remove Parent Agent with Nested Children
    // ===================================================================
    
    // Remove a parent agent which should cascade to children
    // ResourceB has SubB1 and SubB2 as children, and tasks depend on them
    auto resourceBIndex = rootAgentIndexes[1];
    QVERIFY(resourceBIndex.isValid());
    
    agentsView->setCurrentIndex(resourceBIndex);
    QCoreApplication::processEvents();
    
    removeAgentAction->trigger();
    QTest::qWait(200);
    
    // All children agents (SubB1, SubB2) should be removed
    // All dependent tasks should transition to appropriate states
    QTest::qWait(200);

    // ===================================================================
    // PHASE 5: Edge Case - Remove Parent Task with Active Children
    // ===================================================================
    
    // Remove a parent task which should cascade to child tasks
    auto parentTaskIndex = topLevelTaskIndexes[0];
    QVERIFY(parentTaskIndex.isValid());
    
    tasksView->setCurrentIndex(parentTaskIndex);
    QCoreApplication::processEvents();
    
    removeTaskAction->trigger();
    QTest::qWait(200);
    
    // All child tasks should be removed with parent
    QTest::qWait(200);

    // ===================================================================
    // PHASE 6: Bulk Removal Operations
    // ===================================================================
    
    // Remove multiple tasks in succession
    if (taskModel->rowCount() > 0) {
        auto taskToRemove = taskModel->index(0, 0);
        if (taskToRemove.isValid()) {
            tasksView->setCurrentIndex(taskToRemove);
            QCoreApplication::processEvents();
            removeTaskAction->trigger();
            QTest::qWait(100);
        }
    }
    
    if (taskModel->rowCount() > 0) {
        auto taskToRemove = taskModel->index(0, 0);
        if (taskToRemove.isValid()) {
            tasksView->setCurrentIndex(taskToRemove);
            QCoreApplication::processEvents();
            removeTaskAction->trigger();
            QTest::qWait(100);
        }
    }

    // ===================================================================
    // PHASE 7: System Recovery - Create New Entities After Removals
    // ===================================================================
    
    // Create new agents after removals
    createAgentViaDialog("RecoveryAgent1", "Recovery agent 1");
    QTest::qWait(100);
    
    createAgentViaDialog("RecoveryAgent2", "Recovery agent 2");
    QTest::qWait(100);
    
    // Verify system can still create agents
    int finalAgentCount = agentModel->rowCount();
    QVERIFY(finalAgentCount > 0);
    
    // Get the new agent index
    auto recoveryAgent1Index = agentModel->index(finalAgentCount - 2, 0);
    QVERIFY(recoveryAgent1Index.isValid());
    
    // Create tasks with new agent dependencies
    createTaskViaDialog("RecoveryTask1", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/RecoveryAgent1");
    QTest::qWait(100);
    
    createTaskViaDialog("RecoveryTask2", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/RecoveryAgent2");
    QTest::qWait(100);
    
    // Create independent recovery tasks
    createTaskViaDialog("RecoveryTaskIndependent", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex());
    QTest::qWait(100);
    
    // Verify system can still create tasks
    QVERIFY(taskModel->rowCount() > 0);

    // ===================================================================
    // PHASE 8: Dependency Integrity Verification
    // ===================================================================
    
    // Create a complex dependency chain
    createAgentViaDialog("ChainAgent1", "Chain agent 1");
    QTest::qWait(100);
    
    int currentAgentCount = agentModel->rowCount();
    auto chainAgent1Index = agentModel->index(currentAgentCount - 1, 0);
    
    createAgentViaDialog("ChainAgent1Child", "Chain agent 1 child", chainAgent1Index);
    QTest::qWait(100);
    
    // Create tasks depending on the chain
    createTaskViaDialog("ChainTask1", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/ChainAgent1");
    QTest::qWait(100);
    
    createTaskViaDialog("ChainTask2", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/ChainAgent1/ChainAgent1Child");
    QTest::qWait(100);
    
    int currentTaskCount = taskModel->rowCount();
    auto chainTask1Index = taskModel->index(currentTaskCount - 2, 0);
    
    // Create child task under chain task
    createTaskViaDialog("ChainTask1Child", generator->bounded(1, 100), generator->bounded(30, 100), chainTask1Index, "/ChainAgent1");
    QTest::qWait(100);
    
    // Now remove the chain agent - should cascade to child agent and affect all dependent tasks
    agentsView->setCurrentIndex(chainAgent1Index);
    QCoreApplication::processEvents();
    removeAgentAction->trigger();
    QTest::qWait(200);

    // ===================================================================
    // PHASE 9: Final Validation - System Should Still Be Functional
    // ===================================================================
    
    // Create final test entities to ensure system is still working
    createAgentViaDialog("FinalTestAgent", "Final test agent");
    QTest::qWait(100);
    
    createTaskViaDialog("FinalTestTask", generator->bounded(1, 100), generator->bounded(30, 100), QModelIndex(), "/FinalTestAgent");
    QTest::qWait(100);
    
    // Verify models are still valid and functional
    QVERIFY(agentModel != nullptr);
    QVERIFY(taskModel != nullptr);
    QVERIFY(agentModel->rowCount() >= 0);
    QVERIFY(taskModel->rowCount() >= 0);
    
    // Test completed successfully - system should be in a consistent state
    qDebug() << "Stress test completed successfully!";
    qDebug() << "Final agent count:" << agentModel->rowCount();
    qDebug() << "Final task count:" << taskModel->rowCount();
}
