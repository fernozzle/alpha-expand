alpha-expand
============

A simple tool for coloring pixels with 0% alpha for better mipmapping.

Because PNG images are not premultiplied, pixels that are completely transparent still need to have a definite color. Programs like Photoshop or Illustrator usually set these invisible pixels to be white or black, causing white or black fringes to bleed into visible pixels when images are downsized for mipmapping. This tool sets invisible pixels to the color of the nearest visible pixel, ensuring that downsampling does not blend in colors not originally present in the image.

In Windows, you can drag multiple files onto the executable and it'll process them, pausing to keep the terminal open if there's an error.
