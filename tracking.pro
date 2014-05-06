TARGET = Tracking

CONFIG += qt precompile_header debug
QT += network

DESTDIR = build
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp

PRECOMPILED_HEADER = src/local.h
HEADERS = src/local.h \
          src/window.h \
          src/core.h \
          src/tracker.h

SOURCES = src/main.cpp \
          src/hearthstone.cpp \
          src/dhash.cpp \
          src/scene_manager.cpp \
          src/tracker.cpp \
          src/window.cpp \
          src/core.cpp \
          src/logger.cpp \
          src/json.cpp \
          src/autostart.cpp

RESOURCES = tracking.qrc

mac {
  HEADERS += src/osx_window_capture.h
  SOURCES += src/osx_window_capture.cpp

  LIBS += -framework ApplicationServices

  ICON = icons/paw.icns

  # Custom Info.plist to make systray app only
  QMAKE_INFO_PLIST = Info.plist.app
}
