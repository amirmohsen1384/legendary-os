#include <QMessageBox>
#include <QApplication>
#include "core/system.h"
#include "models/processmodel.h"
#include "panels/processpanel.h"
#include <QTreeView>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QModelIndex index;
    ProcessModel model;
    while (true)
    {
        ProcessPanel panel(&model);
        panel.expandFrom(index);
        if(panel.exec() == QDialog::Accepted)
        {
            auto info = panel.getProcessInfo();
            model.insert(info, panel.getParent());
            index = panel.getParent();
        }
        else
        {
            break;
        }
    }

    while (true)
    {
        QEventLoop loop;
        QTreeView view;
        view.setModel(&model);
        view.show();

        QPushButton button("Delete");
        button.show();

        QObject::connect(&button, &QPushButton::clicked,
        [&]()
        {
            model.remove(view.currentIndex());
        }
        );

        QPushButton close("Close");
        close.show();

        QObject::connect(&close, &QPushButton::clicked, &loop, &QEventLoop::exit);

        if(!loop.exec())
        {
            break;
        }
    }
    return 0;
}
