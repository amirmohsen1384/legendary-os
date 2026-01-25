#ifndef SIMPLETEST_H
#define SIMPLETEST_H

#include "panels/mainpanel.h"
#include <QObject>
#include <QTest>

class SimpleTest : public QObject
{
    Q_OBJECT
public:
    explicit SimpleTest(MainPanel *window, QObject *parent = nullptr);

    void run();

private:
    MainPanel *panel {};
};

#endif // SIMPLETEST_H
