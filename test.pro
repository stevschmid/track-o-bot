include(arenatracker.pro)

GTESTPATH = /Users/spidy/dev/googletest

TEMPLATE = app
TARGET = test

INCLUDEPATH += src \
               $$GTESTPATH \
               $$GTESTPATH/include

DEFINES += QT_NO_KEYWORDS \
           _WCHAR_H_CPLUSPLUS_98_CONFORMANCE_

SOURCES -= src/main.cpp

# Input
GTEST_HEADERS = $$GTESTPATH/include/gtest/*.h \
                $$GTESTPATH/include/gtest/internal/*.h

HEADERS += $$GTEST_HEADERS

SOURCES += $$GTESTPATH/src/gtest-all.cc \
           $$GTESTPATH/src/gtest_main.cc \
           test/*_test.cpp
