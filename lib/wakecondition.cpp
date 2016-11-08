#include "wakecondition.h"
#include "fiber.h"
#include <QFuture>

using namespace CoQt;

WakeCondition::WakeCondition(QSharedPointer<Fiber> parent)
    : QObject(parent.data())
    , m_pFiber(parent)
{

}

WakeCondition::~WakeCondition()
{

}

void WakeCondition::wake()
{
    m_pFiber->wake();
}

ImmediateWakeCondition::ImmediateWakeCondition(QSharedPointer<Fiber> parent)
    : WakeCondition(parent)
    , m_wakeTime(QDateTime::currentDateTime())
{

}

ImmediateWakeCondition::~ImmediateWakeCondition()
{
}

QDateTime ImmediateWakeCondition::wakeTime()
{
    return m_wakeTime;
}

bool ImmediateWakeCondition::canWake()
{
    return true;
}


SleepWakeCondition::SleepWakeCondition(int iSleepMs, QSharedPointer<Fiber> parent)
    : WakeCondition(parent)
    , m_wakeTime(QDateTime::currentDateTime())
{
    m_wakeTime = m_wakeTime.addMSecs(iSleepMs);
}

SleepWakeCondition::~SleepWakeCondition()
{
}

QDateTime SleepWakeCondition::wakeTime()
{
    return m_wakeTime;
}

bool SleepWakeCondition::canWake()
{
    return (m_wakeTime <= QDateTime::currentDateTime());
}




LambdaWakeCondition::LambdaWakeCondition(std::function<bool()> func, QSharedPointer<Fiber> parent, int iPollInterval)
    : WakeCondition(parent)
    , m_wakeTime(QDateTime::currentDateTime())
    , m_func(func)
    , m_iPollInterval(iPollInterval)
{
    m_wakeTime = m_wakeTime.addMSecs(iPollInterval);
}

LambdaWakeCondition::~LambdaWakeCondition()
{
}

QDateTime LambdaWakeCondition::wakeTime()
{
    return m_wakeTime;
}

bool LambdaWakeCondition::canWake()
{
    bool bResult = m_func();

    if(!bResult)
        m_wakeTime = QDateTime::currentDateTime().addMSecs(m_iPollInterval);

    return bResult;
}


SignalWakeCondition::SignalWakeCondition(QSharedPointer<Fiber> parent)
    : WakeCondition(parent)
{
}

SignalWakeCondition::~SignalWakeCondition()
{
}

QDateTime SignalWakeCondition::wakeTime()
{
    return QDateTime();
}

bool SignalWakeCondition::canWake()
{
    return false;
}


