# preliminary
# build mp4 video from still images
id=13
cd ../../BUILD/showcase
ffmpeg -y -framerate 8 -s 400x400 -i showcase${id}_%02d.png showcase${id}.mp4
