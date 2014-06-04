#!/usr/bin/env ruby

# Patch icon into the RC
# This is beyond ugly, but the Qt folk seem to think
# that incorporating the auto-generated RC with the VERSION in the pro file
# AND an icon for the executable is a use case which is "Out of scope"
# see https://bugreports.qt-project.org/browse/QTBUG-13400
File.open('Track-o-Bot_resource.rc', 'a') do |f|
  f.puts "IDI_ICON1   ICON    DISCARDABLE icons/win.ico"
end

# Recompile
`rc -fo tmp/Track-o-Bot_resource.res Track-o-Bot_resource.rc`
