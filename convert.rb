FOLDER = '/Users/spidy/dev/hearth_cards_generator/textures_and_json/output'

RATIO_PERCENT = 100*(109.to_f / 217) # EX1_093 as example. ingame width 1027x768 res, generated width 217

Dir.glob File.join(FOLDER, '*.png') do |src|
  filename = File.basename(src)
  dest = "./templates/#{filename}"
  `convert -resize #{RATIO_PERCENT}% #{src} #{dest}`
  `convert +repage -gravity center -crop 75x75+0+0 #{dest} #{dest}`
end


