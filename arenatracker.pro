CONFIG += qt console precompile_header debug
CONFIG -= app_bundle

PRECOMPILED_HEADER = src/local.h

DESTDIR = build
OBJECTS_DIR = tmp

HEADERS = src/local.h \
          src/window_capture.h \
          src/hearthstone.h \
          src/dhasher.cpp

SOURCES = src/main.cpp \
          src/hearthstone.cpp \
          src/dhasher.cpp

mac {
  HEADERS += src/osx_window_capture.h
  SOURCES += src/osx_window_capture.cpp

  LIBS += -framework ApplicationServices
}
