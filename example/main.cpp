#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include <QtConcurrentRun>
#include <QTimer>
#include "fiber.h"
#include "scheduler.h"
#include "context.h"
#include "wakecondition.h"

//Simple fiber example showing a yield and manual wake
void testSimpleFiber()
{
    qDebug() << "Before Fiber";
    QSharedPointer<CoQt::Fiber> fiber = CoQt::createFiber([&]() {
        qDebug() << "Starting Fiber";

        CoQt::Fiber::yieldForever();

        qDebug() << "Middle of fiber";

        CoQt::Fiber::yieldForever();

        qDebug() << "Ending Fiber";
    });

    qDebug() << "Wake1";
    fiber->wake();
    qDebug() << "Wake2";
    fiber->wake();
    qDebug() << "Fiber is done";
}

//Fiber counts to five at the minimum tick interval
void testYield()
{
    CoQt::createFiber([&]() {
       for(int i = 0; i < 5; i++)
       {
           qDebug() << "testYield:" << i;
           CoQt::Fiber::yield(); //return after 1 scheduler tick
       }
    });
}

//Shows a fiber sleeping for the requested number of seconds
void testSleep()
{
    CoQt::createFiber([&]() {
       qDebug() << "Sleeping for 5 seconds";

       //Yield the fiber for 5 seconds
       CoQt::Fiber::yield(5000);
       qDebug() << "Done";
    });
}

//Test waking a fiber with a lambda function
void testLambda()
{
    CoQt::createFiber([&]() {
       qDebug() << "will waik when i == 5";

       int i = 0;
       //Run a lambda function on each scheduler
       //tick, and wake the fiber when it returns true
       CoQt::Fiber::yield([&]() {
           i++;

           return (i == 5);
       }, 10);

       qDebug() << "Done";
    });
}

QSharedPointer<CoQt::Fiber> pFiber1;

//Test multiple nested/related fibers
void testNestedFibers()
{
    pFiber1 = CoQt::createFiber([&]() {

       CoQt::createFiber([&]() {
           int i = 0;
           while(true) {
               i++;
               qDebug() << "3: " << i;
               CoQt::Fiber::yield();
           }
       });

       int i = 0;
       while(true) {
           i++;
           qDebug() << i;
           CoQt::Fiber::yield();
       }
    });

    CoQt::createFiber([&]() {
       int i = 0;
       while(true) {
           i++;
           qDebug() << "2: " << i;
           pFiber1->wake();
           CoQt::Fiber::yield();
       }
    });

    CoQt::createFiber([&]() {
       int i = 0;
       while(true) {
           i++;
           qDebug() << "FOUR: " << i;
           CoQt::Fiber::yield(2000);
       }
    });

    CoQt::createFiber([&]() {
       int i = 0;
       while(true) {
           i++;
           qDebug() << "LAMBDA: " << i;
           int j = 0;
           CoQt::Fiber::yield([&]() {
               j++;
               return (j == 40);
           });
       }
    });
}

//Test the QFuture wake condition
//uses QtConcurrent::run() to do a task in another thread
//and pause the fiber while the thread is running. Once
//the thread returns, the results of the QFuture are available
void testQFuture()
{
    CoQt::createFiber([&]() {
       qDebug() << "Before Thread";

       //Start a new thread, wait for it to complete
       QFuture<int> future = CoQt::Fiber::yield(QtConcurrent::run([]() {
            int i = 0;

            //Currently running in a seperate thread
            qDebug() << "In Thread";

            for(i = 0; i < 123; i++)
            {
                QThread::msleep(25);
            }

            qDebug() << "Returning";

            return i; //Thread ends here
       }));

       //Once we get here, the thread is complete, and the QFuture
       //has the results
       qDebug() << "Result:" << future.result();

       //Just a test to see if we can exit an application from a fiber
       QCoreApplication::quit();
    });
}

//Start a fiber, yield, and resume when a signal is emitted
void testSignalWakeStyle1()
{
    CoQt::createFiber([&]() {
       QTimer t;
       t.setSingleShot(true);
       t.start(3000);

       qDebug() << "Testing signal wake style 2";

       //Wake the fiber when the specified signal is emitted
       CoQt::Fiber::yield(&t, SIGNAL(timeout()));

       qDebug() << "Done";
    });
}

//Start a fiber, yield, and resume when a signal is emitted
void testSignalWakeStyle2()
{
    CoQt::createFiber([&]() {
       QTimer t;
       t.setSingleShot(true);
       t.start(3000);

       qDebug() << "Testing signal wake style 2";

       CoQt::SignalWakeCondition waker(CoQt::context()->curFiber());
       QObject::connect(&t, &QTimer::timeout, &waker, &CoQt::SignalWakeCondition::wake);

       //Will yield the fiber indefinately. However, the QTimer will wake it in about 3 seconds
       CoQt::Fiber::yieldForever();

       qDebug() << "Done";
    });
}

int main(int argc, char * argv[])
{
    QCoreApplication app(argc, argv);

    //Set the tick time to something super long so it's easy
    //to see what's happening
    CoQt::context()->scheduler()->setMinimumTickTime(250);


    qDebug();
    qDebug() << "Simple Fiber Test";
    qDebug() << "==========================";
    testSimpleFiber();

    qDebug();
    qDebug() << "Simple Yield Test";
    qDebug() << "==========================";
    testYield();

    qDebug();
    qDebug() << "Simple Sleep Test";
    qDebug() << "==========================";
    testSleep();

    qDebug();
    qDebug() << "Simple Lambda Test";
    qDebug() << "==========================";
    testLambda();

    qDebug() << "Running the event loop";
    app.exec();
    qDebug() << "Exiting...";
}

