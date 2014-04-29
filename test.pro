include(tracking.pro)

CONFIG += console
CONFIG -= app_bundle

GMOCKPATH = /Users/spidy/dev/gmock-1.7.0
GTESTPATH = /Users/spidy/dev/gmock-1.7.0/gtest

DEFINES += TEST_ENV

TEMPLATE = app
TARGET = test

INCLUDEPATH += src \
               test \
               $$GTESTPATH \
               $$GTESTPATH/include \
               $$GMOCKPATH \
               $$GMOCKPATH/include

DEFINES += QT_NO_KEYWORDS \
           _WCHAR_H_CPLUSPLUS_98_CONFORMANCE_

SOURCES -= src/main.cpp

# Input
GMOCK_HEADERS = $$GMOCKPATH/include/gmock/*.h \
                $$GMOCKPATH/include/gmock/internal/*.h
                
GTEST_HEADERS = $$GTESTPATH/include/gtest/*.h \
                $$GTESTPATH/include/gtest/internal/*.h

HEADERS += $$GTEST_HEADERS \
           $$GMOCK_HEADERS

SOURCES += $$GMOCKPATH/src/gmock-all.cc \
           $$GTESTPATH/src/gtest-all.cc \
           $$GMOCKPATH/src/gmock_main.cc \
           test/*_test.cpp
