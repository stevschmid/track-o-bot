TARGET = Track-o-Bot
VERSION = 0.1.2

CONFIG += qt precompile_header debug_and_release
QT += network

DESTDIR = build
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp

PRECOMPILED_HEADER = src/local.h
HEADERS = src/local.h \
          src/window.h \
          src/core.h \
          src/logger.h \
          src/tracker.h \
          src/log_watcher.h \
          src/card_history.h

SOURCES = src/main.cpp \
          src/hearthstone.cpp \
          src/dhash.cpp \
          src/scene_manager.cpp \
          src/tracker.cpp \
          src/window.cpp \
          src/core.cpp \
          src/logger.cpp \
          src/json.cpp \
          src/autostart.cpp \
          src/log_watcher.cpp \
          src/card_history.cpp

DEFINES += VERSION=\\\"$$VERSION\\\"

RESOURCES += app.qrc
RESOURCES += markers.qrc

CONFIG(debug, debug|release): DEFINES += _DEBUG

mac {
  HEADERS += src/osx_window_capture.h
  SOURCES += src/osx_window_capture.cpp

  LIBS += -framework ApplicationServices -framework Sparkle -framework AppKit

  OBJECTIVE_SOURCES += \
    src/sparkle_updater.mm \
    src/cocoa_initializer.mm

  ICON = icons/logo.icns

  QMAKE_INFO_PLIST = Info.plist.app

  QMAKE_POST_LINK += /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $${VERSION}\" $${DESTDIR}/$${TARGET}.app/Contents/Info.plist;
  QMAKE_POST_LINK += /usr/libexec/PlistBuddy -c \"Set :CFBundleVersion $${VERSION}\" $${DESTDIR}/$${TARGET}.app/Contents/Info.plist;
}

win32 {
  SOURCES += src/win_window_capture.cpp

  LIBS += user32.lib Gdi32.lib
}
