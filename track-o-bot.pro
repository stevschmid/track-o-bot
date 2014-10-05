TARGET = Track-o-Bot
VERSION = 0.4.3

CONFIG += qt precompile_header debug_and_release
QT += network

DESTDIR = build
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

PRECOMPILED_HEADER = src/Local.h
HEADERS = src/Local.h \
          src/Window.h \
          src/Core.h \
          src/Logger.h \
          src/Tracker.h \
          src/Scenes/IngameScene.h \
          src/HearthstoneLogWatcher.h \
          src/HearthstoneLogTracker.h \
          src/Hearthstone.h

SOURCES = src/Main.cpp \
          src/Hearthstone.cpp \
          src/Phash.cpp \
          src/SceneManager.cpp \
          src/Tracker.cpp \
          src/Window.cpp \
          src/Core.cpp \
          src/Logger.cpp \
          src/Json.cpp \
          src/Autostart.cpp \
          src/HearthstoneLogWatcher.cpp \
          src/HearthstoneLogTracker.cpp

FORMS   = src/Window.ui \
          src/SettingsWidget.ui \
          src/LogWidget.ui \
          src/AboutWidget.ui

DEFINES += VERSION=\\\"$$VERSION\\\"

RESOURCES += app.qrc
RESOURCES += markers.qrc

CONFIG(debug, debug|release): DEFINES += _DEBUG

mac {
  DEFINES += PLATFORM=\\\"mac\\\"

  HEADERS += src/OSXWindowCapture.h
  SOURCES += src/OSXWindowCapture.cpp

  LIBS += -framework ApplicationServices -framework Sparkle -framework AppKit

  OBJECTIVE_SOURCES += \
    src/SparkleUpdater.mm \
    src/CocoaInitializer.mm

  ICON = icons/logo.icns

  QMAKE_INFO_PLIST = Info.plist.app

  QMAKE_POST_LINK += /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $${VERSION}\" $${DESTDIR}/$${TARGET}.app/Contents/Info.plist;
  QMAKE_POST_LINK += /usr/libexec/PlistBuddy -c \"Set :CFBundleVersion $${VERSION}\" $${DESTDIR}/$${TARGET}.app/Contents/Info.plist;
}

win32 {
  CONFIG += embed_manifest_exe

  DEFINES += PLATFORM=\\\"win32\\\"

  INCLUDEPATH += . \
                 ../WinSparkle/include

  SOURCES += src/WinWindowCapture.cpp \
             src/WinSparkleUpdater.cpp

  DEFINES += _CRT_SECURE_NO_WARNINGS

  LIBS += user32.lib Gdi32.lib shell32.lib
  LIBS += -L../WinSparkle/Release

  QMAKE_PRE_LINK = ruby dist/win/patch_rc.rb
}

unix {
    DEFINES += PLATFORM=\\\"linux\\\"
    HEADERS += src/LinuxWindowCapture.h
    SOURCES += src/LinuxWindowCapture.cpp
    LIBS += -lXext -lX11 -L/usr/lib/x86_64-linux-gnu/
    CONFIG += link_pkgconfig
    PKGCONFIG += x11
    desktop.path = $$PREFIX/share/applications
    desktop.files += \
        assets/track-o-bot.desktop
    isEmpty(PREFIX){
        PREFIX = /usr/local
    }
    target.path = $$PREFIX/bin
    icon.path = $$PREFIX/share/icons/hicolor/256x256/apps
    icon.files += icons/Track-o-Bot.png
    INSTALLS += target desktop icon
}

