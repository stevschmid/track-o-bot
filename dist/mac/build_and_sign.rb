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

# Codesign frameworks, plugins and app
`codesign --verbose --deep --sign "#{SIGNATURE}" #{APP_DIR}`
`codesign --verify --verbose=4 #{APP_DIR}`
`spctl --verbose=4 --assess --type execute #{APP_DIR}`

# puts "Creating DMG"
`rm dist/mac/Track-o-Bot_#{version}.dmg`
`appdmg dist/mac/track-o-bot.json dist/mac/Track-o-Bot_#{version}.dmg`

