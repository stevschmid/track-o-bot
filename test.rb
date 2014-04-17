require 'phashion'
require 'fileutils'
require 'json'

TEMPLATES = './templates/*.png'

templates = {}

Dir.glob TEMPLATES do |f|
  templates[File.basename(f)] = Phashion::Image.new(f)
end

# Take Screen
`screencapture -o -x -l$(./GetWindowID/GetWindowID "Hearthstone" "Hearthstone") tmp/screenshot_window.png`
# Crop Window Title
`convert -crop +0+22 +repage tmp/screenshot_window.png tmp/screenshot.png`

CARD_OFFSETS = {
  left: {x: 153, y: 198},
  middle: {x: 353, y: 198},
  right: {x: 551, y: 198},
}

cards = {}

CARD_OFFSETS.each do |name, offset|
  `convert -crop 75x75+#{offset[:x]}+#{offset[:y]} tmp/screenshot.png tmp/card_#{name}.png`
  cards[name] = Phashion::Image.new("tmp/card_#{name}.png")
end

metadata = JSON.parse(File.read('../hearth_cards_generator/textures_and_json/output/cards.json'), symbolize_names: true)

cards.each do |card_name, card_image|
  templates.each do |template_name, template_image|
    if card_image.duplicate?(template_image)
      card = metadata.find { |c| c[:id] == template_name.gsub('.png','') }
      p "#{card_name} is #{card[:name]}"
    end
  end
end


class BasicScene
  attr_reader :objects

  private

  def add_object(name, opts = {})
    @objects ||= {}
    @objects[name] = opts
  end

  def contains?(object, template = nil)
    # extract object from screen
    img = Screen.capture(

    # match object to template
  end
end

class ArenaScene < BasicScene
  def initialize
    add_object :card_left, x: 153, y: 198
    add_object :card_center, x: 353, y: 198
    add_object :card_right, x: 551, y: 198

    add_object :cross1
    add_object :cross2
    add_object :cross3

    add_object :score

    add_object :class
  end

  def current?

  end

end
