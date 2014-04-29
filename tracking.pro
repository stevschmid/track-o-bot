CONFIG += qt precompile_header debug

QT += core network widgets macextras

PRECOMPILED_HEADER = src/local.h

DESTDIR = build
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp

TARGET = Tracking

HEADERS = src/local.h \
          src/window_capture.h \
          src/hearthstone.h \
          src/dhash.h \
          src/scene_manager.h \
          src/tracker.h \
          src/window.h \
          src/core.h \
          src/logger.h \
          src/autostart.h

SOURCES = src/main.cpp \
          src/hearthstone.cpp \
          src/dhash.cpp \
          src/scene_manager.cpp \
          src/tracker.cpp \
          src/window.cpp \
          src/core.cpp \
          src/logger.cpp \
          src/autostart.cpp

RESOURCES = arenatracker.qrc

QMAKE_INFO_PLIST = Info.plist.app

mac {
  HEADERS += src/osx_window_capture.h
  SOURCES += src/osx_window_capture.cpp

  ICON = icons/paw.icns

  LIBS += -framework ApplicationServices
}
