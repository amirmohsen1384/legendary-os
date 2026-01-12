#include <QApplication>
#include "src/panels/mainpanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Config::initialize();
    auto config = Config::load();

    MainPanel panel;
    panel.show();

    return app.exec();
}
