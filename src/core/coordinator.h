#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <QThread>

class Coordinator : public QThread
{
public:
    Coordinator(QObject *parent);
};

#endif // COORDINATOR_H
