#ifndef LOGICPROBE_H
#define LOGICPROBE_H

#include <QObject>
#include "enums.h"

class LogicProbe : public QObject
{
    Q_OBJECT
public:
    explicit LogicProbe(QObject *parent = 0);

signals:
    void stateChanged(const State & state);
};

#endif // LOGICPROBE_H
