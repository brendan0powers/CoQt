#include "wakecondition.h"
#include "fiber.h"

using namespace CoQt;

WakeCondition::WakeCondition(QSharedPointer<Fiber> parent)
    : QObject(parent.data())
    , m_pFiber(parent)
{

}

WakeCondition::~WakeCondition()
{

}



ImmediateWakeConiditon::ImmediateWakeConiditon(QSharedPointer<Fiber> parent)
    : WakeCondition(parent)
    , m_wakeTime(QDateTime::currentDateTime())
{

}

ImmediateWakeConiditon::~ImmediateWakeConiditon()
{
}

QDateTime ImmediateWakeConiditon::wakeTime()
{
    return m_wakeTime;
}

bool ImmediateWakeConiditon::canWake()
{
    return true;
}

void ImmediateWakeConiditon::wake()
{
    m_pFiber->wake();
}


SleepWakeConiditon::SleepWakeConiditon(int iSleepMs, QSharedPointer<Fiber> parent)
    : WakeCondition(parent)
    , m_wakeTime(QDateTime::currentDateTime())
{
    m_wakeTime = m_wakeTime.addMSecs(iSleepMs);
}

SleepWakeConiditon::~SleepWakeConiditon()
{
}

QDateTime SleepWakeConiditon::wakeTime()
{
    return m_wakeTime;
}

bool SleepWakeConiditon::canWake()
{
    return (m_wakeTime <= QDateTime::currentDateTime());
}

void SleepWakeConiditon::wake()
{
    m_pFiber->wake();
}




LambdaWakeConiditon::LambdaWakeConiditon(std::function<bool()> func, QSharedPointer<Fiber> parent, int iPollInterval)
    : WakeCondition(parent)
    , m_wakeTime(QDateTime::currentDateTime())
    , m_func(func)
    , m_iPollInterval(iPollInterval)
{
    m_wakeTime = m_wakeTime.addMSecs(iPollInterval);
}

LambdaWakeConiditon::~LambdaWakeConiditon()
{
}

QDateTime LambdaWakeConiditon::wakeTime()
{
    return m_wakeTime;
}

bool LambdaWakeConiditon::canWake()
{
    bool bResult = m_func();

    if(!bResult)
        m_wakeTime = QDateTime::currentDateTime().addMSecs(m_iPollInterval);

    return m_func();
}

void LambdaWakeConiditon::wake()
{
    m_pFiber->wake();
}

