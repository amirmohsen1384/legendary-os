#include <QTreeView>
#include <QTableView>
#include <QApplication>
#include "src/dialogs/taskedit.h"
#include "src/models/readymodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TaskModel model;
    ReadyModel readyQueue;

    while (true)
    {
        TaskEdit editor(&model);
        if (editor.exec() == QDialog::Accepted)
        {
            model.insertTask(editor.getTaskInfo(), editor.getParent());
        }
        else
        {
            break;
        }
    }

    QTreeView overall;
    overall.setModel(&model);
    overall.show();

    QTableView readyView;
    readyView.setSelectionBehavior(QTableView::SelectRows);
    readyView.setModel(&readyQueue);
    readyView.show();

    QObject::connect(&overall, &QTreeView::activated, [&](const QModelIndex &index) {
        if(!readyQueue.insertTask(index)) {
            qDebug() << "Failed to add the task.";
        }
    });

    QObject::connect(&readyView, &QTableView::activated, [&](const QModelIndex &index) {
        if (!readyQueue.removeRows(index.row(), 1)) {
            qDebug() << "Failed to remove the tasks.";
        }
    });

    return app.exec();
}
