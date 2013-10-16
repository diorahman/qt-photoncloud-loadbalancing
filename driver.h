#ifndef DRIVER_H
#define DRIVER_H

#include <QObject>
#include "enums.h"
#include "NetworkLogic.h"

class DebugProbe : public QObject{
    Q_OBJECT
public:
    DebugProbe(QObject * parent = 0);
    ~DebugProbe();
signals:
    void writing(const ExitGames::Common::JString & str);
};

class StdOutputListener: public OutputListener
{

public:
    StdOutputListener();
    ~StdOutputListener();
    virtual void write(const ExitGames::Common::JString& str);
    virtual void writeLine(const ExitGames::Common::JString& str);

public:
    DebugProbe * probe;
};

class QTimer;

class Driver : public QObject
{
    Q_OBJECT
public:
    explicit Driver(QObject *parent = 0);
    ~Driver();

private:
    NetworkLogic * m_logic;
    StdOutputListener m_outputListener;
    QTimer * m_timer;

signals:
    void stateChanged(const QString & logicState);

public slots:
    void drive();

private slots:
    void logic();
    void debug(const ExitGames::Common::JString& str);
    void onStateChanged();

};

#endif // DRIVER_H
