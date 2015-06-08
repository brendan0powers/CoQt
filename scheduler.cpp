#include "scheduler.h"
#include <QTimer>
#include <limits>
#include "fiber.h"
#include "context.h"
#include "wakecondition.h"

#define MIN_TICK_INTERVAL 150

using namespace CoQt;

Scheduler::Scheduler(Context *parent)
    : QObject(parent)
    , m_pContext(parent)
    , m_pTickTImer(new QTimer(this))
{
    connect(m_pTickTImer, &QTimer::timeout, this, &Scheduler::runFibers);
    m_pTickTImer->setSingleShot(true);
}

Scheduler::~Scheduler()
{

}

void Scheduler::scheculeFiber(WakeCondition *pCondition)
{
    m_lstWakeConditions << pCondition;
    calculateNextTick();
}

void Scheduler::unscheculeFiber(WakeCondition *pCondition)
{
    m_lstWakeConditions.removeAll(pCondition);
    calculateNextTick();
}

void Scheduler::runFibers()
{
    QList<QPointer<WakeCondition> > lstWaitConditions = m_lstWakeConditions;

    foreach(QPointer<WakeCondition> pCondition, lstWaitConditions)
    {
        if(!pCondition)
            continue;

        if(pCondition->canWake())
        {
            pCondition->wake();
        }
    }

    calculateNextTick();
}

void Scheduler::calculateNextTick()
{
    int iMSec = std::numeric_limits<int>::max();
    QList<QPointer<WakeCondition> > lstWaitConditions = m_lstWakeConditions;

    foreach(QPointer<WakeCondition> pCondition, lstWaitConditions)
    {
        if(!pCondition)
            continue;

        int iTimeDiff = QDateTime::currentDateTime().msecsTo(pCondition->wakeTime());
        if(iTimeDiff < iMSec)
            iMSec = iTimeDiff;
    }

    if(iMSec == std::numeric_limits<int>::max()) //nothing to do
        return;

    //Ensure the nex scheduled time isn't less than our minimum schedular interval
    if(iMSec < MIN_TICK_INTERVAL)
        iMSec = MIN_TICK_INTERVAL;

    m_pTickTImer->start(iMSec);
}

