
include(priority2_vkb_sliding_press/priority2_vkb_sliding_press.pri)

IMAGES_DATA = theme/meegotouch-keyboard.svg
images_data.path = /usr/share/themes/base/meegotouch/svg
images_data.files = $$IMAGES_DATA

CSS_DATA = theme/libmeego-keyboard.css
css_data.path = /usr/share/themes/base/meegotouch/libmeego-keyboard/style
css_data.files = $$CSS_DATA

INSTALLS += \
    images_data \
    css_data \
