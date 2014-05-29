#!/usr/bin/env ruby

require 'fileutils'

APP_DIR = 'build/Track-o-Bot.app'
QT_LIB_DIR = `qmake -query QT_INSTALL_LIBS`.strip
SIGNATURE = "Developer ID Application: Steven Schmid"

Dir.chdir File.expand_path('..', File.dirname(__FILE__))

# Purge build
`rm -rf build/*.app`

# Create .app
version = File.read('track-o-bot.pro').match(/VERSION = (\d+\.\d+\.\d+)/)[1]

`qmake track-o-bot.pro && make clean && make release`

# Copy frameworks into .app
`macdeployqt #{APP_DIR}`

# 3rd party
`cp -R -H -f /Library/Frameworks/Sparkle.framework #{APP_DIR}/Contents/Frameworks`

# Copy missing plist files
frameworks = File.join(APP_DIR, 'Contents', 'Frameworks')
Dir.foreach(frameworks) do |framework|
  next unless framework.match(/^Qt.*\.framework$/)
  path = File.join(frameworks, framework)

  dst = File.join(path, 'Resources', 'Info.plist')
  src = File.join(QT_LIB_DIR, framework, 'Contents', 'Info.plist')

  FileUtils.cp(src, dst)
end

# Codesign frameworks
Dir.glob('**/*.framework') do |f|
  `codesign --verbose --sign "#{SIGNATURE}" #{f}`
end

# Codesign .app
`codesign --verbose --sign "#{SIGNATURE}" #{APP_DIR}`

`codesign --verify --verbose=4 #{APP_DIR}`
`spctl --verbose=4 --assess --type execute #{APP_DIR}`

puts "Creating DMG"
`rm dist/Track-o-Bot_#{version}.dmg`
`appdmg dist/track-o-bot.json dist/Track-o-Bot_#{version}.dmg`

filesize = File.size("dist/Track-o-Bot_#{version}.dmg")

puts "Appcast entry"

pubdate = Time.now.strftime('%a, %d %b %Y %H:%M:%S %z')

puts %Q[
  <item>
    <title>Version #{version}</title>
    <description><![CDATA[
      ]]></description>
    <pubDate>#{pubdate}</pubDate>
    <enclosure url="https://github.com/stevschmid/track-o-bot/releases/download/#{version}/Track-o-Bot_#{version}.dmg" sparkle:version="#{version}" length="#{filesize}" type="application/octet-stream"/>
  </item>
]

