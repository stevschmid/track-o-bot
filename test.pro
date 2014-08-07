include(track-o-bot.pro)

CONFIG += console
CONFIG -= app_bundle

TARGET = test
TEMPLATE = app

DEFINES += _TEST

GMOCKPATH = ../gmock-1.7.0
GTESTPATH = ../gmock-1.7.0/gtest

INCLUDEPATH += src \
               test \
               $$GTESTPATH \
               $$GTESTPATH/include \
               $$GMOCKPATH \
               $$GMOCKPATH/include

# Ignore forever definition of Qt (otherwise clash with gtest)
DEFINES += "forever=forever"

GMOCK_HEADERS = $$GMOCKPATH/include/gmock/*.h \
                $$GMOCKPATH/include/gmock/internal/*.h
                
GTEST_HEADERS = $$GTESTPATH/include/gtest/*.h \
                $$GTESTPATH/include/gtest/internal/*.h

HEADERS = $$GTEST_HEADERS \
          $$GMOCK_HEADERS \
          src/Local.h \
          src/OSXWindowCapture.h \
          src/Logger.h

SOURCES = $$GMOCKPATH/src/gmock-all.cc \
          $$GTESTPATH/src/gtest-all.cc \
          $$GMOCKPATH/src/gmock_main.cc \
          test/*Test.cpp \
          src/OSXWindowCapture.cpp \
          src/Hearthstone.cpp \
          src/Logger.cpp
