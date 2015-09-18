#!/usr/bin/env ruby

require 'fileutils'

APP_DIR = 'build/Track-o-Bot.app'
QT_DIR = `qmake -query QT_INSTALL_PREFIX`.strip
SIGNATURE = "Developer ID Application: Steven Schmid"

Dir.chdir File.expand_path('../../', File.dirname(__FILE__))

# Create .app
version = File.read('track-o-bot.pro').match(/VERSION = (\d+\.\d+\.\d+)/)[1]

`rm -rf build/Track-o-Bot.app`
`qmake track-o-bot.pro && make clean && make release`

# Copy frameworks into .app
`macdeployqt #{APP_DIR} 2>/dev/null` # ERROR no file at "" is buggy https://github.com/Homebrew/homebrew/issues/42191
`mkdir -p tmp`
`git clone https://github.com/aurelien-rainone/macdeployqtfix.git tmp/macdeployqtfix 2>/dev/null`
puts `python tmp/macdeployqtfix/macdeployqtfix.py -nl -v #{APP_DIR}/Contents/MacOS/Track-o-Bot #{QT_DIR}`

# 3rd party
`cp -R -H -f /Library/Frameworks/Sparkle.framework #{APP_DIR}/Contents/Frameworks`

# Fix path
`install_name_tool -change @rpath/Sparkle.framework/Versions/A/Sparkle @executable_path/../Frameworks/Sparkle.framework/Versions/A/Sparkle #{APP_DIR}/Contents/MacOS/Track-o-Bot`

# What a mess with the broken macdeployqt...
# macdeployqtfix adds new dependencies which are not fixed (i.e.
# the Info.plist is not copied)
# So copy them over manually
frameworks = File.join(APP_DIR, 'Contents', 'Frameworks')
['QtDBus.framework', 'QtPrintSupport.framework'].each do |framework|
  path = File.join(frameworks, framework)
  puts path
  puts Dir.pwd
  # fix framework structure
  # https://developer.apple.com/library/mac/documentation/MacOSX/Conceptual/BPFrameworks/Concepts/FrameworkAnatomy.html
  Dir.chdir(path) do
    # Versions/Current -> Versions/4
    lib_version = Dir.entries('Versions')[2]
    lib = Dir.entries("Versions/#{lib_version}")[2]
    FileUtils.ln_sf lib_version, File.join('Versions', 'Current')

    # QtXXX -> Versions/Current/QtXXX
    FileUtils.ln_sf File.join('Versions', 'Current', lib), lib

    # Versions/4/Resources
    FileUtils.mkdir 'Resources' unless Dir.exists?('Resources')
    res_path = File.join('Versions', lib_version, 'Resources')
    FileUtils.mv 'Resources', res_path
    # Resources -> Versions/Current/Resources
    FileUtils.ln_sf File.join('Versions', 'Current', 'Resources'), 'Resources'

    # copy missing Info.plist
    # Versions/4/Resources/Info.plist
    src = File.join(QT_DIR, 'lib', framework, 'Versions', 'Current', 'Resources', 'Info.plist')
    FileUtils.cp(src, File.join(res_path, 'Info.plist'))
  end
end

# Codesign frameworks, plugins and app
`codesign --verbose --deep --sign "#{SIGNATURE}" #{APP_DIR}`
`codesign --verify --verbose=4 #{APP_DIR}`
`spctl --verbose=4 --assess --type execute #{APP_DIR}`

# puts "Creating DMG"
`rm dist/mac/Track-o-Bot_#{version}.dmg`
`appdmg dist/mac/track-o-bot.json dist/mac/Track-o-Bot_#{version}.dmg`

