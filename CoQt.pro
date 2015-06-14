TEMPLATE = app
CONFIG += c++11
QT += concurrent

SOURCES += main.cpp \
    fiber.cpp \
    scheduler.cpp \
    context.cpp \
    wakecondition.cpp

#fiber backends
win32 {
    SOURCES += fiber_win32.cpp
}
else:unix:!macx {
    SOURCES += fiber_unix.cpp
}
else {
    INCLUDEPATH += "/usr/local/Cellar/boost/1.58.0/include/"
    LIBS += "-L/usr/local/Cellar/boost/1.58.0/lib/"
    LIBS += -lboost_coroutine-mt -lboost_system-mt

    SOURCES += fiber_boost.cpp
}

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    fiber.h \
    scheduler.h \
    context.h \
    wakecondition.h \
    fiber_p.h

