#ifndef STRESSTEST_H
#define STRESSTEST_H

#include "panels/mainpanel.h"
#include <QObject>
#include <QTest>

class StressTest : public QObject
{
    Q_OBJECT
public:
    explicit StressTest(MainPanel *window, QObject *parent = nullptr);

    void run();

private:
    MainPanel *panel {};
};

#endif // STRESSTEST_H
