#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include "fiber.h"
#include "scheduler.h"
#include "context.h"

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
           CoQt::Fiber::yield();
       }
    });
}

//Shows a fiber sleeping for the requested number of seconds
void testSleep()
{
    CoQt::createFiber([&]() {
       qDebug() << "Sleeping for 5 seconds";
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

int main(int argc, char * argv[])
{
    QCoreApplication app(argc, argv);

    //Set the tick time to something super long so it's easy
    //to see what's happening
    CoQt::context()->scheduler()->setMinimumTickTime(250);

    /*
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
*/

    testNestedFibers();

    qDebug() << "Hello World!";
    return app.exec();
}

