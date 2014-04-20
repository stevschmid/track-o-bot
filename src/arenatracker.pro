CONFIG += qt precompile_header

PRECOMPILED_HEADER = local.h

DESTDIR=../bin
OBJECTS_DIR=../tmp

HEADERS = local.h \
          window_capture.h \
          osx_window_capture.h

SOURCES = main.cpp \
          osx_window_capture.cpp
