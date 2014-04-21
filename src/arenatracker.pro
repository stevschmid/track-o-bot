CONFIG += qt console precompile_header
CONFIG -= app_bundle

PRECOMPILED_HEADER = local.h

DESTDIR=../bin
OBJECTS_DIR=../tmp

HEADERS = local.h \
          window_capture.h \
          hearthstone.h \
          dhash.cpp

SOURCES = main.cpp \
          hearthstone.cpp \
          dhash.cpp

mac {
  HEADERS += osx_window_capture.h
  SOURCES += osx_window_capture.cpp

  LIBS += -framework ApplicationServices
}
