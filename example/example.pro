TEMPLATE = app
CONFIG += c++11
QT += concurrent

SOURCES += main.cpp
HEADERS +=

include(deployment.pri)
qtcAddDeployment()

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lCoQt
win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lCoQt
else:unix: LIBS += -L$$OUT_PWD/../lib/ -llib

INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib
