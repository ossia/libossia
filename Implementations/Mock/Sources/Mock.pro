TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXX = clang-3.5
QMAKE_CXXFLAGS += -std=c++1y -stdlib=libc++
QMAKE_LFLAGS += -lc++ -lpthread

INCLUDEPATH += ../../API/
SOURCES += \
    Event.cpp \
    later.cpp \
    TimeBox.cpp \
    Scenario.cpp \
    TimeNode.cpp \
    TimeValue.cpp \
    IObservable.cpp \
    TimeProcess.cpp

HEADERS += \
    later.h

