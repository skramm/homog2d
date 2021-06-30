#!/bin/bash

# This files generates captions images for the demo video.

#-pointsize 20
#-size 400x200

function gen_img
{
convert -size 1000x1000 xc:white +repage \
 -pointsize 50 -fill blue -background None  \
 -font CourierNewB -gravity center caption:"$text" +repage \
 -gravity Center  -composite -strip  tmp/im_$1.jpg
}
mkdir -p tmp

text="Red and blue quadrangles define the 2x4 points used to compute the homography"
gen_img H1

text="Green rectangle and pair of segments show the effect of the homography"
gen_img H2

text="Red segment is computed from the two circle centers, using getSegment( c1, c2 )"
gen_img C1

text="The yellow and blue segments are computed with getTanSegs( c1, c2 )"
gen_img C2

text="The red crosses show computed intersection points with the 3 gray lines"
gen_img C3

text="The green circle will turn orange if inside the rectangle or the other circle"
gen_img C4

text="The rectangle will turn orange if inside the green circle"
gen_img C5


