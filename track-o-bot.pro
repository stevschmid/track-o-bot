TARGET = Track-o-Bot
VERSION = 0.5.0

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
          src/HearthstoneLogWatcher.h \
          src/HearthstoneLogTracker.h \
          src/NeuralNetwork.h \
          src/RankClassifierData.h \
          src/RankClassifier.h

SOURCES = src/Main.cpp \
          src/Hearthstone.cpp \
          src/Tracker.cpp \
          src/Window.cpp \
          src/Core.cpp \
          src/Logger.cpp \
          src/Json.cpp \
          src/Autostart.cpp \
          src/HearthstoneLogWatcher.cpp \
          src/HearthstoneLogTracker.cpp \
          src/NeuralNetwork.cpp \ 
          src/RankClassifier.cpp

FORMS   = src/Window.ui \
          src/SettingsWidget.ui \
          src/LogWidget.ui \
          src/AboutWidget.ui

DEFINES += VERSION=\\\"$$VERSION\\\"

RESOURCES += app.qrc

CONFIG(debug, debug|release): DEFINES += _DEBUG

mac {
  DEFINES += PLATFORM=\\\"mac\\\"

  HEADERS += src/OSXWindowCapture.h src/OSXLocal.h
  SOURCES += src/OSXWindowCapture.cpp

  LIBS += -framework ApplicationServices -framework Sparkle -framework AppKit

  OBJECTIVE_SOURCES += \
    src/SparkleUpdater.mm \
    src/CocoaInitializer.mm \
    src/OSXLocal.mm

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
