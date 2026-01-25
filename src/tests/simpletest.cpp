#include "simpletest.h"
#include "dialogs/taskedit.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QTreeView>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QRandomGenerator>

SimpleTest::SimpleTest(MainPanel *window, QObject *parent)
    : panel(window) {}

void SimpleTest::run()
{
    QVERIFY(panel);

    panel->show();
    QVERIFY(QTest::qWaitForWindowActive(panel));

    auto insertTask = panel->findChild<QAction*>("actionInsertTask");
    QVERIFY(insertTask);

    auto createTaskViaDialog = [&](const QString &name, int priority, int burst, const QModelIndex &parentIndex = QModelIndex())
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

        auto nameEdit     = taskEdit->findChild<QLineEdit*>("nameEdit");
        auto burstEdit    = taskEdit->findChild<QSpinBox*>("burstEdit");
        auto priorityEdit = taskEdit->findChild<QSpinBox*>("priorityEdit");
        auto locationView = taskEdit->findChild<QTreeView*>("locationView");
        auto buttons      = taskEdit->findChild<QDialogButtonBox*>("controlButton");

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

        QCoreApplication::processEvents();

        auto okButton = buttons->button(QDialogButtonBox::Ok);
        QVERIFY(okButton);
        QTest::mouseClick(okButton, Qt::LeftButton);

        QVERIFY(QTest::qWaitFor([&] {
            return taskEdit.isNull();
        }, 1000));
    };



    QVector<QModelIndex> topLevelIndexes;

    auto mainView = panel->findChild<QTreeView*>("tasksView");
    QVERIFY(mainView);
    auto model = mainView->model();
    QVERIFY(model);

    for (int i = 0; i < 3; ++i)
    {
        auto generator = QRandomGenerator::global();
        createTaskViaDialog(QString("Top Task %1").arg(i + 1), generator->bounded(1, 100), generator->bounded(30, 100));

        QVERIFY(QTest::qWaitFor([&] {
            return model->rowCount() == i + 1;
        }, 1000));

        QModelIndex idx = model->index(i, 0);
        QVERIFY(idx.isValid());
        topLevelIndexes.append(idx);
    }

    // --- Create 3 children for each top-level task ---
    int counter = 1;
    auto generator = QRandomGenerator::global();
    for (int parent = 0; parent < topLevelIndexes.size(); ++parent) {
        for (int c = 0; c < 5; ++c) {
            createTaskViaDialog(
                QString("Child %1 of Top %2")
                    .arg(counter++)
                    .arg(parent + 1),
                generator->bounded(1, 100),
                generator->bounded(30, 100),
                topLevelIndexes[parent]
                );
        }
    }
}
