# Build mp4 videos from still images, and generate those images
# Requires ffmpeg, tested with version 4.2.7

# The tasks to be done are in misc/showcase_v/run_parameters.txt

# Run from root level of repo with:
# $ misc/showcase_v/build_vid.sh

# You need to build the apps before, this is done with:
# $ make showcase2

# run programs (will generate png images in BUILD/showcase2) and build videos
# note: ffmpeg option -nostdin is there to avoid bash variable values to be crunched, see https://unix.stackexchange.com/a/718279/73337

mkdir -p BUILD/showcase2/vid/
rm BUILD/showcase2/vid/*.mp4
cd BUILD/showcase2/
while read -r fname app arg1 arg2 arg3;
do
	if [ "$fname" != "" ] && [ "$fname" != "#" ]; then
		./$app $arg1 $arg2 $arg3
		mkdir -p png_${fname}
	 	rm png_${fname}/*.png
	 	mv *.png png_${fname}/
	   ffmpeg -nostdin -y -framerate 12 -s 400x400 -i png_${fname}/${app}_%03d.png vid/${fname}.mp4 1>ffmpeg_${fname}.stdout 2>ffmpeg_${fname}.stderr
	fi
done < ../../misc/showcase_v/run_parameters.txt



