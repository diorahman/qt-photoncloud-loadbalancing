#include "driver.h"
#include <QDebug>
#include <QTimer>

StdOutputListener::StdOutputListener()
{
    probe = new DebugProbe();
}

StdOutputListener::~StdOutputListener()
{
    if(probe) delete probe;
}

void StdOutputListener::write(const ExitGames::Common::JString& str)
{
    emit probe->writing(str);
}

void StdOutputListener::writeLine(const ExitGames::Common::JString& str)
{
    emit probe->writing(str);
}

Driver::Driver(QObject *parent) :
    QObject(parent)
{
    m_logic = new NetworkLogic(&m_outputListener, L"1.0");
    m_timer = new QTimer(this);
    m_timer->setInterval(100);

    connect(m_outputListener.probe, SIGNAL(writing(ExitGames::Common::JString)), this, SLOT(debug(ExitGames::Common::JString)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(logic()));
    connect(m_logic->getStateAccessor(), SIGNAL(stateChanged()), this, SLOT(onStateChanged()));
}

Driver::~Driver()
{
    if(m_logic) delete m_logic;
}

void Driver::drive()
{
    m_timer->start();
}

void Driver::logic()
{
    m_logic->run();
}

void Driver::debug(const ExitGames::Common::JString& str)
{
    qDebug() << "[debug] : " << QString::fromWCharArray(str);
}

void Driver::onStateChanged()
{
    switch (m_logic->getState()) {
    case STATE_INITIALIZED: emit stateChanged("initialized"); break;
    case STATE_CONNECTING: emit stateChanged("connecting"); break;
    case STATE_CONNECTED: emit stateChanged("connected"); break;
    default:
        break;
    }
}

DebugProbe::DebugProbe(QObject *parent) : QObject(parent)
{}

DebugProbe::~DebugProbe()
{}
