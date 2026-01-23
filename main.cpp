#include <QTreeView>
#include <QTableView>
#include <QApplication>
#include "src/panels/mainpanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Settings::initialize(app);

    auto settings = Settings::load();
    MainPanel panel(settings);
    panel.show();

    return app.exec();
}
