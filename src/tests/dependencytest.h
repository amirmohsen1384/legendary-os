#ifndef DEPENDENCYTEST_H
#define DEPENDENCYTEST_H

#include "panels/mainpanel.h"
#include <QObject>
#include <QTest>

class DependencyTest : public QObject
{
    Q_OBJECT
public:
    explicit DependencyTest(MainPanel *window, QObject *parent = nullptr);

    void run();

private:
    MainPanel *panel {};
};

#endif // DEPENDENCYTEST_H
