#include <QApplication>
#include "src/panels/mainpanel.h"
#include "src/tests/stresstest.h"
#include "src/tests/simpletest.h"
#include "src/tests/dependencytest.h"

#define NORMAL_MODE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Settings::initialize(app);

    auto settings = Settings::load();
    MainPanel panel(settings);

#ifdef STRESS_TEST
    StressTest test(&panel);
    test.run();
#endif

#ifdef SIMPLE_TEST
    SimpleTest test(&panel);
    test.run();
#endif

#ifdef DEPENDENCY_TEST
    DependencyTest test(&panel);
    test.run();
#endif

#ifdef NORMAL_MODE
    panel.show();
#endif

    return app.exec();
}
